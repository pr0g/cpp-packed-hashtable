#pragma once

#include <thh_handles/thh_handles.hpp>
#include <unordered_map>

namespace thh
{
  template<typename Key, typename Value>
  class dense_map_t
  {
    using value_type = std::pair<const Key, Value>;

    handle_vector_t<Value> values_;
    std::unordered_map<Key, typed_handle_t<default_tag_t>> handles_;

  public:
    void insert(const value_type& value)
    {
      const auto handle = values_.add(value.second);
      handles_.emplace(value.first, handle);
    }

    void insert(value_type&& value)
    {
      const auto handle = values_.add(std::move(value).second);
      handles_.emplace(std::move(value).first, handle);
    }

    void reserve(int32_t capacity)
    {
      values_.reserve(capacity);
      handles_.reserve(capacity);
    }

    template<typename Fn>
    void call(const Key& key, Fn&& fn)
    {
      if (auto lookup = handles_.find(key); lookup != handles_.end()) {
        values_.call(lookup->second, std::forward<Fn&&>(fn));
      }
    }

    [[nodiscard]] int32_t size() const
    {
      assert(handles_.size() == values_.size());
      assert(values_.size() <= std::numeric_limits<int32_t>::max());
      return static_cast<int32_t>(values_.size());
    }

    auto begin() -> typename decltype(values_)::iterator
    {
      return values_.begin();
    }

    auto end() -> typename decltype(values_)::iterator { return values_.end(); }
  };
} // namespace thh
