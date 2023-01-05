// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 }  }

#include "../../../include/std/basic_optional"

using pair_t = std::pair<int, int>;
using opt_t = std::basic_optional<pair_t>;

static_assert(std::is_copy_constructible_v<opt_t::value_type>);
static_assert(std::is_copy_assignable_v<opt_t::value_type>);

static_assert(std::is_copy_assignable_v<opt_t>); // assertion fails.

class A
{
  void f(const opt_t& opt)
  {
    _opt = opt;
  }

  opt_t _opt;
};
