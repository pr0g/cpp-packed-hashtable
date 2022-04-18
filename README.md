# Experimental hybrid look-up container in C++

## Overview

This library is a random experiment to see what it would take to create a container with the iteration performance of a densely packed `std::array`/`std::vector` with the ability to look-up items in `O(1)` time similar to that of `std::unordered_map`.

`packed_hashtable_t` is the result. A lazy person's approach to solving this problem. Whether this is something that could actually be useful in practice is another question 😅 The intention is to prove the theory while not suggesting this is the optimum implementation.

## Caveats (of which there are many...)

- It's totally possible I missed something blindingly obvious and there's a way better way to do this (_**news flash**: there mostly is, in practice you should probably just use `absl::flat_hash_map` or `robin_hood::unordered_flat_map` as their iteration performance is pretty great, especially as the size of the element increases_). If there is, please do let me know. This is really just a series of musings in the form of code that may be completely worthless. It absolutely isn't something I'd recommend using outside of toy examples.
- Right now the implementation makes fairly liberal use of templates. Some people might (quite fairly) not like this. It's totally possible to rewrite this in a less general way and make different trade-offs. Right now it's possible the current implementation could lead to inflated compile times (a quick [build-bench](https://www.build-bench.com/) investigation shows it takes roughly **1.2x** longer to compile than using `std::unordered_map` on its own - your mileage may vary).
- The interface is intentionally different to `std::unordered_map` (though there are some similarities). As the container has different characteristics to `std::unordered_map`, certain operations have been renamed (e.g. `add_or_update` instead of `insert_or_assign`) to make it clear this is not just a drop-in replacement. The API is designed to align more closely with [handle-vector](https://github.com/pr0g/cpp-handle-container) which this container builds on top of.
- The interface currently exposes two kinds of iterators (one for key/handle pairs, and one for values). It might be possible to only expose value iteration which may simplify the interface while losing some flexibility. If you decide to try something similar please do what's right for you, this is just one possible approach.
- There is a fixed overhead of **28 bytes** (_this isn't strictly true, see_ **[Additional Caveats](#additional-caveats)** _for more details_) right now for every element stored in `packed_hashtable_t`. This is fairly large, and is mostly because of the types used in `handle_vector_t`.
  - This breaks down as follows.
    - `20` bytes per element in the `handle_vector_t`.
      - `16` bytes (per internal handle).
      - `4` bytes (per element id).
    - `8` bytes per element for the `typed_handle_t` stored in the internal `std::unordered_map`.
  - This number could be brought down significantly by being smarter about the number of bits to use for the handle generation (e.g. Store the generation in the upper `8` or `16` bits of the handle (using an `int32` is overkill and lazy right now...). It could also be possible to make the size of the types a compile time option that people select based on the use-case, though this might lead to more template boilerplate and longer compile times... something to definitely experiment with and adjust based on the requirements.
- There is a smattering of unit tests (see `test.cpp`) to verify the core functionality but they are not currently exhaustive.
- There are a series benchmarks (see `bench.cpp`) measuring iteration, addition and removal performance. These again are not exhaustive but give an indication of the differences in performance.
- There's a crude memory tracking executable (see `memory.cpp`) to compare and contrast the amount of memory the different versions of the container and other standard library containers use.

## Implementation

The core idea is to wrap two data structures behind one interface in an attempt to get the best of both worlds.

`packed_hashtable_t` internally has a `handle_vector_t` (please see [this repo](https://github.com/pr0g/cpp-handle-container) for more details, it's essentially a version of [sparse set](https://programmingpraxis.com/2012/03/09/sparse-sets/)) and a `std::unordered_map` (this could just as easily be a more efficient hash table implementation, the main reason for using it is to drag in less dependencies).

When you insert/add a key/value pair, we allocate a value from the `handle_vector_t` and move the value argument into place (`handle_vector_t` is just a `std::vector<T>` under the hood) and then return the handle for that new value. We then store the handle with the key argument in the `std::unordered_map`. To look-up a value, we go **key -> handle -> value**. This means we've added an extra level of indirection for insertions, removals and look-ups, so these will be slightly slower than using a `std::unordered_map` directly, however the cool part is when we iterate over the actual values, they are all packed tightly together in a contiguous buffer and we get excellent cache locality.

The value iterators are exposed through `vbegin()` and `vend()` functions. There's a proxy object called `value_iterator_wrapper_t` which takes a pointer to the `packed_hashtable_t` and provides `begin()`/`end()` pass-through functions so the container can be used with range based for loops.

e.g.

```c++
for (const auto& value : packed_hashtable.value_iteration()) {
  // display value
}
```

There are equivalent iterators for the key/handle pairs which are exposed as `hbegin()` and `hend()` (they also have a helper proxy object that is returned by `handle_iteration()`). It's possible to iterate over the values via the key/handle pairs but this is much slower than iterating the values directly.

And that's pretty much all there is to it.

The only other thing worth mentioning is the `call` API. This takes a key or handle and invokes a callable object with the parameter being whichever value gets resolved. There's also `call_return` which can be used to return a value. It returns a `std::optional` as it's possible the key/handle won't get resolved (it might not be in the container). This matches the API in `handle_vector_t` and helps avoid gnarly lifetime issues that can happen if you hold onto a pointer for too long (see [cpp-handle-container - gotchas](https://github.com/pr0g/cpp-handle-container#gotchas) for more details).

## Additional Caveats

- There are actually two versions of the container, one called `packed_hashtable_t`, and another called `packed_hashtable_rl_t`. The `rl` signifies '_reverse look-up_' and is needed to map from values to handles to keys (**value -> handle -> key**). Internally this is achieved by simply adding an additional `std::unordered_map` that takes a handle as its key, and a const pointer to the original key as the value. The reason for this is to allow fast iteration of values that can then be fully removed from the container (it's possible to return the handle for a specific value from the container, and then use that to look-up the key and then call `remove`). The **big** downside to this is it does use a bunch more memory per element.
  - As mentioned above, `packed_hashtable_t` takes an additional 28 bytes per element due to the `handle_vector_t`, but `packed_hashtable_rl_t` takes an additional **40 bytes** per element on top of that due to the `typed_handle_t` (8 bytes) and `Key*` (8 bytes on x64) plus the internals of a `std::unordered_map` (buckets, lists etc...).
  - **Note**: This reverse mapping is only actually required if removal during iteration is needed. If elements are removed by an outside system, then it's fine to just use `packed_hashtable_t`. It is also perfectly fine to use `packed_hashtable_t` for removal (see the `remove_when` overload), it'll just be much slower.
- An attempt has been made to follow the _'don't pay for what you don't use'_ mantra, which is why there are two versions of the container. To avoid code duplication and any runtime overhead, the _Curiously recurring template pattern (CRTP)_ has been used to support the reverse look-up (this is just an implementation detail and could totally be removed).
- It may be possible to improve the internal implementation of this by switching to use [Boost.Bimap](https://www.boost.org/doc/libs/1_75_0/libs/bimap/doc/html/index.html), a bi-directional map. I haven't investigated this though as I did not want to have to bring in the Boost dependency, or roll my own implementation.

## Results

With all the caveats/drawbacks out of the way, there is actually some good news to share. By moving values to be stored in a contiguous block of memory, performance increases _significantly_ when iterating over the values.

There are quite a few benchmarks for a varying number of elements with different sizes (see `bench.cpp` and `measurements/container-measurements.xlsx` in this repo). As the number and size of elements increases, the performance of `packed_hashtable_t` increases significantly relative to `std::unordered_map`. Depending on the number of elements to visit and the individual size of each element the speed increase can be anywhere from 2.6x (64 elements, 256 bytes each) to **24x** (65,536 elements, 2048 bytes each).

The benchmarks at the time of writing are pretty simple. One set visit every element and read a value from the first byte of each and another attempts to mimic more real-world conditions, reading and writing to multiple fields of each element. There are also benchmarks for additions and removals.

## Usage

To use the library it's possible to drop the `thh-packed-hashtable` folder inside `include` into your project (and then just `#include "thh-packed-hashtable/packed-hashtable.hpp"`) or use CMake's `FetchContent` command.

e.g.

```cmake
# CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
  thh-packed-hashtable
  GIT_REPOSITORY https://github.com/pr0g/cpp-packed-hashtable.git
  GIT_TAG        <latest-commit>)
FetchContent_MakeAvailable(thh-packed-hashtable)
...
target_link_libraries(<your-project> <PRIVATE/PUBLIC> thh-packed-hashtable)
```

To build the examples (`bench.cpp`, `test.cpp` and `memory.cpp`) pass `-DTHH_PACKED_HASHTABLE_ENABLE_MEMORY=ON`, `-DTHH_PACKED_HASHTABLE_ENABLE_TEST=ON` and/or `-DTHH_PACKED_HASHTABLE_ENABLE_BENCH=ON` respectively to the CMake configure command when building from the root of this repo.

e.g.

```bash
cd <thh-packed-hashtable root>
cmake -B build -DTHH_PACKED_HASHTABLE_ENABLE_MEMORY=ON -DTHH_PACKED_HASHTABLE_ENABLE_TEST=ON -DTHH_PACKED_HASHTABLE_ENABLE_BENCH=ON
cmake --build build
```

Also remember to pass `-DCMAKE_BUILD_TYPE=Release` when running the benchmarks if using a single-config generator (with multi-config generators like Visual Studio you can use `cmake --build build --config Release` at build time). The repo contains a `configure.sh` script with an example of being able to configure the benchmarks, memory tracking and tests.
