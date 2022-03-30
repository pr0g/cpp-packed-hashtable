#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <thh-packed-hashtable/packed-hashtable.hpp>

TEST_CASE("Can allocate packed hashtable")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  CHECK(true);
}

TEST_CASE("Container size zero after init")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  const auto dense_map_size = packed_hashtable.size();
  CHECK(dense_map_size == 0);
}

TEST_CASE("Container empty after init")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  CHECK(packed_hashtable.empty());
}

TEST_CASE("Container not empty after add")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add(std::pair{5, 'a'});
  CHECK(!packed_hashtable.empty());
}

TEST_CASE("Container size increase by one after add")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add(std::pair{5, 'a'});
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Elements can be reserved")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.reserve(10);
  CHECK(packed_hashtable.size() == 0);
  CHECK(packed_hashtable.capacity() == 10);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition (l-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_key_value_pair = std::pair<const int, std::string>{2, "a"};
  auto second_key_value_pair = std::pair<const int, std::string>{2, "b"};
  auto first_add = packed_hashtable.add(first_key_value_pair);
  auto second_add = packed_hashtable.add(second_key_value_pair);

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "a");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition and updates value (l-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_key_value_pair = std::pair<const int, std::string>{2, "a"};
  auto second_key_value_pair = std::pair<const int, std::string>{2, "b"};
  auto first_add = packed_hashtable.add_or_update(first_key_value_pair);
  auto second_add = packed_hashtable.add_or_update(second_key_value_pair);

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "b");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition (r-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_add = packed_hashtable.add({2, "a"});
  auto second_add = packed_hashtable.add({2, "b"});

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "a");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition and updates value (r-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_add = packed_hashtable.add_or_update({2, "a"});
  auto second_add = packed_hashtable.add_or_update({2, "b"});

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "b");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Move only type can be added to packed_hashtable")
{
  struct object_t
  {
    object_t(int i, int j) : i_(i), j_(j) {}
    object_t(object_t&&) = default;
    int i_;
    int j_;
  };

  thh::packed_hashtable_t<char, object_t> packed_hashtable;
  packed_hashtable.add({'a', object_t{1, 2}});

  bool called = false;
  packed_hashtable.call('a', [&called](const object_t& c) {
    called = true;
    CHECK(c.i_ == 1);
    CHECK(c.j_ == 2);
  });

  CHECK(called);
}

TEST_CASE("Visit values via handle iterators and key lookup")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({123, "test"});
  packed_hashtable.add({456, "the"});
  packed_hashtable.add({987, "iteration"});

  std::string expected = "testtheiteration";
  std::sort(expected.begin(), expected.end());

  std::string outcome;
  std::for_each(
    packed_hashtable.hbegin(), packed_hashtable.hend(),
    [&packed_hashtable, &outcome](const auto& key_handle) {
      packed_hashtable.call(
        key_handle.first,
        [&outcome](const std::string& value) { outcome += value; });
    });

  std::sort(outcome.begin(), outcome.end());
  CHECK(outcome == expected);
}

TEST_CASE("Visit values via handle iterators and handle lookup")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({123, "test"});
  packed_hashtable.add({456, "the"});
  packed_hashtable.add({987, "iteration"});

  std::string expected = "testtheiteration";
  std::sort(expected.begin(), expected.end());

  std::string outcome;
  std::for_each(
    packed_hashtable.hbegin(), packed_hashtable.hend(),
    [&packed_hashtable, &outcome](const auto& key_handle) {
      packed_hashtable.call(
        key_handle.second,
        [&outcome](const std::string& value) { outcome += value; });
    });

  std::sort(outcome.begin(), outcome.end());
  CHECK(outcome == expected);
}

TEST_CASE("Value can be removed using key")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({5, "hello"});
  packed_hashtable.add({8, "world"});

  packed_hashtable.remove(8);

  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Iterator to next handle is returned after remove")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({5, "hello"});
  packed_hashtable.add({8, "world"});

  const auto handle_it = packed_hashtable.remove(8);

  CHECK(handle_it->first == 5);
}

TEST_CASE("Values can be removed via iteration")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});
  packed_hashtable.add({4, "four"});
  packed_hashtable.add({5, "five"});
  packed_hashtable.add({6, "six"});

  // erase all odd numbers from table
  for (auto it = packed_hashtable.hbegin(); it != packed_hashtable.hend();) {
    if (it->first % 2 != 0) {
      it = packed_hashtable.remove(it);
    } else {
      ++it;
    }
  }

  std::string outcome;
  std::for_each(
    packed_hashtable.vbegin(), packed_hashtable.vend(),
    [&outcome](const auto& value) { outcome.append(value); });

  // sort characters to avoid any order dependence issues
  std::sort(outcome.begin(), outcome.end());

  CHECK(outcome == "fioorstuwx");
  CHECK(packed_hashtable.size() == 3);
}

TEST_CASE("Container has key after add")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  CHECK(packed_hashtable.has(1));
  CHECK(packed_hashtable.has(2));
  CHECK(packed_hashtable.has(3));
}

TEST_CASE("Container does not have key that wasn't added")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  CHECK(!packed_hashtable.has(4));
  CHECK(!packed_hashtable.has(5));
  CHECK(!packed_hashtable.has(6));
}
