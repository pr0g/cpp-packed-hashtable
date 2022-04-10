#include <thh-packed-hashtable/packed-hashtable.hpp>

#include <array>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

// debug memory tracking code
std::size_t g_total = 0;
void* operator new(std::size_t n)
{
  g_total += n;
  return malloc(n);
}

void operator delete(void* memory) noexcept
{
  free(memory);
}

template<int32_t Size>
void run_memory_tracking()
{
  struct object_t
  {
    char data_[Size];
  };

  const std::array sizes{32,   64,   128,  256,    512,    1024,
                         2048, 4096, 8192, 16'384, 32'768, 65'536};

  auto underline_fn = [](size_t length) { return std::string(length, '-'); };

  const std::string vector_name("std::vector");
  std::cout << vector_name << '\n' << underline_fn(vector_name.size()) << '\n';
  for (const int size : sizes) {
    std::vector<object_t> vec;
    vec.reserve(size);
    for (int i = 0; i < size; ++i) {
      vec.push_back(object_t{});
    }
    std::cout << std::left << std::setw(10) << g_total << std::right
              << std::setw(2) << '(' << size << ")\n";
    g_total = 0;
  }

  std::cout << '\n';

  const std::string unordered_map_name("std::unordered_map");
  std::cout << unordered_map_name << '\n'
            << underline_fn(unordered_map_name.size()) << '\n';
  for (const int size : sizes) {
    std::unordered_map<std::string, object_t> map;
    map.reserve(size);
    for (int i = 0; i < size; ++i) {
      map.insert(std::pair(std::to_string(i), object_t{}));
    }
    std::cout << std::left << std::setw(10) << g_total << std::right
              << std::setw(2) << '(' << size << ")\n";
    g_total = 0;
  }

  std::cout << '\n';

  const std::string map_name("std::map");
  std::cout << map_name << '\n' << underline_fn(map_name.size()) << '\n';
  for (const int size : sizes) {
    std::map<std::string, object_t> map;
    for (int i = 0; i < size; ++i) {
      map.insert(std::pair(std::to_string(i), object_t{}));
    }
    std::cout << std::left << std::setw(10) << g_total << std::right
              << std::setw(2) << '(' << size << ")\n";
    g_total = 0;
  }

  std::cout << '\n';

  // 16 bytes (per internal handles)
  // 4 bytes (per element id)
  // X bytes (per T)
  const std::string handle_vector_name("thh::handle_vector_t");
  std::cout << handle_vector_name << '\n'
            << underline_fn(handle_vector_name.size()) << '\n';
  for (const int size : sizes) {
    thh::handle_vector_t<object_t> handle;
    handle.reserve(size);
    for (int i = 0; i < size; ++i) {
      handle.add();
    }
    std::cout << std::left << std::setw(10) << g_total << std::right
              << std::setw(2) << '(' << size << ")\n";
    g_total = 0;
  }

  std::cout << '\n';

  const std::string packed_hashtable_name("thh::packed_hashtable_t");
  std::cout << packed_hashtable_name << '\n'
            << underline_fn(packed_hashtable_name.size()) << '\n';
  for (const int size : sizes) {
    thh::packed_hashtable_t<std::string, object_t> lookup_table;
    lookup_table.reserve(size);
    for (int i = 0; i < size; ++i) {
      lookup_table.add(std::pair(std::to_string(i), object_t{}));
    }
    std::cout << std::left << std::setw(10) << g_total << std::right
              << std::setw(2) << '(' << size << ")\n";
    g_total = 0;
  }

  std::cout << '\n';
}

int main(int argc, char** argv)
{
  run_memory_tracking<32>();
  run_memory_tracking<64>();
  run_memory_tracking<128>();
  run_memory_tracking<256>();
  run_memory_tracking<512>();
  run_memory_tracking<1024>();
  run_memory_tracking<2048>();
  run_memory_tracking<4096>();

  return 0;
}
