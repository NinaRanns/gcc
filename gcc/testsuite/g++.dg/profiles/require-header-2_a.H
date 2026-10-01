// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodule-header -fprofiles" }
// { dg-module-cmi {} }

// A header unit that enforces nothing.

inline int hu2 () { return 2; }
