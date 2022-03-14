# todo

- test for insert

## notes

- `dense_map_t` stores an additional `28` bytes per element over `unordered_map`
  - `20` bytes per element in the `handle_vector_t`
    - `16` bytes (per internal handles)
    - `4` bytes (per element id)
  - `8` bytes per element for the `typed_handle_t` stored in the internal `unordered_map`
