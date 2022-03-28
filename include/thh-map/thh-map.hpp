#pragma once

#include <thh_handles/thh_handles.hpp>
#include <unordered_map>

namespace thh
{
  template<typename Key, typename Value>
  class packed_hashtable_t
  {
    handle_vector_t<Value> values_;
    std::unordered_map<Key, typed_handle_t<default_tag_t>> handles_;

  public:
    using key_value_type = std::pair<const Key, Value>;
    using value_iterator = typename decltype(values_)::iterator;
    using const_value_iterator = typename decltype(values_)::const_iterator;
    using handle_iterator = typename decltype(handles_)::iterator;
    using const_handle_iterator = typename decltype(handles_)::const_iterator;

    template<typename P>
    void add(P&& value);
    void add(key_value_type&& value);

    // add_or_update
    // remove
    // has
    // capacity
    // clear

    void reserve(int32_t capacity);
    [[nodiscard]] int32_t size() const;
    [[nodiscard]] bool empty() const;
    template<typename Fn>
    void call(const Key& key, Fn&& fn);
    // other call overloads...
    auto vbegin() -> value_iterator;
    auto vbegin() const -> const_value_iterator;
    auto vcbegin() const -> const_value_iterator;
    auto vend() -> value_iterator;
    auto vend() const -> const_value_iterator;
    auto vcend() const -> const_value_iterator;

    auto hbegin() -> handle_iterator;
    auto hbegin() const -> const_handle_iterator;
    auto hcbegin() const -> const_handle_iterator;
    auto hend() -> handle_iterator;
    auto hend() const -> const_handle_iterator;
    auto hcend() const -> const_handle_iterator;
  };
} // namespace thh

#include "thh-map.inl"
