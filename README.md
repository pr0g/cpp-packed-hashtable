# Experimental hybrid lookup container in C++

## Overview

This library is a random experiment to see what it would take to create a container with the iteration performance of a densely packed `std::array`/`std::vector` with the ability to look-up items in O(1) time similar to that of `std::unordered_map`.

`packed_hashtable_t` is the result. A lazy person's approach to solving this problem. The intention is to prove the theory that this idea has some merit while not suggesting this is the optimum implementation.

## Caveats (of which there are many...)

- It's totally possible I missed something blindingly obvious and there's a way better way to do this. If there is please let me know. This is really just a random experiment. It absolutely isn't something I'd recommend using outside of toy examples.
- Right now the implementation makes fairly liberal use of templates. Some people might (quite fairly) not like this. It's totally possible to rewrite this in a less general way and make different trade-offs. Right now it's possible the current implementation could lead to inflated compile times (a quick [build-bench](https://www.build-bench.com/) investigation shows it takes roughly **1.2x** longer to compile than using `std::unordered_map` on its own - YMMV).
- The interface is intentionally different to `std::unordered_map` (though there are some similarities). As the container does have different characteristics to `std::unordered_map`, certain operations have been renamed (e.g. `add_or_update` instead of `insert_or_assign`) to make it clear this is not just a drop-in replacement. The API is designed to align more closely with [handle-vector](https://github.com/pr0g/cpp-handle-container) which this container builds on top of.
- The interface currently exposes two kinds of iterators (one for key/handle pairs, and one for values). It might be possible to only expose value iteration which may simplify the interface while losing some flexibility. If you decide to try something similar please do what's right for you, this is just one possible implementation.
- There is a fixed overhead of *28 bytes* right now for every element stored in `packed_hashtable_t`. This is fairly large, and is mostly because of the types used in `handle_vector_t`.
  - This breaks down as follows.
    - `20` bytes per element in the `handle_vector_t`.
      - `16` bytes (per internal handles).
      - `4` bytes (per element id).
    - `8` bytes per element for the `typed_handle_t` stored in the internal `unordered_map`.
  - This number could be brought down significantly by being smarter about the number of bits to use for the handle generation (e.g. Store the generation in the upper 8 or 16 bits of the handle (using an `int32` is overkill and lazy right now...). It could also be possible to make the size of the types a compile time option that people select based on the use-case, though this might lead to more template boilerplate and longer compile times... something to definitely experiment with and adjust based on the requirements.
- There is a smattering of unit tests to verify the core functionality but they are not currently exhaustive (more are planned...).

## Implementation

The core idea is to wrap two data structures behind one interface in an attempt to get the best of both worlds.

`packed_hashtable_t` internally has a `handle_vector_t` (please see [this repo](https://github.com/pr0g/cpp-handle-container) for more details) and a `std::unordered_map` (this could just as easily be a more efficient hash table implementation, the main reason for using it is to drag in less dependences right now).

When you insert/add a key/value pair, we allocate a value from the `handle_vector_t` and move the value argument into place (`handle_vector_t` is just a `std::vector<T>` under the hood) and then return the handle for that new value. We then store the handle with the key argument in the `std::unordered_map`. To look-up a value, we go **key -> handle -> value**. This means we've added an extra level of indirection for insertions, removals and look-ups, so these will be slightly slower than using a `std::unordered_map` directly, however the cool part is when we iterate over the actual values, they're all packed tightly together in a contiguous buffer and we get excellent cache locality.

The value iterators are exposed through `vbegin()` and `vend()` functions. There's also a proxy object called `value_iterator_wrapper_t` which takes a pointer to the `packed_hashtable_t` and provides `begin()`/`end()` pass-through functions so the container can be used with range based for loops.

e.g.

```c++
for (const auto& value : packed_hashtable.value_iteration()) {
  // display value
}
```

There are equivalent iterators for the key/handle pairs which are exposed as `hbegin()` and `hend()` (they also have a helper proxy object that is returned by `handle_iteration()`).

And that's pretty much all there is to it.

The only other thing worth mentioning is the `call` API. This takes a key or handle and invokes a callable object with the parameter being whichever value gets resolved. There's also `call_return` which can be used to return a value. It returns a `std::optional` as it's possible the key/handle won't get resolved (it can fail). This matches the API in `handle_vector_t` and helps avoid gnarly lifetime issues that can happen if you hold onto a pointer for too long (see [cpp-handle-container - gotchas](https://github.com/pr0g/cpp-handle-container#gotchas) for more details).

## Results

With all the caveats/drawbacks out of the way, there is actually some good news to share. By moving values to be stored in a contiguous block of memory, performance increases *significantly* when iterating over the values.

There are quite a few benchmarks for a varying numbers of elements with different sizes (see **measurements/container-measurements.xlsx** in this repo). As the number and size of elements increases, the performance of `packed_hashtable_t` increases significantly relative to `std::unordered_map`. Depending on the number of elements to visit and the individual size of each element the speed increase can be anywhere from 2.6x (64 elements, 256 bytes each) to **24x** (65,536 elements, 2048 bytes each).

The benchmarks at the time of writing are pretty simple (visit every element and read a value from the first byte of each). Real world conditions are likely going to be quite more varied and the gains may not be as significant (one would wager likely still noticeable though).

There aren't currently benchmarks comparing additions/removals (these will be added soon...). The priority was to measure iteration performance first and foremost which is why the majority of benchmarks focussed are around that criteria.
