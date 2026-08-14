// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Require with matching name and argument spelling: must succeed.

import Req4 [[profiles::require(acme::type(fortify: 2))]];

int use () { return k (); }
