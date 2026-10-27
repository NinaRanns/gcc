// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -fdump-lang-profiles" }

// Review item [dom.suppress.label]: a labeled-statement has no attribute
// position of its own; an attribute before a label appertains to the label
// ([stmt.label]).  So the suppress before the case label is kept with the
// label, and the statement after the label is suppressed only by one of its
// own.

[[profiles::enforce(acme::type)]];

int
f (int c)
{
  switch (c)
    {
    [[profiles::suppress(acme::type)]] case 1:
      return 1;
    case 2:
      [[profiles::suppress(acme::type)]] return 2;
    }
  return 0;
}

// { dg-final { scan-lang-dump {suppress acme::type on label_decl <anonymous> at line 17\n} profiles } }
// { dg-final { scan-lang-dump-not {at line 17:} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 20: wraps return_expr, lines 20-20\n} profiles } }
