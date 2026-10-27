// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [ed.colon]: the second example of [decl.profile.require]/2
// writes acme:type, with a single colon, in a module-declaration it means to
// be valid.  As written, it is a syntax error.

export module Kai.Utils [[profiles::enforce(lib::hardened, acme:type)]]; // { dg-error "expected '\\)' before ':' token" }
