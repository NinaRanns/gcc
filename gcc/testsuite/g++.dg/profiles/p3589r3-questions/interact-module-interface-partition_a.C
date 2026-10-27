// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Part:iface }

// An interface partition that enforces nothing.
export module Part:iface;
export int part_f ();
