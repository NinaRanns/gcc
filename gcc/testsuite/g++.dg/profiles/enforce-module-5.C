// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }

// Freestanding profiles::enforce after the named module-declaration is
// ill-formed.  It must not appear in the exported-profiles set either.

export module Local;
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not a" }

export int m ();
int m () { return 3; }

// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
