#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <thh-map/thh-map.hpp>

TEST_CASE("CanAllocContainer")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  CHECK(true);
}

TEST_CASE("ContainerSizeZeroAfterInit")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  const auto dense_map_size = packed_hashtable.size();
  CHECK(dense_map_size == 0);
}

TEST_CASE("ContainerEmptyAfterInit")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  CHECK(packed_hashtable.empty());
}

TEST_CASE("ContainerNotEmptyAfterInsert")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add(std::pair{5, 'a'});
  CHECK(!packed_hashtable.empty());
}

TEST_CASE("ContainerSizeIncreaseByOneAfterInsert")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add(std::pair{5, 'a'});
  CHECK(packed_hashtable.size() == 1);
}

// more add tests...

TEST_CASE("InsertingSameKeyTwiceOverwritesElementLvalue")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  auto first_key_value_pair = std::pair<const int, std::string>{2, "a"};
  auto second_key_value_pair = std::pair<const int, std::string>{2, "b"};
  packed_hashtable.add(first_key_value_pair);
  packed_hashtable.add(second_key_value_pair);
  packed_hashtable.call(2, [](const std::string& c) { CHECK(c == "b"); });
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("InsertingSameKeyTwiceOverwritesElementRvalue")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add({2, 'a'});
  packed_hashtable.add({2, 'b'});
  packed_hashtable.call(2, [](const char c) { CHECK(c == 'b'); });
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("dummy")
{
  struct object_t
  {
    object_t(int i, int j) : i_(i), j_(j) {}
    object_t(object_t&&) = default;
    object_t& operator=(object_t&&) = default;
    int i_;
    int j_;
  };

  thh::packed_hashtable_t<char, object_t> packed_hashtable;
  packed_hashtable.add({'a', object_t{1, 2}});

  packed_hashtable.call('a', [](const object_t& c) {
    CHECK(c.i_ == 1);
    CHECK(c.j_ == 2);
  });
}

TEST_CASE("VisitValuesViaHandleIteratorsAndKeyLookup")
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
