// { dg-do compile { target c++20 } }
// { dg-additional-options "-fcontracts -fcontracts-nonattr -fcontract-evaluation-semantic=enforce" }

// A variable v is constant-initializable if
// — the full-expression of its initialization is a constant expression when interpreted as a constant-expression
// with all contract assertions using the ignore evaluation semantic (6.11.2)
#include <iostream>
#include <type_traits>

bool check(){
  std::cout << 1;
  return true;
}

constexpr int init() pre(check())
{
   static_assert(std::is_constant_evaluated());
   return 1;
}

constexpr int v = init();
