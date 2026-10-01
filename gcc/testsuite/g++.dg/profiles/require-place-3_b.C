// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Valid placements: profiles::require on import-declarations, matching
// the interface's exported enforcements.

import PlaceOk [[profiles::require(std::init)]];
import PlaceOk [[profiles::require(acme::type(fortify: 2))]];
import PlaceOk [[profiles::require(std::init, acme::type(fortify: 2))]];

int use () { return place_ok (); }
