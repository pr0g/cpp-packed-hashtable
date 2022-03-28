#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <thh-map/thh-map.hpp>

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
