// { dg-do compile { target c++11 } }
/* { dg-prune-output "require. attribute ignored" } */

/* Path coverage for profiles_plugin_require_attribute and what it calls.
   Each case is attached to a decl so the attribute handler runs.

   [[profiles::require(profile-designator-list)]]
   Same argument grammar as enforce.  */

/* --- profiles_extract_tokens --- */

/* Missing attribute-argument-clause.  */
[[profiles::require]] int e1; // { dg-error "" }

/* --- profiles_parse_designator_seq / profiles_parse_name --- */

/* Empty body: done before any designator.  */
[[profiles::require()]] int e2; // { dg-error "expected profile-designator" }

/* First token is not an identifier.  */
[[profiles::require(42)]] int e3; // { dg-error "expected profile-name" }
[[profiles::require(+)]] int e4; // { dg-error "expected profile-name" }
[[profiles::require(int)]] int e5; // { dg-error "expected profile-name" }

/* Trailing :: with no following identifier.  */
[[profiles::require(test::)]] int e6; // { dg-error "expected identifier after" }

/* --- success: profile-name only (designator without argument-list) --- */

[[profiles::require(test)]] int ok1;
[[profiles::require(test::profile)]] int ok2;
[[profiles::require(a::b::c)]] int ok3;

/* --- profile-designator with ( profile-argument-list ) --- */

/* Empty argument-list.  */
[[profiles::require(test())]] int e7; // { dg-error "expected profile-argument" }

/* Positional arguments.  */
[[profiles::require(test(x))]] int ok4;
[[profiles::require(test("why"))]] int ok5;
[[profiles::require(test(a, b, c))]] int ok6;

/* Named arguments.  */
[[profiles::require(test(fortify: 3))]] int ok7;
[[profiles::require(test(opts: (a, b)))]] int ok8;
[[profiles::require(test(rule: "r", level: 2))]] int ok9;

/* Nested name with arguments.  */
[[profiles::require(acme::hardened(fortify: 3, sanitize: thread))]] int ok10;

/* Bad arguments inside designator.  */
[[profiles::require(test(+))]] int e8; // { dg-error "expected a non-operator-non-punctuator-token" }
[[profiles::require(test(name:))]] int e9; // { dg-error "expected a non-comma-balanced-token" }
[[profiles::require(test(name: ,))]] int e10; // { dg-error "expected a non-comma-balanced-token" }
[[profiles::require(test(x,))]] int e11; // { dg-error "expected profile-argument" }

/* Missing ')' after argument-list.  */
[[profiles::require(test(x]] int e12; // { dg-error "" }

/* --- profiles_parse_designator_seq: several designators --- */

[[profiles::require(test, other)]] int ok11;
[[profiles::require(test::a, b::c, d)]] int ok12;
[[profiles::require(test(x), other)]] int ok13;
[[profiles::require(test, other(y))]] int ok14;
[[profiles::require(a(x), b(y), c)]] int ok15;

/* Trailing comma in designator-list.  */
[[profiles::require(test,)]] int e13; // { dg-error "expected profile-designator" }
[[profiles::require(test, other,)]] int e14; // { dg-error "expected profile-designator" }

/* Bad second designator.  */
[[profiles::require(test, +)]] int e15; // { dg-error "expected profile-name" }
[[profiles::require(test, other::)]] int e16; // { dg-error "expected identifier after" }
[[profiles::require(test, other(+))]] int e17; // { dg-error "expected a non-operator-non-punctuator-token" }

/* --- profiles_finish_args: leftover tokens after a designator --- */

[[profiles::require(test :)]] int e18; // { dg-error "unexpected token" }
[[profiles::require(test +)]] int e19; // { dg-error "unexpected token" }
[[profiles::require(test profile)]] int e20; // { dg-error "unexpected token" }
[[profiles::require(test(x) +)]] int e21; // { dg-error "unexpected token" }
