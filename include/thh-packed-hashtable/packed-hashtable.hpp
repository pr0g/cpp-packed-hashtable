#pragma once

#include <thh-handle-vector/handle-vector.hpp>
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
    handle_iterator find(const Key& key);
    const_handle_iterator find(const Key& key) const;
    handle_iterator remove(const Key& key);
    handle_iterator remove(handle_iterator position);
    [[nodiscard]] bool has(const Key& key) const;
    [[nodiscard]] int32_t capacity() const;
    void clear();
    void reserve(int32_t capacity);
    [[nodiscard]] int32_t size() const;
    [[nodiscard]] bool empty() const;
    template<typename Fn>
    void call(const Key& key, Fn&& fn);
    template<typename Fn>
    void call(packed_hashtable_handle_t handle, Fn&& fn);
    template<typename Fn>
    void call(const Key& key, Fn&& fn) const;
    template<typename Fn>
    void call(packed_hashtable_handle_t handle, Fn&& fn) const;
    template<typename Fn>
    decltype(auto) call_return(const Key& key, Fn&& fn);
    template<typename Fn>
    decltype(auto) call_return(packed_hashtable_handle_t handle, Fn&& fn);
    template<typename Fn>
    decltype(auto) call_return(const Key& key, Fn&& fn) const;
    template<typename Fn>
    decltype(auto) call_return(packed_hashtable_handle_t handle, Fn&& fn) const;
    [[nodiscard]] auto vbegin() -> value_iterator;
    [[nodiscard]] auto vbegin() const -> const_value_iterator;
    [[nodiscard]] auto vcbegin() const -> const_value_iterator;
    [[nodiscard]] auto vend() -> value_iterator;
    [[nodiscard]] auto vend() const -> const_value_iterator;
    [[nodiscard]] auto vcend() const -> const_value_iterator;
    [[nodiscard]] auto hbegin() -> handle_iterator;
    [[nodiscard]] auto hbegin() const -> const_handle_iterator;
    [[nodiscard]] auto hcbegin() const -> const_handle_iterator;
    [[nodiscard]] auto hend() -> handle_iterator;
    [[nodiscard]] auto hend() const -> const_handle_iterator;
    [[nodiscard]] auto hcend() const -> const_handle_iterator;

    class handle_iterator_wrapper_t
    {
      packed_hashtable_t* pht_ = nullptr;

    public:
      handle_iterator_wrapper_t(packed_hashtable_t& pht) : pht_(&pht) {}
      auto begin() -> handle_iterator { return pht_->hbegin(); }
      auto end() -> handle_iterator { return pht_->hend(); }
    };

    class const_handle_iterator_wrapper_t
    {
      const packed_hashtable_t* pht_ = nullptr;

    public:
      const_handle_iterator_wrapper_t(const packed_hashtable_t& pht)
        : pht_(&pht)
      {
      }
      auto begin() const -> const_handle_iterator { return pht_->hbegin(); }
      auto cbegin() const -> const_handle_iterator { return pht_->hcbegin(); }
      auto end() const -> const_handle_iterator { return pht_->hend(); }
      auto cend() const -> const_handle_iterator { return pht_->hcend(); }
    };

    class value_iterator_wrapper_t
    {
      packed_hashtable_t* pht_ = nullptr;

    public:
      value_iterator_wrapper_t(packed_hashtable_t& pht) : pht_(&pht) {}
      auto begin() -> value_iterator { return pht_->vbegin(); }
      auto end() -> value_iterator { return pht_->vend(); }
    };

    class const_value_iterator_wrapper_t
    {
      const packed_hashtable_t* pht_ = nullptr;

    public:
      const_value_iterator_wrapper_t(const packed_hashtable_t& pht) : pht_(&pht)
      {
      }
      auto begin() const -> const_value_iterator { return pht_->vbegin(); }
      auto cbegin() const -> const_value_iterator { return pht_->vcbegin(); }
      auto end() const -> const_value_iterator { return pht_->vend(); }
      auto cend() const -> const_value_iterator { return pht_->vcend(); }
    };

    auto handle_iteration() -> handle_iterator_wrapper_t
    {
      return handle_iterator_wrapper_t(*this);
    }

    auto handle_iteration() const -> const_handle_iterator_wrapper_t
    {
      return const_handle_iterator_wrapper_t(*this);
    }

    auto value_iteration() -> value_iterator_wrapper_t
    {
      return value_iterator_wrapper_t(*this);
    }

    auto value_iteration() const -> const_value_iterator_wrapper_t
    {
      return const_value_iterator_wrapper_t(*this);
    }

  private:
    template<typename P>
    std::pair<handle_iterator, bool> add_internal(P&& key_value);
    template<typename P>
    std::pair<handle_iterator, bool> add_or_update_internal(P&& key_value);
  };
} // namespace thh

#include "packed-hashtable.inl"
