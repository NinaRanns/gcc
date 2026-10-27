// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Req6 }

export module Req6 [[profiles::enforce(Vendor::Ruleset(rule: "CoreCheck", fortify: 3))]];
export int r6 ();
