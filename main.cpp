#include <thh-map/thh-map.hpp>

#include <string>
#include <iostream>

int main(int argc, char** argv)
{
  struct object_t
  {
    int x;
    int y;
  };

  thh::lookup_table_t<std::string, object_t> lookup_table;

  lookup_table.insert({"hello", object_t{3, 4}});

  lookup_table.call(
    "hello",
    [](object_t& object) {
      std::cout << "object {" << object.x << ", " << object.y << "}\n";
    });

    return 0;
}
