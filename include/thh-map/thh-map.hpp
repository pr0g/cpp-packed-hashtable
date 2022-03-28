#pragma once

#include <thh_handles/thh_handles.hpp>
#include <unordered_map>

namespace thh
{
  using packed_hashtable_handle_t = typed_handle_t<struct packed_hash_tag_t>;

  template<typename Key, typename Value>
  class packed_hashtable_t
  {
    handle_vector_t<Value, struct packed_hash_tag_t> values_;
    std::unordered_map<Key, packed_hashtable_handle_t> handles_;

  public:
    using key_value_type = std::pair<const Key, Value>;
    using value_iterator = typename decltype(values_)::iterator;
    using const_value_iterator = typename decltype(values_)::const_iterator;
    using handle_iterator = typename decltype(handles_)::iterator;
    using const_handle_iterator = typename decltype(handles_)::const_iterator;

    template<typename P>
    std::pair<handle_iterator, bool> add(P&& key_value);
    std::pair<handle_iterator, bool> add(key_value_type&& key_value);
    template<typename P>
    std::pair<handle_iterator, bool> add_or_update(P&& key_value);
    std::pair<handle_iterator, bool> add_or_update(key_value_type&& key_value);

    // remove
    // has
    // capacity
    // clear

    void reserve(int32_t capacity);
    [[nodiscard]] int32_t size() const;
    [[nodiscard]] bool empty() const;
    template<typename Fn>
    void call(const Key& key, Fn&& fn);
    template<typename Fn>
    void call(packed_hashtable_handle_t handle, Fn&& fn);
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

  private:
    template<typename P>
    std::pair<handle_iterator, bool> add_internal(P&& key_value);
    template<typename P>
    std::pair<handle_iterator, bool> add_or_update_internal(P&& key_value);
  };
} // namespace thh

#include "thh-map.inl"
