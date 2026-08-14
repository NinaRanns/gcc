// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Same profile-name as the export, but different arguments: must fail.

import Req3 [[profiles::require(acme::type(fortify: 3))]]; // { dg-error "does not enforce profile .* with the required arguments" }

int use () { return h (); }
