// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles -fdump-lang-module" }
// { dg-module-cmi DF }

// Review item [markup.module.decl-forms], part of [dom.enforce.impl-unit]:
// every form of module-declaration takes an enforce, but P3589R3 gives a
// meaning only to the primary module interface unit's.  The prototype accepts
// all four forms (_a to _d) and exports only this one.
export module DF [[profiles::enforce(p1)]];
export int df ();
// { dg-final { scan-lang-dump {exports 1 profile-enforcement} module } }
