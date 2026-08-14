// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Req1 }

// Interface exporting profile-enforcements for a matching import require.

export module Req1 [[profiles::enforce(std::init, acme::type)]];

export int f ();
int f () { return 0; }
