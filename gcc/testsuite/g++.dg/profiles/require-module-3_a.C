// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Req3 }

export module Req3 [[profiles::enforce(acme::type(fortify: 2))]];

export int h ();
int h () { return 0; }
