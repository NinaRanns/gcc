// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Req5 }

// The argument 0x3 is exported as spelled, not as its value.
export module Req5 [[profiles::enforce(acme(f: 0x3))]];
export int f5 ();
