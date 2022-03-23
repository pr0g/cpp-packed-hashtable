namespace thh
{
  template<typename Key, typename Value>
  template<typename P>
  void packed_hashtable_t<Key, Value>::add(P&& value)
  {
    if (auto lookup = handles_.find(value.first); lookup != handles_.end()) {
      [[maybe_unused]] const bool removed = values_.remove(lookup->second);
      assert(removed);
    }
    const auto handle = values_.add(std::forward<Value>(value.second));
    handles_.insert({std::forward<const Key>(value.first), handle});
  }

  template<typename Key, typename Value>
  void packed_hashtable_t<Key, Value>::add(value_type&& value)
  {
    if (auto lookup = handles_.find(value.first); lookup != handles_.end()) {
      [[maybe_unused]] const bool removed = values_.remove(lookup->second);
      assert(removed);
    }
    const auto handle = values_.add(std::move(value).second);
    handles_.insert({std::move(value).first, handle});
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
  auto packed_hashtable_t<Key, Value>::begin() -> iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::begin() const -> const_iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::cbegin() const -> const_iterator
  {
    return values_.begin();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::end() -> iterator
  {
    return values_.end();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::end() const -> const_iterator
  {
    return values_.end();
  }

  template<typename Key, typename Value>
  auto packed_hashtable_t<Key, Value>::cend() const -> const_iterator
  {
    return values_.end();
  }
} // namespace thh
