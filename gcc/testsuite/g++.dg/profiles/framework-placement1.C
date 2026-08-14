// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// TODO: remove warning when profiles attributes location is verified
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

// A profile-enforcement attribute shall appear only as an
// attribute-declaration, and that attribute-declaration shall precede any
// declaration that is not an attribute-declaration or empty-declaration in
// the translation unit.

// Valid: attribute-declarations that precede any declaration that is not an
// attribute-declaration or empty-declaration.  Exact token-sequence
// repetitions are allowed.  Empty-declarations do not block an enforcement.
[[profiles::enforce(std::init)]];
;
[[profiles::enforce(std::init)]];

// Invalid: appertains to a declaration, not an attribute-declaration.
[[profiles::enforce(std::init)]] int z; // { dg-error "may only appear as an attribute-declaration" }

int before;

// Invalid: does not precede the declaration 'before'.
[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not an attribute-declaration or empty-declaration" }

namespace N { int y; }

// Invalid: a namespace-definition is a blocking declaration too.
[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not an attribute-declaration or empty-declaration" }

void f()
{
  // Invalid: an enforcement may not appear at block scope.
  [[profiles::enforce(std::init)]]; // { dg-error "may only appear at global scope" }
}

struct S
{
  // Invalid: an enforcement may not appear at class scope.
  [[profiles::enforce(std::init)]]; // { dg-error "may only appear at global scope" }
};
