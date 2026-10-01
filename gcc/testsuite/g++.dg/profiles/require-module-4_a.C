// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Req4 }

export module Req4 [[profiles::enforce(acme::type(fortify: 2))]];

export int k ();
int k () { return 0; }
