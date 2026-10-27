// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [interact.module.repeat]: this implementation unit lies in the
// dominion of the primary's enforce ([decl.profile.enforce]/4) and enforces
// the same profile with other arguments.  The repetition rule (/3) is stated
// within one translation unit, and the prototype does not check it here.
module Rp [[profiles::enforce(p1(x: 2))]];
int rp () { return 0; }
