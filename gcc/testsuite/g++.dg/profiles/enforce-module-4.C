// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi {P:Part} }

// Interface partitions do not contribute to the primary interface's
// exported-profiles set (v1).

export module P:Part [[profiles::enforce(std::init)]];

export int k ();
int k () { return 2; }

// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
