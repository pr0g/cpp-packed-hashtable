namespace thh
{
  template<typename Key, typename Value, typename Tag>
  template<typename P>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add_internal(P&& key_value)
  {
    if (auto lookup = handles_.find(key_value.first);
        lookup != handles_.end()) {
      return {lookup, false};
    }
    const auto handle = values_.add(std::forward<Value>(key_value.second));
    return handles_.insert({std::forward<const Key>(key_value.first), handle});
  }

  template<typename Key, typename Value, typename Tag>
  template<typename P>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add_or_update_internal(P&& key_value)
  {
    if (auto lookup = handles_.find(key_value.first);
        lookup != handles_.end()) {
      values_.call(lookup->second, [&key_value](Value& value) {
        value = std::forward<Value>(key_value.second);
      });
      return {lookup, false};
    }
    const auto handle = values_.add(std::forward<Value>(key_value.second));
    return handles_.insert({std::forward<const Key>(key_value.first), handle});
  }

  template<typename Key, typename Value, typename Tag>
  template<typename P>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add(P&& key_value)
  {
    return add_internal(std::forward<P>(key_value));
  }

  template<typename Key, typename Value, typename Tag>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add(key_value_type&& key_value)
  {
    return add_internal(std::move(key_value));
  }

  template<typename Key, typename Value, typename Tag>
  template<typename P>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add_or_update(P&& key_value)
  {
    return add_or_update_internal(std::forward<P>(key_value));
  }

  template<typename Key, typename Value, typename Tag>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add_or_update(key_value_type&& key_value)
  {
    return add_or_update_internal(std::move(key_value));
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::handle_iterator
  packed_hashtable_t<Key, Value, Tag>::find(const Key& key)
  {
    return handles_.find(key);
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::const_handle_iterator
  packed_hashtable_t<Key, Value, Tag>::find(const Key& key) const
  {
    return handles_.find(key);
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::handle_iterator
  packed_hashtable_t<Key, Value, Tag>::remove(const Key& key)
  {
    if (auto lookup = handles_.find(key); lookup != handles_.end()) {
      [[maybe_unused]] const auto removed = values_.remove(lookup->second);
      assert(removed);
      return handles_.erase(lookup);
    }
    return handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::handle_iterator
  packed_hashtable_t<Key, Value, Tag>::remove(handle_iterator position)
  {
    [[maybe_unused]] const auto removed = values_.remove(position->second);
    assert(removed);
    return handles_.erase(position);
  }

  template<typename Key, typename Value, typename Tag>
  bool packed_hashtable_t<Key, Value, Tag>::has(const Key& key) const
  {
    return handles_.find(key) != handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  int32_t packed_hashtable_t<Key, Value, Tag>::capacity() const
  {
    return values_.capacity();
  }

  template<typename Key, typename Value, typename Tag>
  void packed_hashtable_t<Key, Value, Tag>::clear()
  {
    values_.clear();
    handles_.clear();
  }

  template<typename Key, typename Value, typename Tag>
  void packed_hashtable_t<Key, Value, Tag>::reserve(const int32_t capacity)
  {
    assert(capacity > 0);
    values_.reserve(capacity);
    handles_.reserve(capacity);
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  void packed_hashtable_t<Key, Value, Tag>::call(const Key& key, Fn&& fn)
  {
    if (auto lookup = handles_.find(key); lookup != handles_.end()) {
      values_.call(lookup->second, std::forward<Fn>(fn));
    }
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  void packed_hashtable_t<Key, Value, Tag>::call(
    const typed_handle_t<Tag> handle, Fn&& fn)
  {
    values_.call(handle, std::forward<Fn>(fn));
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  void packed_hashtable_t<Key, Value, Tag>::call(const Key& key, Fn&& fn) const
  {
    if (auto lookup = handles_.find(key); lookup != handles_.end()) {
      values_.call(lookup->second, std::forward<Fn>(fn));
    }
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  void packed_hashtable_t<Key, Value, Tag>::call(
    const typed_handle_t<Tag> handle, Fn&& fn) const
  {
    values_.call(handle, std::forward<Fn>(fn));
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  decltype(auto) packed_hashtable_t<Key, Value, Tag>::call_return(
    const Key& key, Fn&& fn)
  {
    if (auto lookup = handles_.find(key); lookup != handles_.end()) {
      return values_.call_return(lookup->second, std::forward<Fn>(fn));
    }
    return std::optional<decltype(fn(*(static_cast<Value*>(nullptr))))>{};
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  decltype(auto) packed_hashtable_t<Key, Value, Tag>::call_return(
    const typed_handle_t<Tag> handle, Fn&& fn)
  {
    return values_.call_return(handle, std::forward<Fn>(fn));
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  decltype(auto) packed_hashtable_t<Key, Value, Tag>::call_return(
    const Key& key, Fn&& fn) const
  {
    if (auto lookup = handles_.find(key); lookup != handles_.end()) {
      return values_.call_return(lookup->second, std::forward<Fn>(fn));
    }
    return std::optional<decltype(fn(*(static_cast<Value*>(nullptr))))>{};
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  decltype(auto) packed_hashtable_t<Key, Value, Tag>::call_return(
    const typed_handle_t<Tag> handle, Fn&& fn) const
  {
    return values_.call_return(handle, std::forward<Fn>(fn));
  }

  template<typename Key, typename Value, typename Tag>
  int32_t packed_hashtable_t<Key, Value, Tag>::size() const
  {
    assert(handles_.size() == values_.size());
    assert(values_.size() <= std::numeric_limits<int32_t>::max());
    return static_cast<int32_t>(values_.size());
  }

  template<typename Key, typename Value, typename Tag>
  bool packed_hashtable_t<Key, Value, Tag>::empty() const
  {
    assert(handles_.empty() == values_.empty());
    return values_.empty();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::vbegin() -> value_iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::vbegin() const
    -> const_value_iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::vcbegin() const
    -> const_value_iterator
  {
    return values_.cbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::vend() -> value_iterator
  {
    return values_.end();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::vend() const -> const_value_iterator
  {
    return values_.end();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::vcend() const
    -> const_value_iterator
  {
    return values_.cend();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hbegin() -> handle_iterator
  {
    return handles_.begin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hbegin() const
    -> const_handle_iterator
  {
    return handles_.begin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hcbegin() const
    -> const_handle_iterator
  {
    return handles_.cbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hend() -> handle_iterator
  {
    return handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hend() const
    -> const_handle_iterator
  {
    return handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hcend() const
    -> const_handle_iterator
  {
    return handles_.cend();
  }
} // namespace thh
