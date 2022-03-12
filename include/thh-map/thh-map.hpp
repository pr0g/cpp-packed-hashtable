#pragma once

#include <thh_handles/thh_handles.hpp>
#include <unordered_map>

namespace thh
{
  template<typename Key, typename Value>
  class lookup_table_t
  {
    using value_type = std::pair<const Key, Value>;

    handle_vector_t<Value> values_;
    std::unordered_map<Key, typed_handle_t<default_tag_t>> handles_;

  public:
    void insert(value_type&& value)
    {
      const auto handle = values_.add(value.second);
      handles_.insert({value.first, handle});
    }

    template<typename Fn>
    void call(const Key& key, Fn&& fn)
    {
      if (auto lookup = handles_.find(key); lookup != handles_.end()) {
        values_.call(lookup->second, std::forward<Fn&&>(fn));
      }
    }

    auto begin() -> typename decltype(values_)::iterator
    {
      return values_.begin();
    }

    auto end() -> typename decltype(values_)::iterator
    {
      return values_.end();
    }
  };
} // namespace thh
