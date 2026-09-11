// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles" }
[[profiles::enforce(std::init)]];              // OK: no declaration yet
[[profiles::enforce(std::init)]] int z;        // { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
void f ()
{
  [[profiles::enforce(std::init)]];            // { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
}
struct S
{
  [[profiles::enforce(std::init)]];            // { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
  // { dg-error "declaration does not declare anything" "" { target *-*-* } .-1 }
};

// A profiles::enforce that appertains to a declaration is rejected even while
// attribute-declarations may still carry one (line 4); at block and class
// scope a declaration has always preceded it.
