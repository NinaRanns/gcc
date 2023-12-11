// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

#include <tuple>
#include "../../../include/std/basic_variant.h"

int main() {
  using variant_t = std::basic_variant<std::allocator<int>, short, int, long>;
  constexpr auto variant_v = variant_t{std::in_place_index_t<0>{}, short{}};
  constexpr auto tuple = std::make_tuple(variant_v);
  constexpr std::tuple tuple_v{variant_v};
}
