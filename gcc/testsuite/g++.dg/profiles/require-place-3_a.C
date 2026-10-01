// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi PlaceOk }

// Interface that exports the profile-enforcements a valid require needs.

export module PlaceOk [[profiles::enforce(std::init, acme::type(fortify: 2))]];

export int place_ok ();
int place_ok () { return 0; }
