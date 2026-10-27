// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi Adv }
// TODO: remove when suppressions are recorded
// { dg-prune-output "warning: attribute ignored" }
// { dg-prune-output "scoped attribute directive ignored" }

// An interface that advertises acme::type and suppresses it throughout.
export module Adv [[profiles::enforce(acme::type)]];
[[profiles::suppress(acme::type, justification: "all of it")]];
export [[profiles::suppress(acme::type)]] int adv_f ();
