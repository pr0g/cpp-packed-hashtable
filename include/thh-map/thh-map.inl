namespace thh
{
  template<typename Key, typename Value>
  template<typename P>
  void packed_hashtable_t<Key, Value>::add(P&& key_value)
  {
    if (auto lookup = handles_.find(key_value.first);
        lookup != handles_.end()) {
      [[maybe_unused]] const bool removed = values_.remove(lookup->second);
      assert(removed);
    }
    const auto handle = values_.add(std::forward<Value>(key_value.second));
    handles_.insert({std::forward<const Key>(key_value.first), handle});
  }

  template<typename Key, typename Value>
  void packed_hashtable_t<Key, Value>::add(key_value_type&& key_value)
  {
    if (auto lookup = handles_.find(key_value.first);
        lookup != handles_.end()) {
      [[maybe_unused]] const bool removed = values_.remove(lookup->second);
      assert(removed);
    }
    const auto handle = values_.add(std::move(key_value).second);
    handles_.insert({std::move(key_value).first, handle});
  }

  template<typename Key, typename Value>
  void packed_hashtable_t<Key, Value>::reserve(const int32_t capacity)
  {
    assert(capacity > 0);
    values_.reserve(capacity);
    handles_.reserve(capacity);
  }

  template<typename Key, typename Value>
  template<typename Fn>
  void packed_hashtable_t<Key, Value>::call(const Key& key, Fn&& fn)
  {
    if (auto lookup = handles_.find(key); lookup != handles_.end()) {
      values_.call(lookup->second, std::forward<Fn&&>(fn));
    }
  }

  template<typename Key, typename Value>
  int32_t packed_hashtable_t<Key, Value>::size() const
  {
    assert(handles_.size() == values_.size());
    assert(values_.size() <= std::numeric_limits<int32_t>::max());
    return static_cast<int32_t>(values_.size());
  }

  template<typename Key, typename Value>
  bool packed_hashtable_t<Key, Value>::empty() const
  {
    assert(handles_.empty() == values_.empty());
    return values_.empty();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::vbegin() -> value_iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::vbegin() const -> const_value_iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::vcbegin() const -> const_value_iterator
  {
    return values_.cbegin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::vend() -> value_iterator
  {
    return values_.end();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::vend() const -> const_value_iterator
  {
    return values_.end();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::vcend() const -> const_value_iterator
  {
    return values_.cend();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::hbegin() -> handle_iterator
  {
    return handles_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::hbegin() const -> const_handle_iterator
  {
    return handles_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::hcbegin() const -> const_handle_iterator
  {
    return handles_.cbegin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::hend() -> handle_iterator
  {
    return handles_.end();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::hend() const -> const_handle_iterator
  {
    return handles_.end();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::hcend() const -> const_handle_iterator
  {
    return handles_.cend();
  }
} // namespace thh
