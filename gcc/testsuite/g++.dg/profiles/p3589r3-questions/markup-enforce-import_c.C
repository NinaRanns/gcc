// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi !Imp1 }

// As _b, in a module unit.
export module Imp1;
import Bob;
[[profiles::enforce(std::init)]];	// { dg-error "may only appear on a module-declaration" }
