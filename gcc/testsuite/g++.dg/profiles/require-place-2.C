// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi !PlaceBad }

// profiles::require must not appear on a module-declaration.

export module PlaceBad [[profiles::require(std::init)]]; // { dg-error "may only appear on an import" }

export int place_bad ();
int place_bad () { return 0; }
