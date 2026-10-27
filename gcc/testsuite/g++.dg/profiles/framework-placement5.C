// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -fmax-errors=1" }

// As framework-placement4.C, for profiles::exempt: one error, diagnosed once.

void
f ()
{
  [[profiles::exempt(acme::type, quote_header: "e.h")]] int y = 0; // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
}
