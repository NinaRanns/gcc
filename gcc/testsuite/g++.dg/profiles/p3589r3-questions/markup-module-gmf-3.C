// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi G3 }

// Review item [markup.module.gmf]: as written, nothing can appear in the
// global module fragment.  The prototype accepts a profiles-only
// attribute-declaration there ([disp.module.gmf]).

// An attribute-declaration made only of profiles attributes is accepted in
// the global module fragment, however the namespace is spelled.

module;
[[profiles::enforce(std::init)]];
[[using profiles: enforce(acme::type)]];
export module G3;
