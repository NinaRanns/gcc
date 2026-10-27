// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi DF:part }

// An interface partition.
export module DF:part [[profiles::enforce(p2)]];
export int dfp ();
// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
