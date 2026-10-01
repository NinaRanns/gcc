// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi Multi }

// Several profiles::enforce attributes on the export module-declaration
// combine into one exported-profiles TREE_LIST.

export module Multi [[profiles::enforce(std::init)]] [[profiles::enforce(acme::type(fortify: 2))]];

export void n ();
void n () {}

// { dg-final { scan-lang-dump {Module Multi exports 2 profile-enforcements:} module } }
// { dg-final { scan-lang-dump {enforce std::init} module } }
// { dg-final { scan-lang-dump {enforce acme::type\(fortify: 2\)} module } }
