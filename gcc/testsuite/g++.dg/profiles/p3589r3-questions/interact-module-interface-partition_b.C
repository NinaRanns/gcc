// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Part }

// The primary interface enforces acme::type and re-exports the partition.
// Under [decl.profile.enforce]/4 its dominion does not reach the partition,
// which is a separate interface unit.
export module Part [[profiles::enforce(acme::type)]];
export import :iface;
export int prim_f ();
