namespace thh
{
  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename P>
  std::pair<
    typename base_packed_hashtable_t<
      Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator,
    bool>
  base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::add_internal(P&& key_value)
  {
    if (auto lookup = keys_to_handles_.find(key_value.first);
        lookup != keys_to_handles_.end()) {
      return {lookup, false};
    }
    const auto handle = values_.add(std::forward<Value>(key_value.second));
    const auto inserted = keys_to_handles_.insert(
      {std::forward<const Key>(key_value.first), handle});
    static_cast<RemovalPolicy&>(*this).add_mapping(
      handle, &inserted.first->first);
    return inserted;
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename P>
  std::pair<
    typename base_packed_hashtable_t<
      Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator,
    bool>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    add_or_update_internal(P&& key_value)
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
    static_cast<RemovalPolicy&>(*this).add_mapping(
      handle, &inserted.first->first);
    return inserted;
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename P>
  std::pair<
    typename base_packed_hashtable_t<
      Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator,
    bool>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::add(
    P&& key_value)
  {
    return add_internal(std::forward<P>(key_value));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  std::pair<
    typename base_packed_hashtable_t<
      Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator,
    bool>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::add(
    key_value_type&& key_value)
  {
    return add_internal(std::move(key_value));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename P>
  std::pair<
    typename base_packed_hashtable_t<
      Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator,
    bool>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    add_or_update(P&& key_value)
  {
    return add_or_update_internal(std::forward<P>(key_value));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  std::pair<
    typename base_packed_hashtable_t<
      Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator,
    bool>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    add_or_update(key_value_type&& key_value)
  {
    return add_or_update_internal(std::move(key_value));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  typename base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::find(
    const Key& key)
  {
    return keys_to_handles_.find(key);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  typename base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::const_handle_iterator
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::find(
    const Key& key) const
  {
    return keys_to_handles_.find(key);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  typename base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator
  base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::remove(const Key& key)
  {
    if (auto position = keys_to_handles_.find(key);
        position != keys_to_handles_.end()) {
      [[maybe_unused]] const auto removed = values_.remove(position->second);
      assert(removed);
      static_cast<RemovalPolicy&>(*this).remove_mapping(position->second);
      return keys_to_handles_.erase(position);
    }
    return keys_to_handles_.end();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  typename base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iterator
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    remove(handle_iterator position)
  {
    [[maybe_unused]] const auto removed = values_.remove(position->second);
    assert(removed);
    static_cast<RemovalPolicy&>(*this).remove_mapping(position->second);
    return keys_to_handles_.erase(position);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  bool base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::has(const Key& key) const
  {
    return keys_to_handles_.find(key) != keys_to_handles_.end();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  typed_handle_t<Tag> base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag,
    RemovalPolicy>::handle_from_index(const int32_t index) const
  {
    return values_.handle_from_index(index);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  [[nodiscard]] std::optional<int32_t> base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag,
    RemovalPolicy>::index_from_handle(typed_handle_t<Tag> handle) const
  {
    return values_.index_from_handle(handle);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  int32_t base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::capacity() const
  {
    return values_.capacity();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  void base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::clear()
  {
    values_.clear();
    keys_to_handles_.clear();
    static_cast<RemovalPolicy&>(*this).clear_mappings();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  void base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    reserve(const int32_t capacity)
  {
    assert(capacity > 0);
    values_.reserve(capacity);
    keys_to_handles_.reserve(capacity);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  void base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    call(const Key& key, Fn&& fn)
  {
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
      values_.call(lookup->second, std::forward<Fn>(fn));
    }
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  void base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    call(const typed_handle_t<Tag> handle, Fn&& fn)
  {
    values_.call(handle, std::forward<Fn>(fn));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  void base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    call(const Key& key, Fn&& fn) const
  {
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
      values_.call(lookup->second, std::forward<Fn>(fn));
    }
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  void base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    call(const typed_handle_t<Tag> handle, Fn&& fn) const
  {
    values_.call(handle, std::forward<Fn>(fn));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  decltype(auto) base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag,
    RemovalPolicy>::call_return(const Key& key, Fn&& fn)
  {
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
      return values_.call_return(lookup->second, std::forward<Fn>(fn));
    }
    return std::optional<decltype(fn(*(static_cast<Value*>(nullptr))))>{};
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  decltype(auto) base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag,
    RemovalPolicy>::call_return(const typed_handle_t<Tag> handle, Fn&& fn)
  {
    return values_.call_return(handle, std::forward<Fn>(fn));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  decltype(auto) base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag,
    RemovalPolicy>::call_return(const Key& key, Fn&& fn) const
  {
    if (auto lookup = keys_to_handles_.find(key);
        lookup != keys_to_handles_.end()) {
      return values_.call_return(lookup->second, std::forward<Fn>(fn));
    }
    return std::optional<decltype(fn(*(static_cast<Value*>(nullptr))))>{};
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Fn>
  decltype(auto) base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag,
    RemovalPolicy>::call_return(const typed_handle_t<Tag> handle, Fn&& fn) const
  {
    return values_.call_return(handle, std::forward<Fn>(fn));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  int32_t base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::size() const
  {
    assert(keys_to_handles_.size() == static_cast<size_t>(values_.size()));
    assert(values_.size() <= std::numeric_limits<int32_t>::max());
    return static_cast<int32_t>(values_.size());
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  bool base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::empty() const
  {
    assert(keys_to_handles_.empty() == values_.empty());
    return values_.empty();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::vbegin() -> value_iterator
  {
    return values_.begin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::vbegin() const
    -> const_value_iterator
  {
    return values_.begin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::vcbegin() const
    -> const_value_iterator
  {
    return values_.cbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::vend() -> value_iterator
  {
    return values_.end();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::vend() const
    -> const_value_iterator
  {
    return values_.end();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::vcend() const
    -> const_value_iterator
  {
    return values_.cend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::hbegin() -> handle_iterator
  {
    return keys_to_handles_.begin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::hbegin() const
    -> const_handle_iterator
  {
    return keys_to_handles_.begin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::hcbegin() const
    -> const_handle_iterator
  {
    return keys_to_handles_.cbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::hend() -> handle_iterator
  {
    return keys_to_handles_.end();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::hend() const
    -> const_handle_iterator
  {
    return keys_to_handles_.end();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::hcend() const
    -> const_handle_iterator
  {
    return keys_to_handles_.cend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    handle_iterator_wrapper_t::handle_iterator_wrapper_t(
      base_packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    handle_iterator_wrapper_t::begin() -> handle_iterator
  {
    return pht_->hbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    handle_iterator_wrapper_t::end() -> handle_iterator
  {
    return pht_->hend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_handle_iterator_wrapper_t::const_handle_iterator_wrapper_t(
      const base_packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_handle_iterator_wrapper_t::begin() const -> const_handle_iterator
  {
    return pht_->hbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_handle_iterator_wrapper_t::cbegin() const -> const_handle_iterator
  {
    return pht_->hcbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_handle_iterator_wrapper_t::end() const -> const_handle_iterator
  {
    return pht_->hend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_handle_iterator_wrapper_t::cend() const -> const_handle_iterator
  {
    return pht_->hcend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    value_iterator_wrapper_t::value_iterator_wrapper_t(
      base_packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    value_iterator_wrapper_t::begin() -> value_iterator
  {
    return pht_->vbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    value_iterator_wrapper_t::end() -> value_iterator
  {
    return pht_->vend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_value_iterator_wrapper_t::const_value_iterator_wrapper_t(
      const base_packed_hashtable_t& pht)
    : pht_(&pht)
  {
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_value_iterator_wrapper_t::begin() const -> const_value_iterator
  {
    return pht_->vbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_value_iterator_wrapper_t::cbegin() const -> const_value_iterator
  {
    return pht_->vcbegin();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_value_iterator_wrapper_t::end() const -> const_value_iterator
  {
    return pht_->vend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    const_value_iterator_wrapper_t::cend() const -> const_value_iterator
  {
    return pht_->vcend();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iteration()
    -> handle_iterator_wrapper_t
  {
    return handle_iterator_wrapper_t(*this);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::handle_iteration() const
    -> const_handle_iterator_wrapper_t
  {
    return const_handle_iterator_wrapper_t(*this);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::value_iteration()
    -> value_iterator_wrapper_t
  {
    return value_iterator_wrapper_t(*this);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  auto base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::value_iteration() const
    -> const_value_iterator_wrapper_t
  {
    return const_value_iterator_wrapper_t(*this);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Compare>
  void base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::sort(Compare&& compare)
  {
    sort(0, size(), std::forward<Compare>(compare));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Compare>
  void base_packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::
    sort(const int32_t begin, const int32_t end, Compare&& compare)
  {
    values_.sort(begin, end, std::forward<Compare>(compare));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename RemovalPolicy>
  template<typename Predicate>
  int32_t base_packed_hashtable_t<
    Key, Value, Hash, KeyEqual, Tag, RemovalPolicy>::partition(Predicate&&
                                                                 predicate)
  {
    return values_.partition(std::forward<Predicate>(predicate));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag>
  void packed_hashtable_rl_t<Key, Value, Hash, KeyEqual, Tag>::add_mapping(
    const typed_handle_t<Tag> handle, const Key* key)
  {
    handles_to_keys_.insert({handle, key});
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag>
  void packed_hashtable_rl_t<Key, Value, Hash, KeyEqual, Tag>::remove_mapping(
    const typed_handle_t<Tag> handle)
  {
    handles_to_keys_.erase(handle);
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag>
  void packed_hashtable_rl_t<Key, Value, Hash, KeyEqual, Tag>::clear_mappings()
  {
    handles_to_keys_.clear();
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag>
  bool packed_hashtable_rl_t<Key, Value, Hash, KeyEqual, Tag>::remove(
    const typed_handle_t<Tag> handle)
  {
    if (this->values_.remove(handle)) {
      if (const auto handle_key = handles_to_keys_.find(handle);
          handle_key != handles_to_keys_.end()) {
        const auto key = handle_key->second;
        handles_to_keys_.erase(handle_key);
        return this->keys_to_handles_.erase(*key) != 0;
      }
    }
    return false;
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag>
  std::optional<Key> packed_hashtable_rl_t<Key, Value, Hash, KeyEqual, Tag>::
    key_from_handle(const typed_handle_t<Tag> handle) const
  {
    if (auto key_it = handles_to_keys_.find(handle);
        key_it != handles_to_keys_.end()) {
      return *key_it->second;
    }
    return {};
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag>
  std::optional<Key> packed_hashtable_rl_t<
    Key, Value, Hash, KeyEqual, Tag>::key_from_index(const int32_t index) const
  {
    return key_from_handle(this->handle_from_index(index));
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename Pred>
  int32_t remove_when(
    packed_hashtable_rl_t<Key, Value, Hash, KeyEqual, Tag>& packed_hashtable_rl,
    const Pred pred)
  {
    const auto old_size = packed_hashtable_rl.size();
    for (auto it = packed_hashtable_rl.vbegin();
         it != packed_hashtable_rl.vend();) {
      if (pred(*it)) {
        const auto handle =
          packed_hashtable_rl.handle_from_index(static_cast<int32_t>(
            std::distance(packed_hashtable_rl.vbegin(), it)));
        packed_hashtable_rl.remove(handle);
      } else {
        ++it;
      }
    }
    return static_cast<int32_t>(old_size - packed_hashtable_rl.size());
  }

  template<
    typename Key, typename Value, typename Hash, typename KeyEqual,
    typename Tag, typename Pred>
  int32_t remove_when(
    packed_hashtable_t<Key, Value, Hash, KeyEqual, Tag>& packed_hashtable,
    Pred pred)
  {
    const auto old_size = packed_hashtable.size();
    for (auto it = packed_hashtable.hbegin(); it != packed_hashtable.hend();) {
      if (const auto result = packed_hashtable.call_return(
            it->second, [&pred](const auto& value) { return pred(value); });
          result.has_value() && result.value()) {
        it = packed_hashtable.remove(it);
      } else {
        ++it;
      }
    }
    return old_size - packed_hashtable.size();
  }
} // namespace thh
