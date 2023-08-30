// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 }  }

#include "../../../../include/std/basic_variant"

#include <tuple>

int main() {
  using variant_t = std::basic_variant<std::allocator<int>,short, int, long>;
  constexpr auto variant_v = variant_t{std::in_place_index_t<0>{}, short{}};
  constexpr auto tuple = std::make_tuple(variant_v);
  constexpr std::tuple tuple_v{variant_v};
}
