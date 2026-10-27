// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// As _b, with the namespace spelled profiles.

import Kai.Utils [[profiles::require(acme::ranges)]];	// { dg-error "does not enforce required profile 'acme::ranges'" }
