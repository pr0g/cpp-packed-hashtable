#include <thh-packed-hashtable/packed-hashtable.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>
#include <random>

// c++20 erase_if stand-in
template<
  class Key, class T, class Hash, class KeyEqual, class Alloc, class Pred>
typename std::unordered_map<Key, T, Hash, KeyEqual, Alloc>::size_type erase_if(
  std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& c, Pred pred)
{
  auto old_size = c.size();
  for (auto it = c.begin(), last = c.end(); it != last;) {
    if (pred(*it)) {
      it = c.erase(it);
    } else {
      ++it;
    }
  }
  return old_size - c.size();
}

template<typename T>
T random_between(T range_from, T range_to)
{
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_int_distribution<T> distribution(range_from, range_to);
  return distribution(generator);
}

template<int32_t Size>
struct object_t
{
  char data_[Size];
};

template<typename T>
static void iterate_packed_hashtable_values(benchmark::State& state)
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
static void iterate_packed_hashtable_handles(benchmark::State& state)
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

template<typename T>
static void iterate_unordered_map(benchmark::State& state)
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

struct vec3_t
{
  float x, y, z;
};

struct color_t
{
  float r, g, b, a;
};

// 48 bytes
struct particle_t
{
  vec3_t position_;
  vec3_t velocity_;
  color_t color_;
  float size_;
  float lifetime_;
};

static void iterate_packed_hashtable_values_particles(benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, particle_t> packed_hashtable_particles;
  packed_hashtable_particles.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    packed_hashtable_particles.add(
      {std::string("name") + std::to_string(i), particle_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    std::for_each(
      packed_hashtable_particles.vbegin(), packed_hashtable_particles.vend(),
      [](auto& particle) {
        particle.position_.x += particle.velocity_.x;
        particle.position_.y += particle.velocity_.y;
        particle.position_.z += particle.velocity_.z;
        particle.color_.r = std::max(0.0f, particle.color_.r - 0.1f);
        particle.color_.g = std::max(0.0f, particle.color_.g - 0.1f);
        particle.color_.b = std::max(0.0f, particle.color_.b - 0.1f);
        particle.lifetime_ -= 0.01666f;
        particle.size_ += 0.01f;
      });
    benchmark::DoNotOptimize(packed_hashtable_particles);
  }
}

BENCHMARK(iterate_packed_hashtable_values_particles)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void iterate_packed_hashtable_handles_particles(benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, particle_t> packed_hashtable_particles;
  packed_hashtable_particles.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    packed_hashtable_particles.add(
      {std::string("name") + std::to_string(i), particle_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    std::for_each(
      packed_hashtable_particles.hbegin(), packed_hashtable_particles.hend(),
      [&packed_hashtable_particles](const auto& key_handle) {
        packed_hashtable_particles.call(key_handle.second, [](auto& particle) {
          particle.position_.x += particle.velocity_.x;
          particle.position_.y += particle.velocity_.y;
          particle.position_.z += particle.velocity_.z;
          particle.color_.r = std::max(0.0f, particle.color_.r - 0.1f);
          particle.color_.g = std::max(0.0f, particle.color_.g - 0.1f);
          particle.color_.b = std::max(0.0f, particle.color_.b - 0.1f);
          particle.lifetime_ -= 0.01666f;
          particle.size_ += 0.01f;
        });
      });
    benchmark::DoNotOptimize(packed_hashtable_particles);
  }
}

BENCHMARK(iterate_packed_hashtable_handles_particles)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void iterate_unordered_map_values_particles(benchmark::State& state)
{
  std::unordered_map<std::string, particle_t> map_particles;
  map_particles.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    map_particles.insert(
      {std::string("name") + std::to_string(i), particle_t{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    std::for_each(
      map_particles.begin(), map_particles.end(), [](auto& particle) {
        particle.second.position_.x += particle.second.velocity_.x;
        particle.second.position_.y += particle.second.velocity_.y;
        particle.second.position_.z += particle.second.velocity_.z;
        particle.second.color_.r =
          std::max(0.0f, particle.second.color_.r - 0.1f);
        particle.second.color_.g =
          std::max(0.0f, particle.second.color_.g - 0.1f);
        particle.second.color_.b =
          std::max(0.0f, particle.second.color_.b - 0.1f);
        particle.second.lifetime_ -= 0.01666f;
        particle.second.size_ += 0.01f;
      });
    benchmark::DoNotOptimize(map_particles);
  }
}

BENCHMARK(iterate_unordered_map_values_particles)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void iterate_packed_hashtable_add_remove_values_particles(
  benchmark::State& state)
{
  thh::packed_hashtable_rl_t<std::string, particle_t>
    packed_hashtable_particles;
  packed_hashtable_particles.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    auto particle = particle_t{};
    particle.lifetime_ = i % 2 == 0 ? 1.0f : 0.0f;
    packed_hashtable_particles.add(
      {std::string("name") + std::to_string(i), particle});
  }

  for ([[maybe_unused]] auto _ : state) {
    thh::remove_when(packed_hashtable_particles, [](const auto& value) {
      return value.lifetime_ <= 0.0f;
    });
    benchmark::DoNotOptimize(packed_hashtable_particles);
  }
}

BENCHMARK(iterate_packed_hashtable_add_remove_values_particles)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void iterate_packed_hashtable_add_remove_handles_particles(
  benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, particle_t> packed_hashtable_particles;
  packed_hashtable_particles.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    auto particle = particle_t{};
    particle.lifetime_ = i % 2 == 0 ? 1.0f : 0.0f;
    packed_hashtable_particles.add(
      {std::string("name") + std::to_string(i), particle});
  }

  for ([[maybe_unused]] auto _ : state) {
    thh::remove_when(packed_hashtable_particles, [](const auto& value) {
      return value.lifetime_ <= 0.0f;
    });
    benchmark::DoNotOptimize(packed_hashtable_particles);
  }
}

BENCHMARK(iterate_packed_hashtable_add_remove_handles_particles)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void iterate_unordered_map_add_remove_particles(benchmark::State& state)
{
  std::unordered_map<std::string, particle_t> unordered_map_particles;
  unordered_map_particles.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    auto particle = particle_t{};
    particle.lifetime_ = i % 2 == 0 ? 1.0f : 0.0f;
    unordered_map_particles.insert(
      {std::string("name") + std::to_string(i), particle});
  }

  for ([[maybe_unused]] auto _ : state) {
    erase_if(unordered_map_particles, [](const auto& value) {
      return value.second.lifetime_ <= 0.0f;
    });
    benchmark::DoNotOptimize(unordered_map_particles);
  }
}

BENCHMARK(iterate_unordered_map_add_remove_particles)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void find_lookup(benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, object_t<32>> packed_hashtable;
  packed_hashtable.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(); ++i) {
    packed_hashtable.add(
      {std::string("name") + std::to_string(i), object_t<32>{}});
  }

  const auto lookup = std::string("name") + std::to_string(state.range(0) / 2);
  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    packed_hashtable.call(
      lookup, [&data_element](auto& value) { data_element = value.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

BENCHMARK(find_lookup)->RangeMultiplier(2)->Range(32, 8 << 13);

static void find_map(benchmark::State& state)
{
  std::unordered_map<std::string, object_t<32>> map;
  map.reserve(state.range());
  for (int i = 0; i < state.range(); ++i) {
    map.insert({std::string("name") + std::to_string(i), object_t<32>{}});
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

BENCHMARK(find_map)->RangeMultiplier(2)->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_unordered_map, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_handles, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_packed_hashtable_values, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_MAIN();

// for (int i = 0; i < state.range(0) / 2; ++i) {
//   packed_hashtable_particles.remove(
//     std::string("name")
//     + std::to_string(random_between((int)0, (int)state.range())));
// }
// for (int i = 0; i < state.range(0); ++i) {
//   packed_hashtable_particles.add(
//     {std::string("name")
//        + std::to_string(packed_hashtable_particles.size() + 1),
//      particle_t{}});
// }
