// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Nothing is advertised, so any require on it fails.

import "require-header-2_a.H" [[profiles::require(lib::hardened)]]; // { dg-error "does not enforce required profile 'lib::hardened'" }

int use () { return hu2 (); }
