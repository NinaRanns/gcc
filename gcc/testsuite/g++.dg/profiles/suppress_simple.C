// { dg-do compile { target c++11 } }
/* { dg-prune-output "suppress. attribute ignored" } */

/* Path coverage for profiles_plugin_suppress_attribute and what it calls.
   Each case is attached to a decl so the attribute handler runs.  */

/* --- profiles_extract_tokens --- */

/* Missing attribute-argument-clause.  */
[[profiles::suppress]] int e1; // { dg-error "" }

/* --- profiles_parse_name --- */

/* Empty body: done before any name.  */
[[profiles::suppress()]] int e2; // { dg-error "expected profile-name" }

/* First token is not an identifier.  */
[[profiles::suppress(42)]] int e3; // { dg-error "expected profile-name" }
[[profiles::suppress(+)]] int e4; // { dg-error "expected profile-name" }
[[profiles::suppress(int)]] int e5; // { dg-error "expected profile-name" }

/* Trailing :: with no following identifier.  */
[[profiles::suppress(test::)]] int e6; // { dg-error "expected identifier after" }

/* --- success: profile-name only (tokens_done after name) --- */

[[profiles::suppress(test)]] int ok1;
[[profiles::suppress(test::profile)]] int ok2;
[[profiles::suppress(a::b::c)]] int ok3;

/* --- after name: not done, next token is not ',' --- */

[[profiles::suppress(test :)]] int e7; // { dg-error "expected .,." }
[[profiles::suppress(test::profile +)]] int e8; // { dg-error "expected .,." }
[[profiles::suppress(test profile)]] int e9; // { dg-error "expected .,." }

/* --- after name ',' : profiles_parse_argument_list + profiles_finish_args --- */

/* Trailing comma with no argument.  */
[[profiles::suppress(test,)]] int e10; // { dg-error "expected profile-argument" }

/* Positional argument.  */
[[profiles::suppress(test, x)]] int ok4;
[[profiles::suppress(test, "why")]] int ok5;

/* Named argument.  */
[[profiles::suppress(test, justification: "why")]] int ok6;
[[profiles::suppress(test, opts: (a, b))]] int ok7;

/* Several arguments.  */
[[profiles::suppress(test, a, b, c)]] int ok8;
[[profiles::suppress(test, rule: "r", justification: "j")]] int ok9;

/* Bad argument after comma.  */
[[profiles::suppress(test, +)]] int e11; // { dg-error "expected a non-operator-non-punctuator-token" }
[[profiles::suppress(test, name:)]] int e12; // { dg-error "expected a non-comma-balanced-token" }
[[profiles::suppress(test, name: ,)]] int e13; // { dg-error "expected a non-comma-balanced-token" }
[[profiles::suppress(test, x,)]] int e14; // { dg-error "expected profile-argument" }
[[profiles::suppress(test, x +)]] int e15; // { dg-error "unexpected token" }
