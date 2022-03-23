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
    using value_type = std::pair<const Key, Value>;
    using iterator = typename decltype(values_)::iterator;
    using const_iterator = typename decltype(values_)::const_iterator;

    template<typename P>
    void add(P&& value);
    void add(value_type&& value);

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
    auto begin() -> iterator;
    auto begin() const -> const_iterator;
    auto cbegin() const -> const_iterator;
    auto end() -> iterator;
    auto end() const -> const_iterator;
    auto cend() const -> const_iterator;
  };
} // namespace thh

#include "thh-map.inl"
