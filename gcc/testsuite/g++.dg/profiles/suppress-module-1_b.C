// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// The importer uses the inline function, instantiates the template and
// constant-evaluates the constexpr function.

import SupMod;
int use () { return inl (1) + tpl (2) + tpl (2.0); }
static_assert (cx (2) == 6);
