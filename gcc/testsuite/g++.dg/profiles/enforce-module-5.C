// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi Local }

// A module-declaration is not a declaration, so a free-standing
// profiles::enforce may follow it while nothing else precedes it
// ([decl.profile.enforce]/1).  It is local to this unit, so it is not
// exported.

export module Local;

[[profiles::enforce(std::init)]];

export int m ();
int m () { return 3; }

// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
