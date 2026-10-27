// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi DF:detail }

// An implementation partition.
module DF:detail [[profiles::enforce(p4)]];
int dfd ();
// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
