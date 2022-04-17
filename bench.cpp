#include <thh-packed-hashtable/packed-hashtable.hpp>

#include <absl/container/flat_hash_map.h>
#include <benchmark/benchmark.h>
#include <robin_hood.h>

#include <cstdint>
#include <random>

// c++20 erase_if stand-in
template<
  class Key, class T, class Hash, class KeyEqual, class Alloc, class Pred>
typename std::unordered_map<Key, T, Hash, KeyEqual, Alloc>::size_type erase_if(
  std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& container, Pred pred)
{
  const auto old_size = container.size();
  for (auto it = container.begin(), last = container.end(); it != last;) {
    if (pred(*it)) {
      it = container.erase(it);
    } else {
      ++it;
    }
  }
  return old_size - container.size();
}

template<typename T>
T random_between(const T from, const T to)
{
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_int_distribution<T> distribution(from, to);
  return distribution(generator);
}

template<int32_t Size>
struct object_t
{
  char data_[Size];
};

struct vec3_t
{
  float x, y, z;
};

struct color_t
{
  float r, g, b, a;
};

// note: somewhat representative object that may be stored in the container
// size: 48 bytes (arbitrary initial values)
struct particle_t
{
  vec3_t position_{1.0f, 2.0f, 3.0f};
  vec3_t velocity_{-1.0f, 1.0f, 1.0f};
  color_t color_{1.0f, 1.0f, 1.0f, 0.5f};
  float size_{1.0f};
  float lifetime_{1.0f};
};

template<typename T>
thh::packed_hashtable_t<std::string, T> populate_packed_hashtable(
  benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, T> packed_hashtable;
  packed_hashtable.reserve(static_cast<int32_t>(state.range(0)));
  for (int i = 0; i < state.range(0); ++i) {
    packed_hashtable.add({std::string("name") + std::to_string(i), T{}});
  }
  return packed_hashtable;
}

// iterate the packed hashtable using value iteration, reading the first byte of
// every element (object_t of varying size)
template<typename T>
static void iterate_object_t_in_packed_hashtable_by_value(
  benchmark::State& state)
{
  const thh::packed_hashtable_t<std::string, T> packed_hashtable =
    populate_packed_hashtable<T>(state);

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      packed_hashtable.vbegin(), packed_hashtable.vend(),
      [&data_element](auto& value) { data_element = value.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

// iterate the packed hashtable using handle iteration, reading the first byte
// of every element (object_t of varying size)
template<typename T>
static void iterate_object_t_in_packed_hashtable_by_handle(
  benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, T> packed_hashtable =
    populate_packed_hashtable<T>(state);

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

// iterate the packed hashtable using handle iteration (ensure handles are in a
// random order) reading the first byte of every element (object_t of varying
// size)
template<typename T>
static void iterate_object_t_in_packed_hashtable_by_handle_in_random_order(
  benchmark::State& state)
{
  thh::packed_hashtable_t<std::string, T> packed_hashtable =
    populate_packed_hashtable<T>(state);

  // remove half of the handles at random
  for (int i = 0; i < state.range(0) / 2; ++i) {
    // hacky random removal, not efficient
    while (true) {
      if (auto found = packed_hashtable.find(
            std::string("name")
            + std::to_string(random_between((int)0, (int)state.range(0))));
          found != packed_hashtable.hend()) {
        packed_hashtable.remove(found);
        break;
      }
    }
  }

  // add back the handles (they will fill up the random holes that were left)
  auto upper_range = state.range(0);
  for (int i = 0; i < state.range(0) / 2; ++i) {
    packed_hashtable.add(
      {std::string("name") + std::to_string(upper_range++), T{}});
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

// iterate the unordered_map using key/value iteration, reading the first byte
// of every element (object_t of varying size)
template<typename T>
static void iterate_object_t_in_unordered_map_by_key_value_pair(
  benchmark::State& state)
{
  std::unordered_map<std::string, T> unordered_map;
  unordered_map.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    unordered_map.insert({std::string("name") + std::to_string(i), T{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      unordered_map.begin(), unordered_map.end(),
      [&data_element](auto& value) { data_element = value.second.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

// iterate the flat_hash_map using key/value iteration, reading the first byte
// of every element (object_t of varying size)
template<typename T>
static void iterate_object_t_in_flat_hash_map_by_key_value_pair(
  benchmark::State& state)
{
  absl::flat_hash_map<std::string, T> flat_hash_map;
  flat_hash_map.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    flat_hash_map.insert({std::string("name") + std::to_string(i), T{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      flat_hash_map.begin(), flat_hash_map.end(),
      [&data_element](auto& value) { data_element = value.second.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

// iterate the robin hood unordered_flat_map using key/value iteration, reading
// the first byte of every element (object_t of varying size)
template<typename T>
static void iterate_object_t_in_unordered_flat_map_by_key_value_pair(
  benchmark::State& state)
{
  robin_hood::unordered_flat_map<std::string, T> flat_hash_map;
  flat_hash_map.reserve(state.range(0));
  for (int i = 0; i < state.range(0); ++i) {
    flat_hash_map.insert({std::string("name") + std::to_string(i), T{}});
  }

  for ([[maybe_unused]] auto _ : state) {
    char data_element = 0;
    std::for_each(
      flat_hash_map.begin(), flat_hash_map.end(),
      [&data_element](auto& value) { data_element = value.second.data_[0]; });
    benchmark::DoNotOptimize(data_element);
  }
}

// iterate the packed hashtable using value iteration, modifying every member of
// the element for each iteration
static void iterate_particle_t_in_packed_hashtable_by_value(
  benchmark::State& state)
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

BENCHMARK(iterate_particle_t_in_packed_hashtable_by_value)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

// iterate the packed hashtable using handle iteration, modifying every member
// of the element for each iteration
static void iterate_particle_t_in_packed_hashtable_by_handle(
  benchmark::State& state)
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

BENCHMARK(iterate_particle_t_in_packed_hashtable_by_handle)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

// iterate the unordered_map using key/value iteration, modifying every member
// of the element for each iteration
static void iterate_particle_t_in_unordered_map_by_key_value_pair(
  benchmark::State& state)
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

BENCHMARK(iterate_particle_t_in_unordered_map_by_key_value_pair)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void add_particle_t_in_packed_hashtable(benchmark::State& state)
{
  thh::packed_hashtable_t<int64_t, particle_t> packed_hashtable_particles;
  packed_hashtable_particles.reserve(static_cast<int32_t>(state.range(0)));
  for ([[maybe_unused]] auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      packed_hashtable_particles.add({i, particle_t{}});
    }
  }
}

BENCHMARK(add_particle_t_in_packed_hashtable)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void add_particle_t_inpacked_hashtable_rl(benchmark::State& state)
{
  thh::packed_hashtable_rl_t<int64_t, particle_t> packed_hashtable_particles;
  packed_hashtable_particles.reserve(static_cast<int32_t>(state.range(0)));
  for ([[maybe_unused]] auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      packed_hashtable_particles.add({i, particle_t{}});
    }
  }
}

BENCHMARK(add_particle_t_inpacked_hashtable_rl)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void add_particle_t_in_unordered_map(benchmark::State& state)
{
  std::unordered_map<int64_t, particle_t> map_particles;
  map_particles.reserve(state.range(0));
  for ([[maybe_unused]] auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      map_particles.insert({i, particle_t{}});
    }
  }
}

BENCHMARK(add_particle_t_in_unordered_map)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

// remove elements passing a predicate from the packed hashtable using value
// iteration (note: uses reverse lookup - packed_hashtable_rl_t)
static void remove_particle_t_in_packed_hashtable_by_value(
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

BENCHMARK(remove_particle_t_in_packed_hashtable_by_value)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

// remove elements passing a predicate from the packed hashtable using handle
// iteration (note: does not use reverse lookup - packed_hashtable_t)
static void remove_particle_t_in_packed_hashtable_by_handle(
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

BENCHMARK(remove_particle_t_in_packed_hashtable_by_handle)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

// remove elements passing a predicate from an unordered map using key/value
// iteration
static void remove_particle_t_in_unordered_map_by_key_value_pair(
  benchmark::State& state)
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

BENCHMARK(remove_particle_t_in_unordered_map_by_key_value_pair)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void find_value_in_packed_hashtable_by_key(benchmark::State& state)
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

BENCHMARK(find_value_in_packed_hashtable_by_key)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

static void find_value_in_unordered_map_by_key(benchmark::State& state)
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

BENCHMARK(find_value_in_unordered_map_by_key)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_map_by_key_value_pair, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_flat_hash_map_by_key_value_pair, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_unordered_flat_map_by_key_value_pair, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_handle, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_handle, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order,
  object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order,
  object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_handle_in_random_order,
  object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_value, object_t<32>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_value, object_t<64>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_value, object_t<128>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_value, object_t<256>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(iterate_object_t_in_packed_hashtable_by_value, object_t<512>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_value, object_t<1024>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_value, object_t<2048>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);
BENCHMARK_TEMPLATE(
  iterate_object_t_in_packed_hashtable_by_value, object_t<4096>)
  ->RangeMultiplier(2)
  ->Range(32, 8 << 13);

BENCHMARK_MAIN();
