#include <thh-map/thh-map.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>

struct object_t
{
  int64_t data_[128];
};

static void iterate_lookup(benchmark::State& state)
{
  thh::lookup_table_t<std::string, object_t> look_up_table;
  for (int i = 0; i < 128; ++i) {
    look_up_table.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    std::for_each(look_up_table.begin(), look_up_table.end(), [](auto& value) {
      value.data_[32] = 1234;
    });
    benchmark::ClobberMemory();
  }
}

BENCHMARK(iterate_lookup);

static void iterate_map(benchmark::State& state)
{
  std::unordered_map<std::string, object_t> look_up_map;
  for (int i = 0; i < 128; ++i) {
    look_up_map.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    std::for_each(look_up_map.begin(), look_up_map.end(), [](auto& value) {
      value.second.data_[32] = 1234;
    });
    benchmark::ClobberMemory();
  }
}

BENCHMARK(iterate_map);

static void find_lookup(benchmark::State& state)
{
  thh::lookup_table_t<std::string, object_t> look_up_table;
  for (int i = 0; i < 128; ++i) {
    look_up_table.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    look_up_table.call("name24", [](auto& value) { value.data_[32] = 1234; });
    benchmark::ClobberMemory();
  }
}

BENCHMARK(find_lookup);

static void find_map(benchmark::State& state)
{
  std::unordered_map<std::string, object_t> look_up_map;
  for (int i = 0; i < 128; ++i) {
    look_up_map.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    if (auto found = look_up_map.find("name24"); found != look_up_map.end()) {
      found->second.data_[32] = 1234;
    }
    benchmark::ClobberMemory();
  }
}

BENCHMARK(find_map);

BENCHMARK_MAIN();
