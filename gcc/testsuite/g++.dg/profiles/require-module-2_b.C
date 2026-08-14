// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Require a profile the interface does not export.

import Req2 [[profiles::require(acme::type)]]; // { dg-error "does not enforce required profile" }

int use () { return g (); }
