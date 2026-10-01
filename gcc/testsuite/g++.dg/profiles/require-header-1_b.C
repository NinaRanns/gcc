// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// A require on the import of a header unit is checked against the header's
// enforcements, arguments included.

import "require-header-1_a.H" [[profiles::require(lib::hardened)]];
import "require-header-1_a.H" [[profiles::require(acme::type(fortify: 2))]];
import "require-header-1_a.H" [[profiles::require(acme::type(fortify: 3))]]; // { dg-error "does not enforce profile 'acme::type' with the required arguments" }
import "require-header-1_a.H" [[profiles::require(std::init)]]; // { dg-error "does not enforce required profile 'std::init'" }

int use () { return hu (); }
