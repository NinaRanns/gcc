// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -fdump-lang-profiles" }

// Review item [dom.suppress.extent]: a suppression covers everything nested
// in the statement it is on, with no bound: here a whole loop, its body, and
// a lambda defined in it, whose body belongs to another function.

[[profiles::enforce(acme::type)]];

int
f (int n)
{
  int s = 0;
  [[profiles::suppress(acme::type)]] for (int i = 0; i < n; ++i)
    {
      auto add = [&] (int v) { s += v; };
      add (i);
    }
  return s;
}

// { dg-final { scan-lang-dump {suppress acme::type at line 14: wraps bind_expr, lines 14-18\n} profiles } }
