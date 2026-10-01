// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Req2 }

export module Req2 [[profiles::enforce(std::init)]];

export int g ();
int g () { return 0; }
