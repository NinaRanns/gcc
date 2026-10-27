// { dg-do compile { target c++20 } }
// { dg-additional-options "-fprofiles -Wall -Wreturn-type -fdump-lang-profiles" }

// A profiles::suppress on a statement is kept in a PROFILED_STMT, which
// wraps what the statement added.  The wrapped code is unchanged: it is
// instantiated, constant-evaluated and compiled as before, and it is not
// reported as an ignored attribute.

[[profiles::enforce(P)]];

int
f (int x)
{
  [[profiles::suppress(P)]] if (x) x++;
  [[profiles::suppress(P)]] while (x > 100) x--;
  [[profiles::suppress(P)]] return x;	// no "control reaches end"
}

template <typename T>
T
t (T x)
{
  [[profiles::suppress(P)]] for (int i = 0; i < 2; ++i) x += i;
  return x;
}
int ti = t (1);

constexpr int
c (int x)
{
  [[profiles::suppress(P)]] if (x > 0) return x * 2;
  return 0;
}
static_assert (c (4) == 8);

consteval int
ce (int x)
{
  [[profiles::suppress(P)]] for (int i = 0; i < 3; ++i) x += i;
  return x;
}
static_assert (ce (1) == 4);

auto l = [] (int x) { [[profiles::suppress(P)]] return x + 1; };
int li = l (1);

// { dg-final { scan-lang-dump {suppress P at line 14: wraps if_stmt, lines 14-14\n} profiles } }
// { dg-final { scan-lang-dump {suppress P at line 15: wraps while_stmt, lines 15-15\n} profiles } }
// { dg-final { scan-lang-dump {suppress P at line 16: wraps return_expr, lines 16-16\n} profiles } }
// { dg-final { scan-lang-dump {suppress P at line 23: wraps for_stmt, lines 23-23\n} profiles } }
// { dg-final { scan-lang-dump {suppress P at line 31: wraps if_stmt, lines 31-31\n} profiles } }
// { dg-final { scan-lang-dump {suppress P at line 39: wraps bind_expr, lines 39-39\n} profiles } }
// { dg-final { scan-lang-dump {suppress P at line 44: wraps return_expr, lines 44-44\n} profiles } }
