// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi !GmfDecl }

// [decl.profile.enforce]/1 counts every declaration in the translation unit,
// those in the global module fragment included, so after one there a
// free-standing profiles::enforce is ill-formed even right after the
// module-declaration.

module;
#include "enforce-module-8.h"
export module GmfDecl;

[[profiles::enforce(std::init)]]; // { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
