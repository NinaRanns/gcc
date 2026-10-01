// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodule-header -fprofiles" }
// { dg-module-cmi {} }

// A header unit has no module-declaration: what it advertises is the
// profile-enforcements of its own attribute-declarations
// ([decl.profile.require]/2).

[[profiles::enforce(lib::hardened, acme::type(fortify: 2))]];

inline int hu () { return 1; }
