// { dg-do compile { target c++26 } }
// { dg-additional-options "-fprofiles -fcontracts" }

// Position audit, block scope: [stmt.pre] and the productions it uses
// (checked against the draft at cplusplus/draft d4d6f381, 2026-10-25)
// (labels, init-statement, condition, for-range-declaration,
// exception-declaration, block-declarations).
//
// Purpose: show that an attribute-specifier-seq is ACCEPTED at every such
// position, and pin what currently happens to a profiles attribute there.
// There are two fates today:
//   S  statement attribute: parsed, then discarded by cp_parser_statement
//      ("attributes at the beginning of statement are ignored")
//   D  attribute on a declared entity or label: reaches decl_attributes,
//      which does not know the profiles namespace ("... directive ignored")
//   C  contract_assert's own attribute slot (after the keyword): dropped by
//      the contracts parser ("attributes are ignored on 'contract_assert'")
// Nothing is recorded yet (wip-review.md [b.layer-c], [b.dispatch]).  When
// suppress is recorded, these expectations are meant to change: a FAIL here
// is the signal to update them, not to prune.
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
  [[profiles::suppress(acme::type)]] co_return;		// { dg-warning "attributes at the beginning of statement are ignored" } S jump: co_return
}

void
g (int c, R r)
{
  [[profiles::suppress(acme::type)]] contract_assert (c);	// { dg-warning "attributes at the beginning of statement are ignored" } S assertion-statement
  contract_assert [[profiles::suppress(acme::type)]] (c);	// { dg-warning "attributes are ignored on 'contract_assert'" } C assertion-statement, after the keyword
  [[profiles::suppress(acme::type)]] f ();		// { dg-warning "attributes at the beginning of statement are ignored" } S expression-statement
  [[profiles::suppress(acme::type)]] ;			// { dg-warning "attributes at the beginning of statement are ignored" } S null statement
  [[profiles::suppress(acme::type)]] { }		// { dg-warning "attributes at the beginning of statement are ignored" } S compound-statement
  [[profiles::suppress(acme::type)]] if (c) { }		// { dg-warning "attributes at the beginning of statement are ignored" } S if
  [[profiles::suppress(acme::type)]] if constexpr (true) { }	// { dg-warning "attributes at the beginning of statement are ignored" } S if constexpr
  [[profiles::suppress(acme::type)]] if consteval { } else { }	// { dg-warning "attributes at the beginning of statement are ignored" } S if consteval
  [[profiles::suppress(acme::type)]] if !consteval { }	// { dg-warning "attributes at the beginning of statement are ignored" } S if !consteval
  [[profiles::suppress(acme::type)]] switch (c) { }	// { dg-warning "attributes at the beginning of statement are ignored" } S switch
  [[profiles::suppress(acme::type)]] while (c) { break; }	// { dg-warning "attributes at the beginning of statement are ignored" } S while
  [[profiles::suppress(acme::type)]] do { } while (0);	// { dg-warning "attributes at the beginning of statement are ignored" } S do
  [[profiles::suppress(acme::type)]] for (;;) { break; }	// { dg-warning "attributes at the beginning of statement are ignored" } S for
  [[profiles::suppress(acme::type)]] for (int x : r) { }	// { dg-warning "attributes at the beginning of statement are ignored" } S range-for
  [[profiles::suppress(acme::type)]] template for (int x : {1, 2}) { }	// { dg-warning "attributes at the beginning of statement are ignored" } S expansion-statement
  for (;;) { [[profiles::suppress(acme::type)]] break; }	// { dg-warning "attributes at the beginning of statement are ignored" } S break
  for (;;) { [[profiles::suppress(acme::type)]] continue; }	// { dg-warning "attributes at the beginning of statement are ignored" } S continue
  [[profiles::suppress(acme::type)]] goto l1;		// { dg-warning "attributes at the beginning of statement are ignored" } S goto
 l1:;
  [[profiles::suppress(acme::type)]] try { } catch (...) { }	// { dg-warning "attributes at the beginning of statement are ignored" } S try-block
  if (c) [[profiles::suppress(acme::type)]] f ();	// { dg-warning "attributes at the beginning of statement are ignored" } S substatement
  if (c) { } else [[profiles::suppress(acme::type)]] f ();	// { dg-warning "attributes at the beginning of statement are ignored" } S else substatement
 l3: [[profiles::suppress(acme::type)]] f ();		// { dg-warning "attributes at the beginning of statement are ignored" } S statement after a label
  [] { [[profiles::suppress(acme::type)]] f (); } ();	// { dg-warning "attributes at the beginning of statement are ignored" } S statement in a lambda body
  [[profiles::suppress(acme::type)]] [[profiles::suppress(acme::type)]] f ();	// { dg-warning "attributes at the beginning of statement are ignored" } S two attribute-specifiers

  // Declarations and labels.
  [[profiles::suppress(acme::type)]] int d1 = 0;	// { dg-warning "scoped attribute directive ignored" } D declaration-statement
  [[profiles::suppress(acme::type)]] struct S { } s;	// { dg-warning "scoped attribute directive ignored" } D simple-declaration with class-specifier
  using U [[profiles::suppress(acme::type)]] = int;	// { dg-warning "scoped attribute directive ignored" } D alias-declaration
  auto [a1 [[profiles::suppress(acme::type)]], b1] = P {};	// { dg-warning "scoped attribute directive ignored" } D sb-identifier (C++26)
  [[profiles::suppress(acme::type)]] auto [a2, b2] = P {};	// { dg-warning "scoped attribute directive ignored" } D structured-binding declaration
  [[profiles::suppress(acme::type)]] l2: ;		// { dg-warning "scoped attribute directive ignored" } D label: identifier
  switch (c) { [[profiles::suppress(acme::type)]] case 1: ; }	// { dg-warning "scoped attribute directive ignored" } D label: case
  switch (c) { [[profiles::suppress(acme::type)]] default: ; }	// { dg-warning "scoped attribute directive ignored" } D label: default
  if ([[profiles::suppress(acme::type)]] int x = 1; x) { }	// { dg-warning "scoped attribute directive ignored" } D init-statement
  if ([[profiles::suppress(acme::type)]] int x = f ()) { }	// { dg-warning "scoped attribute directive ignored" } D condition (if)
  while ([[profiles::suppress(acme::type)]] int x = f ()) { break; }	// { dg-warning "scoped attribute directive ignored" } D condition (while)
  switch ([[profiles::suppress(acme::type)]] int x = f ()) { }	// { dg-warning "scoped attribute directive ignored" } D condition (switch)
  for ([[profiles::suppress(acme::type)]] int i = 0; i < 1; ++i) { }	// { dg-warning "scoped attribute directive ignored" } D for init-statement
  if ([[profiles::suppress(acme::type)]] auto [a3, b3] = P {}) { }	// { dg-warning "scoped attribute directive ignored" } D condition: structured-binding-declaration
  for ([[profiles::suppress(acme::type)]] auto [a4, b4] : parr) { }	// { dg-warning "scoped attribute directive ignored" } D for-range: structured-binding-declaration
  if (using T2 [[profiles::suppress(acme::type)]] = int; c) { }	// { dg-warning "scoped attribute directive ignored" } D init-statement: alias-declaration
  auto lt = [] <class U> [[profiles::suppress(acme::type)]] (U) { };	// { dg-warning "scoped attribute directive ignored" } D lambda, after template-parameter-list
  for ([[profiles::suppress(acme::type)]] int x : r) { }	// { dg-warning "scoped attribute directive ignored" } D for-range-declaration
  try { } catch ([[profiles::suppress(acme::type)]] int e) { }	// { dg-warning "scoped attribute directive ignored" } D exception-declaration

  // Block-declarations that handle their own attributes.
  [[profiles::suppress(acme::type)]] using namespace std;	// { dg-warning "'suppress' attribute directive ignored" } using-directive (message drops the namespace: name-lookup.cc)
  [[profiles::suppress(acme::type)]] asm ("");		// { dg-warning "attributes ignored on 'asm' declaration" } asm-declaration

  // Not a position: static_assert-declaration has no attribute-specifier-seq.
  [[profiles::suppress(acme::type)]] static_assert (true);	// { dg-error "expected primary-expression" }
  // { dg-warning "attributes at the beginning of statement are ignored" "" { target *-*-* } .-1 }
}
