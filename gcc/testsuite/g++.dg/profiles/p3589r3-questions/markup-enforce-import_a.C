// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Bob }

export module Bob;
export int bob ();
