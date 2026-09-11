// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// TODO: remove warning when profiles attributes location is verified
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

// Empty-declarations (including a bare semicolon) do not close the
// profiles::enforce window.  A non-empty declaration does.

[[profiles::enforce(std::init)]];
[[profiles::enforce(std::init)]];

;
[[profiles::enforce(std::init)]];

int before;

[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not a" }

namespace N { int y; }

[[profiles::enforce(std::init)]]; // { dg-error "must precede any declaration that is not a" }
