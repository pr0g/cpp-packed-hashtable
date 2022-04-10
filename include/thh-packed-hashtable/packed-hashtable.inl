namespace thh
{
  template<typename Key, typename Value, typename Tag>
  template<typename P>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add_internal(P&& key_value)
  {
    if (auto lookup = keys_to_handles_.find(key_value.first);
        lookup != keys_to_handles_.end()) {
      return {lookup, false};
    }
    const auto handle = values_.add(std::forward<Value>(key_value.second));
    const auto inserted = keys_to_handles_.insert(
      {std::forward<const Key>(key_value.first), handle});
    handles_to_keys_.insert({handle, &inserted.first->first});
    return inserted;
  }

  template<typename Key, typename Value, typename Tag>
  template<typename P>
  std::pair<typename packed_hashtable_t<Key, Value, Tag>::handle_iterator, bool>
  packed_hashtable_t<Key, Value, Tag>::add_or_update_internal(P&& key_value)
  {
    if (auto lookup = keys_to_handles_.find(key_value.first);
        lookup != keys_to_handles_.end()) {
      values_.call(lookup->second, [&key_value](Value& value) {
        value = std::forward<Value>(key_value.second);
      });
      return {lookup, false};
    }
    const auto handle = values_.add(std::forward<Value>(key_value.second));
    const auto inserted = keys_to_handles_.insert(
      {std::forward<const Key>(key_value.first), handle});
    handles_to_keys_.insert({handle, &inserted.first->first});
    return inserted;
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
    return keys_to_handles_.find(key);
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::const_handle_iterator
  packed_hashtable_t<Key, Value, Tag>::find(const Key& key) const
  {
    return keys_to_handles_.find(key);
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::handle_iterator
  packed_hashtable_t<Key, Value, Tag>::remove(const Key& key)
  {
    if (auto position = keys_to_handles_.find(key);
        position != keys_to_handles_.end()) {
      [[maybe_unused]] const auto removed = values_.remove(position->second);
      assert(removed);
      handles_to_keys_.erase(position->second);
      return keys_to_handles_.erase(position);
    }
    return keys_to_handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  typename packed_hashtable_t<Key, Value, Tag>::handle_iterator
  packed_hashtable_t<Key, Value, Tag>::remove(handle_iterator position)
  {
    [[maybe_unused]] const auto removed = values_.remove(position->second);
    assert(removed);
    handles_to_keys_.erase(position->second);
    return keys_to_handles_.erase(position);
  }

  template<typename Key, typename Value, typename Tag>
  bool packed_hashtable_t<Key, Value, Tag>::remove(
    const typed_handle_t<Tag> handle)
  {
    if (const auto removed = values_.remove(handle)) {
      if (const auto key = handles_to_keys_.find(handle);
          key != handles_to_keys_.end()) {
        handles_to_keys_.erase(key);
        return keys_to_handles_.erase(*key->second) != 0;
      }
    }
    return false;
  }

  template<typename Key, typename Value, typename Tag>
  bool packed_hashtable_t<Key, Value, Tag>::has(const Key& key) const
  {
    return keys_to_handles_.find(key) != keys_to_handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  typed_handle_t<Tag> packed_hashtable_t<Key, Value, Tag>::handle_from_index(
    const int32_t index)
  {
    return values_.handle_from_index(index);
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
    keys_to_handles_.clear();
    handles_to_keys_.clear();
  }

  template<typename Key, typename Value, typename Tag>
  void packed_hashtable_t<Key, Value, Tag>::reserve(const int32_t capacity)
  {
    assert(capacity > 0);
    values_.reserve(capacity);
    keys_to_handles_.reserve(capacity);
  }

  template<typename Key, typename Value, typename Tag>
  template<typename Fn>
  void packed_hashtable_t<Key, Value, Tag>::call(const Key& key, Fn&& fn)
  {
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
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
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
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
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
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
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
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
    assert(keys_to_handles_.size() == values_.size());
    assert(values_.size() <= std::numeric_limits<int32_t>::max());
    return static_cast<int32_t>(values_.size());
  }

  template<typename Key, typename Value, typename Tag>
  bool packed_hashtable_t<Key, Value, Tag>::empty() const
  {
    assert(keys_to_handles_.empty() == values_.empty());
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
    return keys_to_handles_.begin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hbegin() const
    -> const_handle_iterator
  {
    return keys_to_handles_.begin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hcbegin() const
    -> const_handle_iterator
  {
    return keys_to_handles_.cbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hend() -> handle_iterator
  {
    return keys_to_handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hend() const
    -> const_handle_iterator
  {
    return keys_to_handles_.end();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::hcend() const
    -> const_handle_iterator
  {
    return keys_to_handles_.cend();
  }

  template<typename Key, typename Value, typename Tag>
  packed_hashtable_t<Key, Value, Tag>::handle_iterator_wrapper_t::
    handle_iterator_wrapper_t(packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::handle_iterator_wrapper_t::begin()
    -> handle_iterator
  {
    return pht_->hbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::handle_iterator_wrapper_t::end()
    -> handle_iterator
  {
    return pht_->hend();
  }

  template<typename Key, typename Value, typename Tag>
  packed_hashtable_t<Key, Value, Tag>::const_handle_iterator_wrapper_t::
    const_handle_iterator_wrapper_t(const packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_handle_iterator_wrapper_t::begin() const
    -> const_handle_iterator
  {
    return pht_->hbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_handle_iterator_wrapper_t::cbegin() const
    -> const_handle_iterator
  {
    return pht_->hcbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_handle_iterator_wrapper_t::end() const
    -> const_handle_iterator
  {
    return pht_->hend();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_handle_iterator_wrapper_t::cend() const
    -> const_handle_iterator
  {
    return pht_->hcend();
  }

  template<typename Key, typename Value, typename Tag>
  packed_hashtable_t<Key, Value, Tag>::value_iterator_wrapper_t::
    value_iterator_wrapper_t(packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::value_iterator_wrapper_t::begin()
    -> value_iterator
  {
    return pht_->vbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::value_iterator_wrapper_t::end()
    -> value_iterator
  {
    return pht_->vend();
  }

  template<typename Key, typename Value, typename Tag>
  packed_hashtable_t<Key, Value, Tag>::const_value_iterator_wrapper_t::
    const_value_iterator_wrapper_t(const packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_value_iterator_wrapper_t::begin() const
    -> const_value_iterator
  {
    return pht_->vbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_value_iterator_wrapper_t::cbegin() const
    -> const_value_iterator
  {
    return pht_->vcbegin();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_value_iterator_wrapper_t::end() const
    -> const_value_iterator
  {
    return pht_->vend();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<
    Key, Value, Tag>::const_value_iterator_wrapper_t::cend() const
    -> const_value_iterator
  {
    return pht_->vcend();
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::handle_iteration()
    -> handle_iterator_wrapper_t
  {
    return handle_iterator_wrapper_t(*this);
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::handle_iteration() const
    -> const_handle_iterator_wrapper_t
  {
    return const_handle_iterator_wrapper_t(*this);
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::value_iteration()
    -> value_iterator_wrapper_t
  {
    return value_iterator_wrapper_t(*this);
  }

  template<typename Key, typename Value, typename Tag>
  auto packed_hashtable_t<Key, Value, Tag>::value_iteration() const
    -> const_value_iterator_wrapper_t
  {
    return const_value_iterator_wrapper_t(*this);
  }
} // namespace thh
