// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// TODO: remove warning when profiles attributes location is verified
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

// Enforce-only attribute-declarations may appear until the first
// import or other non-enforcement declaration.  A bare empty-declaration
// ends that window (same as ending a module import region).

[[profiles::enforce(std::init)]];
[[profiles::enforce(std::init)]];

;
[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not a" }

int before;

[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not a" }

namespace N { int y; }

[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not a" }
