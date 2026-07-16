// { dg-do compile { target c++11 } }
/* { dg-prune-output "enforce. attribute ignored" } */

/* Path coverage for profiles_plugin_enforce_attribute and what it calls.
   Each case is attached to a decl so the attribute handler runs.

   [[profiles::enforce(profile-designator-list)]]  */

/* --- profiles_extract_tokens --- */

/* Missing attribute-argument-clause.  */
[[profiles::enforce]] int e1; // { dg-error "" }

/* --- profiles_parse_designator_seq / profiles_parse_name --- */

/* Empty body: done before any designator.  */
[[profiles::enforce()]] int e2; // { dg-error "expected profile-designator" }

/* First token is not an identifier.  */
[[profiles::enforce(42)]] int e3; // { dg-error "expected profile-name" }
[[profiles::enforce(+)]] int e4; // { dg-error "expected profile-name" }
[[profiles::enforce(int)]] int e5; // { dg-error "expected profile-name" }

/* Trailing :: with no following identifier.  */
[[profiles::enforce(test::)]] int e6; // { dg-error "expected identifier after" }

/* --- success: profile-name only (designator without argument-list) --- */

[[profiles::enforce(test)]] int ok1;
[[profiles::enforce(test::profile)]] int ok2;
[[profiles::enforce(a::b::c)]] int ok3;

/* --- profile-designator with ( profile-argument-list ) --- */

/* Empty argument-list.  */
[[profiles::enforce(test())]] int e7; // { dg-error "expected profile-argument" }

/* Positional arguments.  */
[[profiles::enforce(test(x))]] int ok4;
[[profiles::enforce(test("why"))]] int ok5;
[[profiles::enforce(test(a, b, c))]] int ok6;

/* Named arguments.  */
[[profiles::enforce(test(fortify: 3))]] int ok7;
[[profiles::enforce(test(opts: (a, b)))]] int ok8;
[[profiles::enforce(test(rule: "r", level: 2))]] int ok9;

/* Nested name with arguments.  */
[[profiles::enforce(acme::hardened(fortify: 3, sanitize: thread))]] int ok10;

/* Bad arguments inside designator.  */
[[profiles::enforce(test(+))]] int e8; // { dg-error "expected a non-operator-non-punctuator-token" }
[[profiles::enforce(test(name:))]] int e9; // { dg-error "expected a non-comma-balanced-token" }
[[profiles::enforce(test(name: ,))]] int e10; // { dg-error "expected a non-comma-balanced-token" }
[[profiles::enforce(test(x,))]] int e11; // { dg-error "expected profile-argument" }

/* Missing ')' after argument-list.  */
[[profiles::enforce(test(x]] int e12; // { dg-error "" }

/* --- profiles_parse_designator_seq: several designators --- */

[[profiles::enforce(test, other)]] int ok11;
[[profiles::enforce(test::a, b::c, d)]] int ok12;
[[profiles::enforce(test(x), other)]] int ok13;
[[profiles::enforce(test, other(y))]] int ok14;
[[profiles::enforce(a(x), b(y), c)]] int ok15;

/* Trailing comma in designator-list.  */
[[profiles::enforce(test,)]] int e13; // { dg-error "expected profile-designator" }
[[profiles::enforce(test, other,)]] int e14; // { dg-error "expected profile-designator" }

/* Bad second designator.  */
[[profiles::enforce(test, +)]] int e15; // { dg-error "expected profile-name" }
[[profiles::enforce(test, other::)]] int e16; // { dg-error "expected identifier after" }
[[profiles::enforce(test, other(+))]] int e17; // { dg-error "expected a non-operator-non-punctuator-token" }

/* --- profiles_finish_args: leftover tokens after a designator --- */

[[profiles::enforce(test :)]] int e18; // { dg-error "unexpected token" }
[[profiles::enforce(test +)]] int e19; // { dg-error "unexpected token" }
[[profiles::enforce(test profile)]] int e20; // { dg-error "unexpected token" }
[[profiles::enforce(test(x) +)]] int e21; // { dg-error "unexpected token" }
