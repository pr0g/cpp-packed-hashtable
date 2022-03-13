#include <thh-map/thh-map.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>

constexpr int size = 4096;
constexpr int half_size = size / 2;

struct object_t
{
  char data_[size];
};

static void iterate_lookup(benchmark::State& state)
{
  thh::lookup_table_t<std::string, object_t> look_up_table;
  look_up_table.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    look_up_table.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      look_up_table.begin(), look_up_table.end(),
      [&data_element](auto& value) { data_element = value.data_[half_size]; });
    benchmark::DoNotOptimize(data_element);
    benchmark::ClobberMemory();
  }
}

BENCHMARK(iterate_lookup)->RangeMultiplier(2)->Range(32, 8 << 12);

static void iterate_map(benchmark::State& state)
{
  std::unordered_map<std::string, object_t> look_up_map;
  look_up_map.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    look_up_map.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      look_up_map.begin(), look_up_map.end(), [&data_element](auto& value) {
        data_element = value.second.data_[half_size];
      });
    benchmark::DoNotOptimize(data_element);
    benchmark::ClobberMemory();
  }
}

BENCHMARK(iterate_map)->RangeMultiplier(2)->Range(32, 8 << 12);

static void find_lookup(benchmark::State& state)
{
  thh::lookup_table_t<std::string, object_t> look_up_table;
  look_up_table.reserve(state.range(0));
  for (int i = 0; i < state.range(); ++i) {
    look_up_table.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  const auto lookup = std::string("name") + std::to_string(half_size);
  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    look_up_table.call(lookup, [&data_element](auto& value) {
      data_element = value.data_[half_size];
    });
    benchmark::ClobberMemory();
  }
}

BENCHMARK(find_lookup)->RangeMultiplier(2)->Range(32, 8 << 12);

static void find_map(benchmark::State& state)
{
  std::unordered_map<std::string, object_t> look_up_map;
  look_up_map.reserve(state.range());
  for (int i = 0; i < state.range(); ++i) {
    look_up_map.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  const auto lookup = std::string("name") + std::to_string(half_size);
  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    if (auto found = look_up_map.find(lookup); found != look_up_map.end()) {
      data_element = found->second.data_[half_size];
    }
    benchmark::DoNotOptimize(data_element);
    benchmark::ClobberMemory();
  }
}

BENCHMARK(find_map)->RangeMultiplier(2)->Range(32, 8 << 12);

BENCHMARK_MAIN();
