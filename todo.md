# todo

- ~~test for insert~~
- implement emplace
- implement clear
- implement erase
- add const call and call_return
- test for find
- test for iterate
- test for enumerate
- add contains
- add capacity

- ebegin()
- cebegin()
- hbegin
- chbegin
- hend()

- value_iterator
- handle_iterator

- dense_iterator
- lookup_iterator

## notes

- `dense_map_t` stores an additional `28` bytes per element over `unordered_map`
  - `20` bytes per element in the `handle_vector_t`
    - `16` bytes (per internal handles)
    - `4` bytes (per element id)
  - `8` bytes per element for the `typed_handle_t` stored in the internal `unordered_map`
