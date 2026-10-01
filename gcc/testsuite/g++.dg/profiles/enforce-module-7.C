// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi Rep }

// A repeated profile-enforcement has no effect ([decl.profile.enforce]/3), so
// a profile repeated on the module-declaration is exported once.

export module Rep [[profiles::enforce(P, Q)]] [[profiles::enforce(P)]];
export int rep ();

// { dg-final { scan-lang-dump {Module Rep exports 2 profile-enforcements:} module } }
