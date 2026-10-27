// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [interact.module.advertised]: [decl.profile.require]/2 looks
// only at what the module enforces, so the require is met although the whole
// interface suppresses the profile.  (Today the prototype does not yet record
// suppressions, but the paper requires this outcome anyway.)

import Adv [[profiles::require(acme::type)]];
int use () { return adv_f (); }
