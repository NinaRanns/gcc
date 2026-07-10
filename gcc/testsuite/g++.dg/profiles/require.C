// { dg-do compile }
// { dg-options "-fprofiles" }
//
// Syntax coverage for:
//   [[profiles::require(profile-designator-list)]]
// with the same profile-designator / profile-argument grammar as enforce.
// Module-import placement is covered separately under g++.dg/modules/.

// Paper-shaped: single simple profile-name.
[[profiles::require(std::type)]];

// Paper-shaped: single nested profile-name.
[[profiles::require(std::lib::hardened)]];

// Paper-shaped: profile-name with named profile-arguments.
[[profiles::require(acme::hardened(fortify: 3, sanitize: thread))]];

// Paper-shaped: several profile-designators in one list.
[[profiles::require(std::lib::hardened, acme::type)]];

// Exact repetition of the same designator token sequence.
[[profiles::require(std::lib::hardened)]];
[[profiles::require(std::lib::hardened)]];

// Bare identifier as profile-name.
[[profiles::require(type)]];

// Two-segment profile-name.
[[profiles::require(acme::type)]];

// Three-segment profile-name.
[[profiles::require(acme::safety::bounds)]];

// Vendor-style nested profile-name.
[[profiles::require(Vendor::Ruleset)]];

// Designator with a single positional profile-argument.
[[profiles::require(Vendor::Ruleset("CoreCheck"))]];

// Designator with positional then named profile-arguments.
[[profiles::require(Vendor::Ruleset("CoreCheck", fortify: 3))]];

// Designator with only named profile-arguments.
[[profiles::require(acme(fortify: 3))]];

// Designator with several named profile-arguments.
[[profiles::require(acme(fortify: 3, sanitize: thread))]];

// Named argument whose value is a parenthesized balanced-token.
[[profiles::require(acme(opts: (fortify: 3)))]];

// Commas are OK inside a single balanced (...) value.
[[profiles::require(acme(opts: (sanitize: thread, fortify: 2)))]];

// Named argument whose value is a bracket balanced-token.
[[profiles::require(acme(ids: [1, 2, 3]))]];

// Named argument whose value is a brace balanced-token.
[[profiles::require(acme(map: {a: 1, b: 2}))]];

// Nested balanced punctuation inside one argument value.
[[profiles::require(acme(rule: (a::b<c, d>::e)))]];

// String literal as a named argument value.
[[profiles::require(acme(name: "CoreCheck"))]];

// String literal as a positional argument.
[[profiles::require(acme("CoreCheck"))]];

// Integer positional argument.
[[profiles::require(acme(3))]];

// Keyword positional argument.
[[profiles::require(acme(true))]];

// Identifier positional argument.
[[profiles::require(acme(thread))]];

// Mixed designators: bare name, then name with arguments.
[[profiles::require(std::type, acme(fortify: 3))]];

// Mixed designators: name with arguments, then bare name.
[[profiles::require(acme(fortify: 3), std::type)]];

// Three designators in one list.
[[profiles::require(std::lib::hardened, acme::type, Vendor::Ruleset("CoreCheck"))]];

// Named argument value that is the ':' token itself.
[[profiles::require(acme(name: :))]];

// Empty balanced [] as a named argument value.
[[profiles::require(acme(x: []))]];

// Punctuator '+' as a named argument value.
[[profiles::require(acme(op: +))]];

// Punctuator '==' as a named argument value.
[[profiles::require(acme(op: ==))]];

// Punctuator '->' as a named argument value.
[[profiles::require(acme(op: ->))]];

// Punctuator '...' as a named argument value.
[[profiles::require(acme(op: ...))]];

// Punctuator '::' as a named argument value.
[[profiles::require(acme(op: ::))]];

// Five positional arguments inside a designator.
[[profiles::require(acme(a, b, c, d, e))]];

// Named then positional arguments inside a designator.
[[profiles::require(acme(rule: "x", "y"))]];

// Positional then named arguments inside a designator.
[[profiles::require(acme("x", rule: "y"))]];

// Empty profile-designator-list.
[[profiles::require()]]; // { dg-error "" }

// Trailing comma in the designator-list.
[[profiles::require(std::type,)]]; // { dg-error "" }

// Empty designator between commas in the designator-list.
[[profiles::require(std::type,, acme::type)]]; // { dg-error "" }

// Designator with empty argument-list parentheses.
[[profiles::require(acme())]]; // { dg-error "" }

// Trailing comma inside a designator's argument-list.
[[profiles::require(acme(fortify: 3,))]]; // { dg-error "" }

// Empty argument between commas inside a designator.
[[profiles::require(acme(a,, b))]]; // { dg-error "" }

// Positional argument that is a punctuator.
[[profiles::require(acme(+))]]; // { dg-error "" }

// Positional argument that is '::'.
[[profiles::require(acme(::))]]; // { dg-error "" }

// Positional argument that is '*'.
[[profiles::require(acme(*))]]; // { dg-error "" }

// Parenthesized expression is not a positional non-punctuator token.
[[profiles::require(acme((a)))]]; // { dg-error "" }

// Named form with no value after ':'.
[[profiles::require(acme(name:))]]; // { dg-error "" }

// Named form whose value is ')'.
[[profiles::require(acme(name: ))]]; // { dg-error "" }

// Named form whose value is ','.
[[profiles::require(acme(name: ,))]]; // { dg-error "" }

// Profile-name cannot start with '::'.
[[profiles::require(::type)]]; // { dg-error "" }

// Profile-name cannot end with a trailing '::'.
[[profiles::require(acme::)]]; // { dg-error "" }

// Profile-name cannot end with a trailing '::' after a segment.
[[profiles::require(acme::type::)]]; // { dg-error "" }

// Keyword is not an identifier profile-name.
[[profiles::require(int)]]; // { dg-error "" }

// After 'std' comes '::', so this is not a named argument 'std'.
[[profiles::require(acme(std::rule: "x"))]]; // { dg-error "" }

// Named value is only 'std'; '::x' is leftover inside the argument-list.
[[profiles::require(acme(rule: std::x))]]; // { dg-error "" }

// Qualified-id as a positional argument is more than one token.
[[profiles::require(acme(a::b))]]; // { dg-error "" }

// Unbalanced ')' inside a bracket value.
[[profiles::require(acme(x: [))]]; // { dg-error "" }

// Leftover tokens after a balanced value inside the argument-list.
[[profiles::require(acme(x: ([({ })]))"y"))]]; // { dg-error "" }

// 'class' is a keyword, so not a named-argument identifier.
[[profiles::require(acme(class: x))]]; // { dg-error "" }

// Missing closing ')' of the attribute-argument-clause.
[[profiles::require(std::type]]; // { dg-error "" }

// Stray tokens after a complete designator before the attribute ')'.
[[profiles::require(std::type +)]]; // { dg-error "" }

// Valid again: whole '(std::x)' is one balanced-token named value.
[[profiles::require(acme(rule: (std::x)))]];

// Valid again: nested '[[1]]' balances before the designator ')'.
[[profiles::require(acme(ids: [[1]]))]];

// Valid again: commas inside a string; the string is one token.
[[profiles::require(acme(j: "a, b, and c"))]];

// Valid again: ']]' inside a string does not end the attribute.
[[profiles::require(acme(j: "]]"))]];
