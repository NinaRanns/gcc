// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Nm }

export module Nm [[profiles::enforce(test::profile)]];
export int nm ();
