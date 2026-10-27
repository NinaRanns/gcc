// { dg-do compile { target c++26 } }
// { dg-additional-options "-fprofiles -fcontracts -fdump-lang-profiles" }

// Position audit, block scope: [stmt.pre] and the productions it uses
// (checked against the draft at cplusplus/draft d4d6f381, 2026-10-25)
// (labels, init-statement, condition, for-range-declaration,
// exception-declaration, block-declarations).
//
// Purpose: show that an attribute-specifier-seq is ACCEPTED at every such
// position, and pin what currently happens to a profiles attribute there.
// The fates today:
//   W  statement attribute: the statement is wrapped in a PROFILED_STMT,
//      checked below in -fdump-lang-profiles (what the statement added)
//   K  attribute on a declared entity or label: kept with it, in its
//      DECL_ATTRIBUTES, checked below in -fdump-lang-profiles
//   C  contract_assert's own attribute slot (after the keyword): dropped by
//      the contracts parser ("attributes are ignored on 'contract_assert'")
// A FAIL here is the signal to update these expectations, not to prune.
//
// No dg-prune-output: every diagnostic is expected explicitly.
//
// contract_assert comes FIRST on purpose: a for/while loop whose body is exactly
// `{ continue; }`, followed in the same function by contract_assert, ICEs trunk
// in gimplify_expr with or without attributes (bug-notes/
// contract-assert-after-continue-loop-ice.md).  Not this test's subject.

#include <coroutine>
#include <initializer_list>

int f ();
struct R { int *begin (); int *end (); };
struct P { int a, b; explicit operator bool () const { return a; } };
P parr[1];
struct T {
  struct promise_type {
    T get_return_object () { return {}; }
    std::suspend_never initial_suspend () { return {}; }
    std::suspend_never final_suspend () noexcept { return {}; }
    void return_void () {}
    void unhandled_exception () {}
  };
};

T
co ()
{
  [[profiles::suppress(acme::type)]] co_return;		// W jump: co_return
}

void
g (int c, R r)
{
  [[profiles::suppress(acme::type)]] contract_assert (c);	// W assertion-statement
  contract_assert [[profiles::suppress(acme::type)]] (c);	// { dg-warning "attributes are ignored on 'contract_assert'" } C assertion-statement, after the keyword
  [[profiles::suppress(acme::type)]] f ();		// W expression-statement
  [[profiles::suppress(acme::type)]] ;			// W null statement
  [[profiles::suppress(acme::type)]] { }		// W compound-statement
  [[profiles::suppress(acme::type)]] if (c) { }		// W if
  [[profiles::suppress(acme::type)]] if constexpr (true) { }	// W if constexpr
  [[profiles::suppress(acme::type)]] if consteval { } else { }	// W if consteval
  [[profiles::suppress(acme::type)]] if !consteval { }	// W if !consteval
  [[profiles::suppress(acme::type)]] switch (c) { }	// W switch
  [[profiles::suppress(acme::type)]] while (c) { break; }	// W while
  [[profiles::suppress(acme::type)]] do { } while (0);	// W do
  [[profiles::suppress(acme::type)]] for (;;) { break; }	// W for
  [[profiles::suppress(acme::type)]] for (int x : r) { }	// W range-for
  [[profiles::suppress(acme::type)]] template for (int x : {1, 2}) { }	// W expansion-statement
  for (;;) { [[profiles::suppress(acme::type)]] break; }	// W break
  for (;;) { [[profiles::suppress(acme::type)]] continue; }	// W continue
  [[profiles::suppress(acme::type)]] goto l1;		// W goto
 l1:;
  [[profiles::suppress(acme::type)]] try { } catch (...) { }	// W try-block
  if (c) [[profiles::suppress(acme::type)]] f ();	// W substatement
  if (c) { } else [[profiles::suppress(acme::type)]] f ();	// W else substatement
 l3: [[profiles::suppress(acme::type)]] f ();		// W statement after a label
  [] { [[profiles::suppress(acme::type)]] f (); } ();	// W statement in a lambda body
  [[profiles::suppress(acme::type)]] [[profiles::suppress(acme::type)]] f ();	// W two attribute-specifiers

  // Declarations and labels.
  [[profiles::suppress(acme::type)]] int d1 = 0;	// K declaration-statement
  [[profiles::suppress(acme::type)]] struct S { } s;	// K simple-declaration with class-specifier
  using U [[profiles::suppress(acme::type)]] = int;	// K alias-declaration
  auto [a1 [[profiles::suppress(acme::type)]], b1] = P {};	// K sb-identifier (C++26)
  [[profiles::suppress(acme::type)]] auto [a2, b2] = P {};	// K structured-binding declaration
  [[profiles::suppress(acme::type)]] l2: ;		// K label: identifier
  switch (c) { [[profiles::suppress(acme::type)]] case 1: ; }	// K label: case
  switch (c) { [[profiles::suppress(acme::type)]] default: ; }	// K label: default
  if ([[profiles::suppress(acme::type)]] int x = 1; x) { }	// K init-statement
  if ([[profiles::suppress(acme::type)]] int x = f ()) { }	// K condition (if)
  while ([[profiles::suppress(acme::type)]] int x = f ()) { break; }	// K condition (while)
  switch ([[profiles::suppress(acme::type)]] int x = f ()) { }	// K condition (switch)
  for ([[profiles::suppress(acme::type)]] int i = 0; i < 1; ++i) { }	// K for init-statement
  if ([[profiles::suppress(acme::type)]] auto [a3, b3] = P {}) { }	// K condition: structured-binding-declaration
  for ([[profiles::suppress(acme::type)]] auto [a4, b4] : parr) { }	// K for-range: structured-binding-declaration
  if (using T2 [[profiles::suppress(acme::type)]] = int; c) { }	// K init-statement: alias-declaration
  auto lt = [] <class U> [[profiles::suppress(acme::type)]] (U) { };	// K lambda, after template-parameter-list
  for ([[profiles::suppress(acme::type)]] int x : r) { }	// K for-range-declaration
  try { } catch ([[profiles::suppress(acme::type)]] int e) { }	// K exception-declaration

  // Block-declarations that handle their own attributes.
  [[profiles::suppress(acme::type)]] using namespace std;	// { dg-warning "'suppress' attribute directive ignored" } using-directive (message drops the namespace: name-lookup.cc)
  [[profiles::suppress(acme::type)]] asm ("");		// { dg-warning "attributes ignored on 'asm' declaration" } asm-declaration

  // Not a position: static_assert-declaration has no attribute-specifier-seq.
  [[profiles::suppress(acme::type)]] static_assert (true);	// { dg-error "expected primary-expression" }
}

// What each W position wrapped.
// { dg-final { scan-lang-dump {suppress acme::type at line 47: wraps cleanup_point_expr, lines 47-47\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 53: wraps assertion_stmt, lines 53-53\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 55: wraps cleanup_point_expr, lines 55-55\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 56: wraps nothing\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 57: wraps nothing\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 58: wraps if_stmt, lines 58-58\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 59: wraps if_stmt, lines 59-59\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 60: wraps if_stmt, lines 60-60\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 61: wraps if_stmt, lines 61-61\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 62: wraps switch_stmt, lines 62-62\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 63: wraps while_stmt, lines 63-63\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 64: wraps do_stmt, lines 64-64\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 65: wraps for_stmt, lines 65-65\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 66: wraps bind_expr, lines 66-66\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 67: wraps bind_expr, lines 67-67\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 68: wraps break_stmt, lines 68-68\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 69: wraps continue_stmt, lines 69-69\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 70: wraps predict_expr goto_expr, lines 70-70\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 72: wraps try_block, lines 72-72\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 73: wraps cleanup_point_expr, lines 73-73\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 74: wraps cleanup_point_expr, lines 74-74\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 75: wraps cleanup_point_expr, lines 75-75\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 76: wraps cleanup_point_expr, lines 76-76\n} profiles } }
// { dg-final { scan-lang-dump-times {suppress acme::type at line 77: wraps cleanup_point_expr, lines 77-77\n} 2 profiles } }
// { dg-final { scan-lang-dump {suppress acme::type at line 105: wraps nothing\n} profiles } }

// What each K position kept, and on which entity.
// { dg-final { scan-lang-dump {suppress acme::type on var_decl d1 at line 80\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl s at line 81\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on type_decl U at line 82\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl a1 at line 83\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl <anonymous> at line 84\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on label_decl l2 at line 85\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on label_decl <anonymous> at line 86\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on label_decl <anonymous> at line 87\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl x at line 88\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl x at line 89\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl x at line 90\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl x at line 91\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl i at line 92\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl <anonymous> at line 93\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl <anonymous> at line 94\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on type_decl T2 at line 95\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl operator\(\) at line 96\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl x at line 97\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl e at line 98\n} profiles } }
