// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -fdump-lang-profiles" }

// Review item [dom.suppress.decl]: for a declaration, [decl.profile.suppress]/3
// ends the dominion at the declaration "to which the attribute appertains",
// but an attribute in a declaration appertains to entities, not to the
// declaration.  The prototype follows appertainment ([disp.suppress.decl]):
// the suppression is kept with each entity it appertains to.  At #1 that is
// `a' alone, although under /3 the tokens of `b' follow the attribute; at #2
// it is both `c' and `d'.

[[profiles::enforce(P)]];
int f ();
void
g ()
{
  int a [[profiles::suppress(P)]] = f (), b = a;	// #1
  [[profiles::suppress(P)]] int c = f (), d = c;	// #2
  (void) b; (void) d;
}

// { dg-final { scan-lang-dump {suppress P on var_decl a at line 17\n} profiles } }
// { dg-final { scan-lang-dump-not {on var_decl b } profiles } }
// { dg-final { scan-lang-dump {suppress P on var_decl c at line 18\n} profiles } }
// { dg-final { scan-lang-dump {suppress P on var_decl d at line 18\n} profiles } }
