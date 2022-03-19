#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <thh-map/thh-map.hpp>

TEST_CASE("CanAllocContainer")
{
  thh::dense_map_t<char, char> dense_map;
  CHECK(true);
}

TEST_CASE("ContainerSizeZeroAfterInit")
{
  thh::dense_map_t<char, char> dense_map;
  const auto dense_map_size = dense_map.size();
  CHECK(dense_map_size == 0);
}

TEST_CASE("ContainerEmptyAfterInit")
{
  thh::dense_map_t<char, char> dense_map;
  CHECK(dense_map.empty());
}

TEST_CASE("ContainerNotEmptyAfterInsert")
{
  thh::dense_map_t<int, char> dense_map;
  dense_map.insert(std::pair{5, 'a'});
  CHECK(!dense_map.empty());
}

TEST_CASE("ContainerSizeIncreaseByOneAfterInsert")
{
  thh::dense_map_t<int, char> dense_map;
  dense_map.insert(std::pair{5, 'a'});
  CHECK(dense_map.size() == 1);
}

TEST_CASE("ContainerSizeIncreaseByOneAfterTryEmplace")
{
  thh::dense_map_t<int, char> dense_map;
  dense_map.try_emplace(5, 'a');
  CHECK(dense_map.size() == 1);
}
