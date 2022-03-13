#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <thh-map/thh-map.hpp>

TEST_CASE("CanAllocContainer")
{
  thh::dense_map_t<char, char> dense_map;
  CHECK(true);
}
