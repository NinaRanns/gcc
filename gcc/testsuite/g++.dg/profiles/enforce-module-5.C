// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi Local }

// Empty-declaration enforcements in the interface are TU-local only;
// they must not appear in the module exported-profiles set.

export module Local;
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

[[profiles::enforce(std::init)]];

export int m ();
int m () { return 3; }

// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
