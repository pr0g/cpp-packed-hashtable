#pragma once

#include <thh_handles/thh_handles.hpp>
#include <unordered_map>

namespace thh
{
  template<typename Key, typename Value>
  class dense_map_t
  {
    handle_vector_t<Value> values_;
    std::unordered_map<Key, typed_handle_t<default_tag_t>> handles_;

  public:
    using value_type = std::pair<const Key, Value>;
    using iterator = typename decltype(values_)::iterator;
    using const_iterator = typename decltype(values_)::const_iterator;

    void insert(const value_type& value);
    void insert(value_type&& value);
    template<typename... Args>
    void try_emplace(const Key& key, Args&&... args);
    void reserve(int32_t capacity);
    [[nodiscard]] int32_t size() const;
    [[nodiscard]] bool empty() const;
    template<typename Fn>
    void call(const Key& key, Fn&& fn);
    auto begin() -> iterator;
    auto begin() const -> const_iterator;
    auto cbegin() const -> const_iterator;
    auto end() -> iterator;
    auto end() const -> const_iterator;
    auto cend() const -> const_iterator;
  };
} // namespace thh

#include "thh-map.inl"
