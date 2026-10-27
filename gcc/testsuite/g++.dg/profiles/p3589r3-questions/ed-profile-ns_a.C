// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Kai.Utils }

export module Kai.Utils [[profiles::enforce(lib::hardened, acme::type)]];
