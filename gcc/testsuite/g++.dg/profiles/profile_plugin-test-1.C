// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// { dg-warning "Callback to register profiles" "" { target *-*-* } 0 }
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

[[profiles::enforce(test::plugin)]]; // { dg-warning "profile 'test::plugin' enforced by plugin" }

// Unregistered names are still accepted by the framework today.
[[profiles::enforce(other::name)]];
