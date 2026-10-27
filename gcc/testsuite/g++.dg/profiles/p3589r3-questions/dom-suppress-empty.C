// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles -fdump-lang-profiles" }
// TODO: remove when suppressions at namespace scope are recorded
// { dg-prune-output "attribute ignored" }

// Review item [dom.suppress.empty]: a free-standing suppress needs no grammar
// change: at namespace scope it is an attribute-declaration, at block scope it
// appertains to a null statement.  Under [decl.profile.suppress]/3 its
// dominion is that construct's own tokens, so it suppresses nothing.  The
// natural block-scope spelling for re-enabling a profile, an enforce, is
// rejected by [decl.profile.enforce]/1.

[[profiles::enforce(acme::type)]];
[[profiles::suppress(acme::type, justification: "why")]];

void
f ()
{
  [[profiles::suppress(acme::type, justification: "why")]];
  [[profiles::enforce(acme::type)]];	// { dg-error "may only appear on a module-declaration" }
}

// The block-scope one is kept, wrapping nothing.
// { dg-final { scan-lang-dump {suppress acme::type at line [0-9]+: wraps nothing\n} profiles } }
