// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [interact.module.interface-partition]: the require is met,
// because the primary interface enforces acme::type, although part_f was
// declared outside any acme::type dominion.  The paper as written allows
// this; [disp.module.partitions] proposes that the primary diagnose it.

import Part [[profiles::require(acme::type)]];
int use () { return part_f () + prim_f (); }
