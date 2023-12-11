// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 } }

#include "../../../include/std/basic_variant.h"

// P2062R2 Inheriting from std::variant (resolving LWG 3052)


struct V : std::basic_variant<std::allocator<int>, int> {
    using std::basic_variant<std::allocator<int>, int>::basic_variant;
};

constexpr int
test01()
{
  V v = 42;
  return std::visit([](int&){ return 17; }, v);
}
static_assert( test01() == 17 );

constexpr int
test02()
{
  const V c = 77;
  std::basic_variant<std::allocator<int>, char*, long> x = 88L;
  return std::visit([](auto&& a, auto&& b) {
    if constexpr (std::is_same_v<decltype(a), const int&&>)
      if constexpr (std::is_same_v<decltype(b), long&&>)
	return 99;
    return 0;
  },
  std::move(c), std::move(x));
}
static_assert( test02() == 99 );
