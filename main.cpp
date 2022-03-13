#include <thh-map/thh-map.hpp>

#include <array>
#include <iostream>
#include <string>

// debug memory tracking code
int g_total = 0;
void* operator new(std::size_t n)
{
  g_total += n;
  return malloc(n);
}

void operator delete(void* memory) noexcept
{
  free(memory);
}

int main(int argc, char** argv)
{
  struct object_t
  {
    char data_[4096];
  };

  const std::array sizes{32,   64,   128,  256,    512,   1024,
                         2048, 4096, 8192, 16'384, 32'768};

  std::cout << "vector\n";
  for (const int size : sizes) {
    std::vector<object_t> vec;
    vec.reserve(size);
    for (int i = 0; i < size; ++i) {
      vec.push_back(object_t{});
    }
    std::cout << g_total << '\n';
    g_total = 0;
  }

  std::cout << '\n';

  std::cout << "unordered_map\n";
  for (const int size : sizes) {
    std::unordered_map<std::string, object_t> map;
    map.reserve(size);
    for (int i = 0; i < size; ++i) {
      map.insert(std::pair(std::to_string(i), object_t{}));
    }
    std::cout << g_total << '\n';
    g_total = 0;
  }

  std::cout << '\n';

  // 16 bytes (per internal handles)
  // 4 bytes (per element id)
  // X bytes (per T)
  std::cout << "handle\n";
  for (const int size : sizes) {
    thh::handle_vector_t<object_t> handle;
    handle.reserve(size);
    for (int i = 0; i < size; ++i) {
      auto h = handle.add();
    }
    std::cout << g_total << '\n';
    g_total = 0;
  }

  std::cout << '\n';

  std::cout << "lookup\n";
  for (const int size : sizes) {
    thh::lookup_table_t<std::string, object_t> lookup_table;
    lookup_table.reserve(size);
    for (int i = 0; i < size; ++i) {
      lookup_table.insert(std::pair(std::to_string(i), object_t{}));
    }
    std::cout << g_total << '\n';
    g_total = 0;
  }

  return 0;
}
