// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// An implementation unit.
module DF [[profiles::enforce(p3)]];
int df () { return 0; }
