// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi Args }

// Argument clauses are part of the exported enforcement designators.

export module Args [[profiles::enforce(acme::hardened(fortify: 3, sanitize: thread))]];

export void g ();
void g () {}

// { dg-final { scan-lang-dump {Module Args exports 1 profile-enforcement:} module } }
// { dg-final { scan-lang-dump {enforce acme::hardened\(fortify: 3, sanitize: thread\)} module } }
