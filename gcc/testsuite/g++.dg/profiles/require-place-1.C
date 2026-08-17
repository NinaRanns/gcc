// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

// profiles::require is only valid on an import-declaration.

[[profiles::require(std::init)]]; // { dg-error "may only appear on an import" }

[[profiles::require(std::init)]] int x; // { dg-error "may only appear on an import" }

void f ()
{
  [[profiles::require(acme::type)]] int y; // { dg-error "may only appear on an import" }
  [[profiles::require(acme::type)]]; // { dg-error "may only appear on an import" }
}
