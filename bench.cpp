#include <thh-packed-hashtable/packed-hashtable.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>

template<int32_t Size>
struct object_t
{
  char data_[Size];
};

template<typename T>
void iterate_lookup(benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, T> packed_hashtable;
  packed_hashtable.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    packed_hashtable.add({std::string("name") + std::to_string(i), T{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      packed_hashtable.vbegin(), packed_hashtable.vend(),
      [&data_element](auto& value) { data_element = value.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

template<typename T>
static void iterate_map(benchmark::State& state)
{
  std::unordered_map<std::string, T> map;
  map.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    map.insert({std::string("name") + std::to_string(i), T{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(map.begin(), map.end(), [&data_element](auto& value) {
      data_element = value.second.data_[0];
    });
    benchmark::DoNotOptimize(data_element);
  }
}

template<typename T>
static void iterate_lookup_handles(benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, T> packed_hashtable;
  packed_hashtable.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    packed_hashtable.add({std::string("name") + std::to_string(i), T{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      packed_hashtable.hbegin(), packed_hashtable.hend(),
      [&data_element, &packed_hashtable](const auto& key_handle) {
        packed_hashtable.call(key_handle.second, [&data_element](auto& value) {
          data_element = value.data_[0];
        });
      });
    benchmark::DoNotOptimize(data_element);
  }
}

static void find_lookup(benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, object_t> packed_hashtable;
  packed_hashtable.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(); ++i) {
    packed_hashtable.add({std::string("name") + std::to_string(i), object_t{}});
  }

  const auto lookup = std::string("name") + std::to_string(state.range(0) / 2);
  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    packed_hashtable.call(
      lookup, [&data_element](auto& value) { data_element = value.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

BENCHMARK(find_lookup)->RangeMultiplier(2)->Range(32, 8 << 12);

static void find_map(benchmark::State& state)
{
  std::unordered_map<std::string, object_t> map;
  map.reserve(state.range());
  for (int i = 0; i < state.range(); ++i) {
    map.insert({std::string("name") + std::to_string(i), object_t{}});
  }

  const auto lookup = std::string("name") + std::to_string(state.range(0) / 2);
  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    if (auto found = map.find(lookup); found != map.end()) {
      data_element = found->second.data_[0];
    }
    benchmark::DoNotOptimize(data_element);
  }
}

BENCHMARK(find_map)->RangeMultiplier(2)->Range(32, 8 << 12);

BENCHMARK_TEMPLATE(iterate_map, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_map, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup_handles, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_lookup, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_lookup, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_MAIN();
