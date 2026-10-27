// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Review item [identity.require.args]: repetition ([decl.profile.enforce]/3)
// compares token sequences, so the same named arguments in another order are
// a different enforcement, whatever the profile takes them to mean.

[[profiles::enforce(Vendor::Ruleset(rule: "CoreCheck", fortify: 3))]];
[[profiles::enforce(Vendor::Ruleset(fortify: 3, rule: "CoreCheck"))]]; // { dg-error "already loaded with different arguments" }
