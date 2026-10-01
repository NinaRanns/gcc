// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Import requiring profiles that the interface exports: must succeed.

import Req1 [[profiles::require(std::init, acme::type)]];

int use () { return f (); }
