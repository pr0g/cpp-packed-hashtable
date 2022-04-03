#pragma once

#include <thh-handle-vector/handle-vector.hpp>
#include <unordered_map>

namespace thh
{
  // default tag for packed_hashtable handles
  struct packed_hashtable_tag_t
  {
  };

  // alias for default packed hashtable handle if a custom tag is not used
  using packed_hashtable_handle_t = typed_handle_t<packed_hashtable_tag_t>;

  // hybrid lookup container for efficient element iteration at the cost of
  // additional memory usage
  // values are stored in a handle_vector_t (elements are tightly packed and are
  // accessible through an indirect handle as well as direct iteration)
  // keys are stored in an unordered_map and its values are the handles to the
  // underlying elements stored in the handle_vector_t
  template<typename Key, typename Value, typename Tag = packed_hashtable_tag_t>
  class packed_hashtable_t
  {
    // store for underlying values
    handle_vector_t<Value, Tag> values_;
    // key to handle mapping (key -> handle -> value)
    std::unordered_map<Key, typed_handle_t<Tag>> handles_;

  public:
    using key_value_type = std::pair<const Key, Value>;
    using value_iterator = typename decltype(values_)::iterator;
    using const_value_iterator = typename decltype(values_)::const_iterator;
    using handle_iterator = typename decltype(handles_)::iterator;
    using const_handle_iterator = typename decltype(handles_)::const_iterator;

    // adds a value to the container (forwarding reference)
    // returns a pair consisting of an interator to the inserted element (or to
    // the element that prevented the insertion) and a bool indicating whether
    // the insertion took place
    // type P should conform to key_value_type
    template<typename P>
    std::pair<handle_iterator, bool> add(P&& key_value);
    // adds a value to the container (rvalue reference)
    // returns a pair consisting of an interator to the inserted element (or to
    // the element that prevented the insertion) and a bool indicating whether
    // the insertion took place
    // note: supports .add({key, value}) syntax
    std::pair<handle_iterator, bool> add(key_value_type&& key_value);
    // adds a value to the container or updates it if the key already exists
    // returns a pair consisting of an interator to the inserted or updated
    // element and a bool indicating whether whether the insertion took place
    // type P should conform to key_value_type
    template<typename P>
    std::pair<handle_iterator, bool> add_or_update(P&& key_value);
    // adds a value to the container or updates it if the key already exists
    // returns a pair consisting of an interator to the inserted or updated
    // element and a bool indicating whether whether the insertion took place
    // note: supports .add({key, value}) syntax
    std::pair<handle_iterator, bool> add_or_update(key_value_type&& key_value);
    // finds a handle with the specified key
    // returns an iterator to the discovered element or one past the end if the
    // element was not found (hend())
    [[nodiscard]] handle_iterator find(const Key& key);
    // finds a handle with the specified key (const overload)
    // returns an iterator to the discovered element or one past the end if the
    // element was not found (hend())
    [[nodiscard]] const_handle_iterator find(const Key& key) const;
    // removes the element with the equivalent key (if one exists)
    // returns an interator following the last removed element or one past the
    // end if the element was not found (hend())
    handle_iterator remove(const Key& key);
    // removes the element at position
    // returns an interator following the last removed element (position must
    // be valid and dereferenceable)
    handle_iterator remove(handle_iterator position);
    // returns if the container has an element with the equivalent key
    [[nodiscard]] bool has(const Key& key) const;
    // returns the number of available handles (includes element storage that is
    // reserved but not yet in use)
    // the capacity refers to the values underlying storage, not the key-handle
    // pairs
    [[nodiscard]] int32_t capacity() const;
    // removes all elements from the container
    // note: will invalidate all handles
    // note: capacity remains unchanged, internal handles are not cleared
    void clear();
    // reserves underlying memory for the number of elements specified
    // note: will attempt to reserve capacity for the key-handle pairs as well
    void reserve(int32_t capacity);
    // returns the number of elements currently stored in the container
    [[nodiscard]] int32_t size() const;
    // returns if the container has any elements or not
    [[nodiscard]] bool empty() const;
    // invokes a callable object on an element in the container using a key
    template<typename Fn>
    void call(const Key& key, Fn&& fn);
    // invokes a callable object on an element in the container using a handle
    template<typename Fn>
    void call(typed_handle_t<Tag> handle, Fn&& fn);
    // invokes a callable object on an element in the container using a key
    // (const overload)
    template<typename Fn>
    void call(const Key& key, Fn&& fn) const;
    // invokes a callable object on an element in the container using a handle
    // (const overload)
    template<typename Fn>
    void call(typed_handle_t<Tag> handle, Fn&& fn) const;
    // invokes a callable object on an element in the container and returns a
    // std::optional containing either the result or an empty optional (as the
    // key may not have been found)
    template<typename Fn>
    decltype(auto) call_return(const Key& key, Fn&& fn);
    // invokes a callable object on an element in the container and returns a
    // std::optional containing either the result or an empty optional (as the
    // handle may not have been successfully resolved)
    template<typename Fn>
    decltype(auto) call_return(typed_handle_t<Tag> handle, Fn&& fn);
    // invokes a callable object on an element in the container and returns a
    // std::optional containing either the result or an empty optional (as the
    // key may not have been found)
    // (const overload)
    template<typename Fn>
    decltype(auto) call_return(const Key& key, Fn&& fn) const;
    // invokes a callable object on an element in the container and returns a
    // std::optional containing either the result or an empty optional (as the
    // handle may not have been successfully resolved)
    // (const overload)
    template<typename Fn>
    decltype(auto) call_return(typed_handle_t<Tag> handle, Fn&& fn) const;
    // returns an iterator to the beginning of the values (contiguous)
    [[nodiscard]] auto vbegin() -> value_iterator;
    // returns a const iterator to the beginning of the values (contiguous)
    [[nodiscard]] auto vbegin() const -> const_value_iterator;
    // returns a const iterator to the beginning of the values (contiguous)
    [[nodiscard]] auto vcbegin() const -> const_value_iterator;
    // returns an iterator to the end of the values (contiguous)
    [[nodiscard]] auto vend() -> value_iterator;
    // returns a const iterator to the end of the values (contiguous)
    [[nodiscard]] auto vend() const -> const_value_iterator;
    // returns a const iterator to the end of the values (contiguous)
    [[nodiscard]] auto vcend() const -> const_value_iterator;
    // returns an iterator to the beginning of the handles (sparse)
    [[nodiscard]] auto hbegin() -> handle_iterator;
    // returns a const iterator to the beginning of the handles (sparse)
    [[nodiscard]] auto hbegin() const -> const_handle_iterator;
    // returns a const iterator to the beginning of the handles (sparse)
    [[nodiscard]] auto hcbegin() const -> const_handle_iterator;
    // returns an iterator to the end of the handles (sparse)
    [[nodiscard]] auto hend() -> handle_iterator;
    // returns a const iterator to the end of the handles (sparse)
    [[nodiscard]] auto hend() const -> const_handle_iterator;
    // returns a const iterator to the end of the handles (sparse)
    [[nodiscard]] auto hcend() const -> const_handle_iterator;

    // proxy to support friendly iteration for handles (see handle_iteration())
    // note: to be used with range based for
    // for (auto handle : packed_hashtable.handle_iteration())
    class handle_iterator_wrapper_t
    {
      packed_hashtable_t* pht_ = nullptr;

    public:
      handle_iterator_wrapper_t(packed_hashtable_t& pht);
      [[nodiscard]] auto begin() -> handle_iterator;
      [[nodiscard]] auto end() -> handle_iterator;
    };

    // proxy to support friendly iteration for handles (see handle_iteration())
    // note: to be used with range based for (const version)
    // for (auto handle : packed_hashtable.handle_iteration())
    class const_handle_iterator_wrapper_t
    {
      const packed_hashtable_t* pht_ = nullptr;

    public:
      const_handle_iterator_wrapper_t(const packed_hashtable_t& pht);
      [[nodiscard]] auto begin() const -> const_handle_iterator;
      [[nodiscard]] auto cbegin() const -> const_handle_iterator;
      [[nodiscard]] auto end() const -> const_handle_iterator;
      [[nodiscard]] auto cend() const -> const_handle_iterator;
    };

    // proxy to support friendly iteration for values (see value_iteration())
    // note: to be used with range based for
    // for (auto& value : packed_hashtable.value_iteration())
    class value_iterator_wrapper_t
    {
      packed_hashtable_t* pht_ = nullptr;

    public:
      value_iterator_wrapper_t(packed_hashtable_t& pht);
      [[nodiscard]] auto begin() -> value_iterator;
      [[nodiscard]] auto end() -> value_iterator;
    };

    // proxy to support friendly iteration for values (see value_iteration())
    // note: to be used with range based for (const version)
    // for (const auto& value : packed_hashtable.value_iteration())
    class const_value_iterator_wrapper_t
    {
      const packed_hashtable_t* pht_ = nullptr;

    public:
      const_value_iterator_wrapper_t(const packed_hashtable_t& pht);
      [[nodiscard]] auto begin() const -> const_value_iterator;
      [[nodiscard]] auto cbegin() const -> const_value_iterator;
      [[nodiscard]] auto end() const -> const_value_iterator;
      [[nodiscard]] auto cend() const -> const_value_iterator;
    };

    // returns a proxy object to the container to provide begin/end iterators
    // for handles (to be used with range based for loop)
    [[nodiscard]] auto handle_iteration() -> handle_iterator_wrapper_t;
    // returns a proxy object to the container to provide begin/end iterators
    // for handles (to be used with range based for loop) (const overload)
    [[nodiscard]] auto handle_iteration() const
      -> const_handle_iterator_wrapper_t;
    // returns a proxy object to container to provide begin/end iterators for
    // values (to be used with range based for loop)
    [[nodiscard]] auto value_iteration() -> value_iterator_wrapper_t;
    // returns a proxy object to container to provide begin/end iterators for
    // values (to be used with range based for loop) (const overload)
    [[nodiscard]] auto value_iteration() const
      -> const_value_iterator_wrapper_t;

  private:
    // internal implementation of add, used by both public add overloads
    template<typename P>
    std::pair<handle_iterator, bool> add_internal(P&& key_value);
    // internal implementation of add_or_update, used by both public
    // add_or_update overloads
    template<typename P>
    std::pair<handle_iterator, bool> add_or_update_internal(P&& key_value);
  };
} // namespace thh

#include "packed-hashtable.inl"
