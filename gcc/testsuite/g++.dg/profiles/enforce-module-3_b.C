// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }

// Implementation unit: enforce on a non-exporting module-declaration must
// not publish exported profiles into the CMI-bound set.

module N [[profiles::enforce(std::init)]];

int h ();
int h () { return 1; }

// { dg-final { scan-lang-dump-not {exports .* profile-enforcement} module } }
