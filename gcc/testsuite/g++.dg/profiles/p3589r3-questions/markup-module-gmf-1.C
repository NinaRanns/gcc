// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi !G1 }

// Review item [markup.module.gmf]: as written, nothing can appear in the
// global module fragment.  The prototype accepts a profiles-only
// attribute-declaration there ([disp.module.gmf]).

// An attribute-declaration in the global module fragment that has no
// profiles content keeps trunk's pedwarn, with -fprofiles too.

module;
[[gnu::unused]];	// { dg-error "global module fragment contents must be from preprocessor inclusion" }
// { dg-warning "attribute ignored" "" { target *-*-* } .-1 }
export module G1;
