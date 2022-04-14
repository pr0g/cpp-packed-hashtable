#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <thh-packed-hashtable/packed-hashtable.hpp>

TEST_CASE("Can allocate packed hashtable")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  CHECK(true);
}

TEST_CASE("Container size zero after init")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  const auto dense_map_size = packed_hashtable.size();
  CHECK(dense_map_size == 0);
}

TEST_CASE("Container empty after init")
{
  thh::packed_hashtable_t<char, char> packed_hashtable;
  CHECK(packed_hashtable.empty());
}

TEST_CASE("Container not empty after add")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add(std::pair{5, 'a'});
  CHECK(!packed_hashtable.empty());
}

TEST_CASE("Container size increase by one after add")
{
  thh::packed_hashtable_t<int, char> packed_hashtable;
  packed_hashtable.add(std::pair{5, 'a'});
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Elements can be reserved")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.reserve(10);
  CHECK(packed_hashtable.size() == 0);
  CHECK(packed_hashtable.capacity() == 10);
}

TEST_CASE("Container can be cleared")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  packed_hashtable.clear();

  CHECK(packed_hashtable.size() == 0);
  CHECK(packed_hashtable.empty());

  for (const auto& key : {1, 2, 3}) {
    CHECK(!packed_hashtable.has(key));
  }
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition (l-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_key_value_pair = std::pair<const int, std::string>{2, "a"};
  auto second_key_value_pair = std::pair<const int, std::string>{2, "b"};
  auto first_add = packed_hashtable.add(first_key_value_pair);
  auto second_add = packed_hashtable.add(second_key_value_pair);

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "a");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition and updates value (l-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_key_value_pair = std::pair<const int, std::string>{2, "a"};
  auto second_key_value_pair = std::pair<const int, std::string>{2, "b"};
  auto first_add = packed_hashtable.add_or_update(first_key_value_pair);
  auto second_add = packed_hashtable.add_or_update(second_key_value_pair);

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "b");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition (r-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_add = packed_hashtable.add({2, "a"});
  auto second_add = packed_hashtable.add({2, "b"});

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "a");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Adding same key with add or update returns iterator that prevented "
          "addition and updates value (r-value)")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  auto first_add = packed_hashtable.add_or_update({2, "a"});
  auto second_add = packed_hashtable.add_or_update({2, "b"});

  CHECK(first_add.first->first == 2);
  CHECK(first_add.second);
  CHECK(second_add.first->first == 2);
  CHECK(!second_add.second);
  CHECK(second_add.first->second == first_add.first->second);

  bool called = false;
  packed_hashtable.call(2, [&called](const std::string& c) {
    CHECK(c == "b");
    called = true;
  });

  CHECK(called);
  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Move only type can be added to packed_hashtable")
{
  struct object_t
  {
    object_t(int i, int j) : i_(i), j_(j) {}
    object_t(object_t&&) = default;
    int i_;
    int j_;
  };

  thh::packed_hashtable_t<char, object_t> packed_hashtable;
  packed_hashtable.add({'a', object_t{1, 2}});

  bool called = false;
  packed_hashtable.call('a', [&called](const object_t& c) {
    called = true;
    CHECK(c.i_ == 1);
    CHECK(c.j_ == 2);
  });

  CHECK(called);
}

TEST_CASE("Visit values via handle iterators and key lookup")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({123, "test"});
  packed_hashtable.add({456, "the"});
  packed_hashtable.add({987, "iteration"});

  std::string expected = "testtheiteration";
  std::sort(expected.begin(), expected.end());

  std::string outcome;
  std::for_each(
    packed_hashtable.hbegin(), packed_hashtable.hend(),
    [&packed_hashtable, &outcome](const auto& key_handle) {
      packed_hashtable.call(
        key_handle.first,
        [&outcome](const std::string& value) { outcome += value; });
    });

  std::sort(outcome.begin(), outcome.end());
  CHECK(outcome == expected);
}

TEST_CASE("Visit values via handle iterators and handle lookup")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({123, "test"});
  packed_hashtable.add({456, "the"});
  packed_hashtable.add({987, "iteration"});

  std::string expected = "testtheiteration";
  std::sort(expected.begin(), expected.end());

  std::string outcome;
  std::for_each(
    packed_hashtable.hbegin(), packed_hashtable.hend(),
    [&packed_hashtable, &outcome](const auto& key_handle) {
      packed_hashtable.call(
        key_handle.second,
        [&outcome](const std::string& value) { outcome += value; });
    });

  std::sort(outcome.begin(), outcome.end());
  CHECK(outcome == expected);
}

TEST_CASE("Container call overloads are called for const container with key")
{
  thh::packed_hashtable_t<char, std::string> packed_hashtable;
  packed_hashtable.add({'a', std::string{"letter-a"}});

  bool called = false;
  std::as_const(packed_hashtable).call('a', [&called](const std::string& str) {
    called = true;
    CHECK(str == std::string{"letter-a"});
  });

  CHECK(called);
}

TEST_CASE("Container call overloads are called for const container with handle")
{
  thh::packed_hashtable_t<char, std::string> packed_hashtable;
  packed_hashtable.add({'a', std::string{"letter-a"}});

  bool called = false;
  std::as_const(packed_hashtable)
    .call(packed_hashtable.hbegin()->second, [&called](const std::string& str) {
      called = true;
      CHECK(str == std::string{"letter-a"});
    });

  CHECK(called);
}

TEST_CASE("Value can be removed using key")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({5, "hello"});
  packed_hashtable.add({8, "world"});

  packed_hashtable.remove(8);

  CHECK(packed_hashtable.size() == 1);
}

TEST_CASE("Iterator to next handle is returned after remove")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({5, "hello"});
  packed_hashtable.add({8, "world"});

  const auto handle_it = packed_hashtable.remove(8);

  CHECK(handle_it->first == 5);
}

TEST_CASE("Values can be removed via iteration")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});
  packed_hashtable.add({4, "four"});
  packed_hashtable.add({5, "five"});
  packed_hashtable.add({6, "six"});

  // erase all odd numbers from table
  for (auto it = packed_hashtable.hbegin(); it != packed_hashtable.hend();) {
    if (it->first % 2 != 0) {
      it = packed_hashtable.remove(it);
    } else {
      ++it;
    }
  }

  std::string outcome;
  std::for_each(
    packed_hashtable.vbegin(), packed_hashtable.vend(),
    [&outcome](const auto& value) { outcome.append(value); });

  // sort characters to avoid any order dependence issues
  std::sort(outcome.begin(), outcome.end());

  CHECK(outcome == "fioorstuwx");
  CHECK(packed_hashtable.size() == 3);
}

TEST_CASE("Container has key after add")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  CHECK(packed_hashtable.has(1));
  CHECK(packed_hashtable.has(2));
  CHECK(packed_hashtable.has(3));
}

TEST_CASE("Container does not have key that wasn't added")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;

  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  CHECK(!packed_hashtable.has(4));
  CHECK(!packed_hashtable.has(5));
  CHECK(!packed_hashtable.has(6));
}

TEST_CASE("looping")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  for ([[maybe_unused]] auto& h : packed_hashtable.handle_iteration()) {
    // MESSAGE(h.first);
    // MESSAGE(h.second.gen_, h.second.id_);
  }

  for ([[maybe_unused]] const auto& v :
       std::as_const(packed_hashtable).value_iteration()) {
    // MESSAGE(v);
  }

  for ([[maybe_unused]] auto& v : packed_hashtable.value_iteration()) {
    // v += std::string("-again");
    // MESSAGE(v);
  }
}

TEST_CASE("Container handle can be found using key")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  const auto key_handle = packed_hashtable.find(3);

  CHECK(key_handle->first == 3);

  const auto expected = packed_hashtable.call_return(
    key_handle->second, [](const auto& value) { return value; });

  CHECK(expected == "three");
}

TEST_CASE("Const container handle can be found using key")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  const auto key_handle = std::as_const(packed_hashtable).find(1);

  CHECK(key_handle->first == 1);

  const auto expected =
    std::as_const(packed_hashtable)
      .call_return(key_handle->second, [](const auto& value) { return value; });

  CHECK(expected == "one");
}

TEST_CASE("Container handle not found for non-existent key or handle")
{
  thh::packed_hashtable_t<std::string, int> packed_hashtable;
  packed_hashtable.add({"one", 1});
  packed_hashtable.add({"two", 2});
  packed_hashtable.add({"three", 3});

  const auto key_handle = packed_hashtable.find(std::string("four"));
  CHECK(key_handle == packed_hashtable.hend());

  const auto expected = packed_hashtable.call_return(
    thh::packed_hashtable_handle_t{}, [](const auto& value) { return value; });

  CHECK(!expected.has_value());
}

TEST_CASE("Container call return with key produces expected value")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  auto two_expected = packed_hashtable.call_return(2, [](std::string& value) {
    value += "four";
    return value;
  });

  auto three_expected =
    std::as_const(packed_hashtable)
      .call_return(3, [](const std::string& value) { return value; });

  CHECK(two_expected == std::string("twofour"));
  CHECK(three_expected == std::string("three"));
}

TEST_CASE("Container call return with handle produces expected value")
{
  thh::packed_hashtable_t<int, std::string> packed_hashtable;
  packed_hashtable.add({1, "one"});
  packed_hashtable.add({2, "two"});
  packed_hashtable.add({3, "three"});

  auto key_handle_two = packed_hashtable.find(2);
  auto two_expected = packed_hashtable.call_return(
    key_handle_two->second, [](std::string& value) {
      value += "four";
      return value;
    });

  auto key_handle_three = packed_hashtable.find(3);
  auto three_expected =
    std::as_const(packed_hashtable)
      .call_return(key_handle_three->second, [](const std::string& value) {
        return value;
      });

  CHECK(two_expected == std::string("twofour"));
  CHECK(three_expected == std::string("three"));
}

struct vec3_t
{
  float x, y, z;
};

struct color_t
{
  float r, g, b, a;
};

// 48 bytes
struct particle_t
{
  vec3_t position_;
  vec3_t velocity_;
  color_t color_;
  float size_;
  float lifetime_;
};

TEST_CASE("test skipping")
{
  thh::packed_hashtable_rl_t<std::string, particle_t>
    packed_hashtable_particles;
  // thh::packed_hashtable_t<std::string, particle_t>
  // packed_hashtable_particles;

  // const char(
  //   *find_size_1)[sizeof(thh::packed_hashtable_t<std::string, particle_t>)] =
  //   1;
  // const char(*find_size_2)[sizeof(
  //   thh::packed_hashtable_simple_t<std::string, particle_t>)] = 1;

  packed_hashtable_particles.reserve(static_cast<int32_t>(64));
  for (int i = 0; i < 64; ++i) {
    auto particle = particle_t{};
    particle.lifetime_ = i % 2 == 0 ? 1.0f : 0.0f;
    packed_hashtable_particles.add(
      {std::string("name") + std::to_string(i), particle});
  }

  // std::for_each(
  // packed_hashtable_particles.vbegin(), packed_hashtable_particles.vend(),
  // [&packed_hashtable_particles, index = 0](const auto& value) mutable {
  // for (auto it = packed_hashtable_particles.vbegin();
  //      it != packed_hashtable_particles.vend();) {
  //   const auto& value = *it;
  //   if (value.lifetime_ <= 0.0f) {
  //     const auto handle =
  //       packed_hashtable_particles.handle_from_index(static_cast<int32_t>(
  //         std::distance(packed_hashtable_particles.vbegin(), it)));
  //     packed_hashtable_particles.remove(handle);
  //     // index++;
  //   } else {
  //     it++;
  //   }
  // }
  // });

  thh::remove_when(packed_hashtable_particles, [](const auto& value) {
    return value.lifetime_ <= 0.0f;
  });

  MESSAGE(packed_hashtable_particles.size());
}
