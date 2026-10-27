// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// TODO: remove when suppressions are recorded
// { dg-prune-output "warning: attribute ignored" }

// Review item [markup.empty-decl]: P3589R3 adds an optional
// attribute-specifier-seq to empty-declaration, which makes
// [[profiles::enforce(P)]]; ambiguous with the existing attribute-declaration,
// and it does not say whether an attribute-declaration counts as a
// "non-empty-declaration" for [decl.profile.enforce]/1.  The prototype parses
// the form as an attribute-declaration, and no attribute-declaration ends the
// region in which profiles::enforce may appear, whatever its attributes.

[[gnu::unused]];
[[profiles::enforce(e1)]];
[[profiles::suppress(acme::type)]];
[[profiles::enforce(e2)]];
[[profiles::exempt(acme::type, angle_header: "x.h")]];
[[profiles::enforce(e3)]];
;
[[profiles::enforce(e4)]];

// Any other declaration ends it.
static_assert (true, "");
[[profiles::enforce(e5)]];	// { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
