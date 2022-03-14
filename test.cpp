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
