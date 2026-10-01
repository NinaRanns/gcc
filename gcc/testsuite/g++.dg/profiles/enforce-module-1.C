// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi M }

// profiles::enforce on the primary interface export module-declaration
// is recorded in the module's exported-profiles set (CMI-bound content).

export module M [[profiles::enforce(std::init, acme::type)]];

export int f ();
int f () { return 0; }

// { dg-final { scan-lang-dump {Module M exports 2 profile-enforcements:} module } }
// { dg-final { scan-lang-dump {enforce std::init} module } }
// { dg-final { scan-lang-dump {enforce acme::type} module } }
