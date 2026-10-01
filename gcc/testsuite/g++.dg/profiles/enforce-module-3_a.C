// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi N }

// Primary interface with no profiles::enforce on the module-declaration.

export module N;

export int h ();
int h () { return 0; }
