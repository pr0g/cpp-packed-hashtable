#include <iostream>

#include <algorithm>
#include <numeric>
#include <thh-packed-hashtable/packed-hashtable.hpp>

template<>
struct std::hash<thh::typed_handle_t<struct entity_id_tag>>
{
  size_t operator()(
    const thh::typed_handle_t<struct entity_id_tag>& handle) const
  {
    // based on boost hash combine function
    // ref:
    // boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
    const auto hash_combine_fn = [](std::size_t& seed, const auto& v) {
      std::hash<typename std::remove_const_t<
        typename std::remove_reference_t<decltype(v)>>>
        hasher;
      seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };

    std::size_t seed = 0;
    hash_combine_fn(seed, handle.gen_);
    hash_combine_fn(seed, handle.id_);
    return seed;
  }
};

int main(int argc, char** argv)
{
  struct transform_component_t
  {
    char id_;
  };

  struct physics_component_t
  {
    char id_;
  };

  // struct entity_id_hash_t
  // {
  //   std::size_t operator()(const entity_id_t<Tag>& handle) const
  //   {
  //     // based on boost hash combine function
  //     // ref:
  //     //
  //     boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
  //     const auto hash_combine_fn = [](std::size_t& seed, const auto& v) {
  //       std::hash<typename std::remove_const_t<
  //         typename std::remove_reference_t<decltype(v)>>>
  //         hasher;
  //       seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  //     };

  //     std::size_t seed = 0;
  //     hash_combine_fn(seed, handle.gen_);
  //     hash_combine_fn(seed, handle.id_);
  //     return seed;
  //   }
  // };

  // thh::handle_vector_t<transform_component_t> transform_components;
  // thh::handle_vector_t<physics_component_t> physics_components;

  struct empty_t
  {
  };

  using entity_id_t = thh::typed_handle_t<struct entity_id_tag>;

  thh::handle_vector_t<empty_t, struct entity_id_tag> entity_ids;

  constexpr int EntityCount = 6;
  std::vector<entity_id_t> entity_handles;
  entity_handles.reserve(EntityCount);
  for (int i = 0; i < EntityCount; ++i) {
    entity_handles.push_back(entity_ids.add());
  }

  for (int i = 0; i < EntityCount; ++i) {
    auto handle = entity_ids.handle_from_index(i);
    std::cout << handle.id_ << ", ";
  }

  std::cout << '\n';

  //  for (int i = (int)entity_handles.size() - 1; i >= 0; --i) {
  //    if (i % 2 == 0) {
  //      entity_ids.remove(entity_handles[i]);
  //      entity_handles.erase(entity_handles.begin() + i);
  //    }
  //  }

  //  for (int i = 0; i < EntityCount; ++i) {
  //    auto handle = entity_ids.handle_from_index(i);
  //    std::cout << handle.id_ << ", ";
  //  }
  //
  //  std::cout << '\n';

  //  for (int i = 0; i < EntityCount / 2; ++i) {
  //    entity_handles.push_back(entity_ids.add());
  //  }

  //  for (int i = 0; i < EntityCount; ++i) {
  //    auto handle = entity_ids.handle_from_index(i);
  //    std::cout << handle.id_ << ", ";
  //  }
  //
  //  std::cout << '\n';

  thh::packed_hashtable_rl_t<
    entity_id_t, transform_component_t, struct transform_tag>
    transform_components;

  thh::packed_hashtable_rl_t<
    entity_id_t, physics_component_t, struct physics_tag>
    physics_components;

  //  for (int i = 0; i < entity_handles.size(); ++i) {
  //    transform_components.add(
  //      {entity_handles[i], transform_component_t{(char)(97 + i)}});
  //  }

  transform_components.add({entity_handles[2], transform_component_t{'c'}});
  transform_components.add({entity_handles[1], transform_component_t{'b'}});
  transform_components.add({entity_handles[5], transform_component_t{'f'}});
  transform_components.add({entity_handles[4], transform_component_t{'e'}});
  transform_components.add({entity_handles[0], transform_component_t{'a'}});
  transform_components.add({entity_handles[3], transform_component_t{'d'}});

  //  for (int i = 5; i < EntityCount; ++i) {
  //  physics_components.add({entity_handles[9], physics_component_t{}});
  //  physics_components.add({entity_handles[5], physics_component_t{}});
  //  physics_components.add({entity_handles[7], physics_component_t{}});
  //  physics_components.add({entity_handles[6], physics_component_t{}});
  //  physics_components.add({entity_handles[8], physics_component_t{}});
  physics_components.add({entity_handles[4], physics_component_t{'e'}});
  physics_components.add({entity_handles[5], physics_component_t{'f'}});
  physics_components.add({entity_handles[0], physics_component_t{'a'}});
  physics_components.add({entity_handles[1], physics_component_t{'b'}});
  physics_components.add({entity_handles[3], physics_component_t{'d'}});
  physics_components.add({entity_handles[2], physics_component_t{'c'}});
  //  }

  auto transform_components_display = [&transform_components] {
    std::cout << "transform components entity ids\n";
    for (auto it = transform_components.vbegin();
         it != transform_components.vend(); ++it) {
      const auto handle = transform_components.handle_from_index(
        static_cast<int32_t>(std::distance(transform_components.vbegin(), it)));
      if (auto entity_id = transform_components.key_from_handle(handle);
          entity_id.has_value()) {
        std::cout << entity_id->id_ << ", ";
      }
    }
    std::cout << '\n';

    std::cout << "transform component values\n";
    for (const auto& v : transform_components.value_iteration()) {
      std::cout << v.id_ << ", ";
    }

    std::cout << '\n';
  };

  auto physics_components_display = [&physics_components] {
    std::cout << "physics components entity ids\n";
    for (auto it = physics_components.vbegin(); it != physics_components.vend();
         ++it) {
      const auto handle = physics_components.handle_from_index(
        static_cast<int32_t>(std::distance(physics_components.vbegin(), it)));
      if (auto entity_id = physics_components.key_from_handle(handle);
          entity_id.has_value()) {
        std::cout << entity_id->id_ << ", ";
      }
    }
    std::cout << '\n';

    std::cout << "physics component values\n";
    for (const auto& v : physics_components.value_iteration()) {
      std::cout << v.id_ << ", ";
    }

    std::cout << '\n';
  };

  transform_components_display();
  physics_components_display();

  std::vector<entity_id_t> physics_value_order_entity_ids;
  // physics components is smaller than transform
  // build entity id list for physics components
  for (auto it = physics_components.vbegin(); it != physics_components.vend();
       ++it) {
    const auto handle = physics_components.handle_from_index(
      static_cast<int32_t>(std::distance(physics_components.vbegin(), it)));
    if (auto entity_id = physics_components.key_from_handle(handle);
        entity_id.has_value()) {
      physics_value_order_entity_ids.push_back(entity_id.value());
    }
  }

  physics_components.sort(
    [&physics_value_order_entity_ids](int32_t lhs, int32_t rhs) {
      return physics_value_order_entity_ids[lhs].id_
           < physics_value_order_entity_ids[rhs].id_;
    });

  std::vector<entity_id_t> transform_value_order_entity_ids;
  for (auto it = transform_components.vbegin();
       it != transform_components.vend(); ++it) {
    const auto handle = transform_components.handle_from_index(
      static_cast<int32_t>(std::distance(transform_components.vbegin(), it)));
    if (auto entity_id = transform_components.key_from_handle(handle);
        entity_id.has_value()) {
      transform_value_order_entity_ids.push_back(entity_id.value());
    }
  }

  transform_components.sort(
    [&transform_components, &physics_components,
     &transform_value_order_entity_ids](int32_t lhs, int32_t rhs) {
      // auto lhh = transform_components.handle_from_index(lhs);
      // auto rhh = transform_components.handle_from_index(rhs);

      // auto lhk = transform_components.key_from_handle(lhh);
      // auto rhk = transform_components.key_from_handle(rhh);

      return transform_value_order_entity_ids[lhs].id_
           < transform_value_order_entity_ids[rhs].id_;
    });

  auto t0 = transform_components
              .call_return(entity_handles[0], [](const auto v) { return v; })
              .value();
  auto t1 = transform_components
              .call_return(entity_handles[1], [](const auto v) { return v; })
              .value();
  auto t2 = transform_components
              .call_return(entity_handles[2], [](const auto v) { return v; })
              .value();
  auto t3 = transform_components
              .call_return(entity_handles[3], [](const auto v) { return v; })
              .value();
  auto t4 = transform_components
              .call_return(entity_handles[4], [](const auto v) { return v; })
              .value();
  auto t5 = transform_components
              .call_return(entity_handles[5], [](const auto v) { return v; })
              .value();

  transform_components_display();
  physics_components_display();

  for (auto& value_order_entity_id : transform_value_order_entity_ids) {
    std::cout << value_order_entity_id.id_ << ", ";
  }

  std::cout << '\n';

  for (auto& value_order_entity_id : physics_value_order_entity_ids) {
    std::cout << value_order_entity_id.id_ << ", ";
  }

  std::cout << '\n';

  auto physics_system_fn =
    [](transform_component_t& transform, physics_component_t& physics) {

    };

  // find the smallest list of components
  // partition other set of components based on those in the set or not
  // sort valid (left) partition based on entity id
  // iterate

  // need to sort by shortest list of components

  auto t_it = transform_components.vbegin();
  auto p_it = physics_components.vbegin();
  for (;
       t_it != transform_components.vend() && p_it != physics_components.vend();
       ++p_it, ++t_it) {
    physics_system_fn(*t_it, *p_it);
  }

  std::vector<int> outside_handles = {0, 1, 2, 3, 4, 5};
  std::vector<int> internal_handles = {2, 0, 5, 1, 3, 4};
  std::vector<char> internal_values = {'f', 'c', 'e', 'a', 'd', 'b'};

  for (int lookup : outside_handles) {
    std::cout << internal_values[internal_handles[lookup]] << ", ";
  }

  std::cout << '\n';

  std::vector<int32_t> indices(6);
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(
    indices.begin(), indices.end(), [&](const int32_t lhs, const int32_t rhs) {
      return internal_values[lhs] < internal_values[rhs];
    });

  for (int i = 0; i < internal_handles.size(); ++i) {
    auto other = std::find(indices.begin(), indices.end(), internal_handles[i]);
    if (other != indices.end()) {
      internal_handles[i] = std::distance(indices.begin(), other);
    }
  }
  thh::apply_permutation(internal_values, indices);

  // std::sort(
  //   internal_values.begin(), internal_values.end(), [&](const char lhs, const
  //   char rhs) {
  //     return lhs < rhs;
  //   });

  for (int internal_value : internal_values) {
    std::cout << (char)internal_value << ", ";
  }

  std::cout << '\n';

  for (int lookup : outside_handles) {
    std::cout << internal_values[internal_handles[lookup]] << ", ";
  }

  std::cout << '\n';

  return 0;
}
