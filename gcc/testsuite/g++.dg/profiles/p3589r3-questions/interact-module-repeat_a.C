// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Rp }

export module Rp [[profiles::enforce(p1(x: 1))]];
export int rp ();
