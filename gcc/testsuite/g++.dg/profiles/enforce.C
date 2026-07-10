// { dg-do compile }
// { dg-options "-fprofiles" }

// Paper-shaped: single simple profile-name.
[[profiles::enforce(std::type)]];

// Paper-shaped: single nested profile-name.
[[profiles::enforce(std::lib::hardened)]];

// Paper-shaped: profile-name with named profile-arguments.
[[profiles::enforce(acme::hardened(fortify: 3, sanitize: thread))]];

// Paper-shaped: several profile-designators in one list.
[[profiles::enforce(std::lib::hardened, acme::type)]];

// Exact repetition of the same designator token sequence is allowed.
[[profiles::enforce(std::lib::hardened)]];
[[profiles::enforce(std::lib::hardened)]];

// Bare identifier as profile-name.
[[profiles::enforce(type)]];

// Two-segment profile-name.
[[profiles::enforce(acme::type)]];

// Three-segment profile-name.
[[profiles::enforce(acme::safety::bounds)]];

// Vendor-style nested profile-name.
[[profiles::enforce(Vendor::Ruleset)]];

// Designator with a single positional profile-argument.
[[profiles::enforce(Vendor::Ruleset("CoreCheck"))]];

// Designator with positional then named profile-arguments.
[[profiles::enforce(Vendor::Ruleset("CoreCheck", fortify: 3))]];

// Designator with only named profile-arguments.
[[profiles::enforce(acme(fortify: 3))]];

// Designator with several named profile-arguments.
[[profiles::enforce(acme(fortify: 3, sanitize: thread))]];

// Named argument whose value is a parenthesized balanced-token.
[[profiles::enforce(acme(opts: (fortify: 3)))]];

// Commas are OK inside a single balanced (...) value.
[[profiles::enforce(acme(opts: (sanitize: thread, fortify: 2)))]];

// Named argument whose value is a bracket balanced-token.
[[profiles::enforce(acme(ids: [1, 2, 3]))]];

// Named argument whose value is a brace balanced-token.
[[profiles::enforce(acme(map: {a: 1, b: 2}))]];

// Nested balanced punctuation inside one argument value.
[[profiles::enforce(acme(rule: (a::b<c, d>::e)))]];

// String literal as a named argument value.
[[profiles::enforce(acme(name: "CoreCheck"))]];

// String literal as a positional argument.
[[profiles::enforce(acme("CoreCheck"))]];

// Integer positional argument.
[[profiles::enforce(acme(3))]];

// Keyword positional argument.
[[profiles::enforce(acme(true))]];

// Identifier positional argument.
[[profiles::enforce(acme(thread))]];

// Mixed designators: bare name, then name with arguments.
[[profiles::enforce(std::type, acme(fortify: 3))]];

// Mixed designators: name with arguments, then bare name.
[[profiles::enforce(acme(fortify: 3), std::type)]];

// Three designators in one list.
[[profiles::enforce(std::lib::hardened, acme::type, Vendor::Ruleset("CoreCheck"))]];

// Named argument value that is the ':' token itself.
[[profiles::enforce(acme(name: :))]];

// Empty balanced [] as a named argument value.
[[profiles::enforce(acme(x: []))]];

// Punctuator '+' as a named argument value.
[[profiles::enforce(acme(op: +))]];

// Punctuator '==' as a named argument value.
[[profiles::enforce(acme(op: ==))]];

// Punctuator '->' as a named argument value.
[[profiles::enforce(acme(op: ->))]];

// Punctuator '...' as a named argument value.
[[profiles::enforce(acme(op: ...))]];

// Punctuator '::' as a named argument value.
[[profiles::enforce(acme(op: ::))]];

// Five positional arguments inside a designator.
[[profiles::enforce(acme(a, b, c, d, e))]];

// Named then positional arguments inside a designator.
[[profiles::enforce(acme(rule: "x", "y"))]];

// Positional then named arguments inside a designator.
[[profiles::enforce(acme("x", rule: "y"))]];

// Empty profile-designator-list.
[[profiles::enforce()]]; // { dg-error "" }

// Trailing comma in the designator-list.
[[profiles::enforce(std::type,)]]; // { dg-error "" }

// Empty designator between commas in the designator-list.
[[profiles::enforce(std::type,, acme::type)]]; // { dg-error "" }

// Designator with empty argument-list parentheses.
[[profiles::enforce(acme())]]; // { dg-error "" }

// Trailing comma inside a designator's argument-list.
[[profiles::enforce(acme(fortify: 3,))]]; // { dg-error "" }

// Empty argument between commas inside a designator.
[[profiles::enforce(acme(a,, b))]]; // { dg-error "" }

// Positional argument that is a punctuator.
[[profiles::enforce(acme(+))]]; // { dg-error "" }

// Positional argument that is '::'.
[[profiles::enforce(acme(::))]]; // { dg-error "" }

// Positional argument that is '*'.
[[profiles::enforce(acme(*))]]; // { dg-error "" }

// Parenthesized expression is not a positional non-punctuator token.
[[profiles::enforce(acme((a)))]]; // { dg-error "" }

// Named form with no value after ':'.
[[profiles::enforce(acme(name:))]]; // { dg-error "" }

// Named form whose value is ')'.
[[profiles::enforce(acme(name: ))]]; // { dg-error "" }

// Named form whose value is ','.
[[profiles::enforce(acme(name: ,))]]; // { dg-error "" }

// Profile-name cannot start with '::'.
[[profiles::enforce(::type)]]; // { dg-error "" }

// Profile-name cannot end with a trailing '::'.
[[profiles::enforce(acme::)]]; // { dg-error "" }

// Profile-name cannot end with a trailing '::' after a segment.
[[profiles::enforce(acme::type::)]]; // { dg-error "" }

// Keyword is not an identifier profile-name.
[[profiles::enforce(int)]]; // { dg-error "" }

// After 'std' comes '::', so this is not a named argument 'std'.
[[profiles::enforce(acme(std::rule: "x"))]]; // { dg-error "" }

// Named value is only 'std'; '::x' is leftover inside the argument-list.
[[profiles::enforce(acme(rule: std::x))]]; // { dg-error "" }

// Qualified-id as a positional argument is more than one token.
[[profiles::enforce(acme(a::b))]]; // { dg-error "" }

// Unbalanced ')' inside a bracket value.
[[profiles::enforce(acme(x: [))]]; // { dg-error "" }

// Leftover tokens after a balanced value inside the argument-list.
[[profiles::enforce(acme(x: ([({ })]))"y"))]]; // { dg-error "" }

// 'class' is a keyword, so not a named-argument identifier.
[[profiles::enforce(acme(class: x))]]; // { dg-error "" }

// Missing closing ')' of the attribute-argument-clause.
[[profiles::enforce(std::type]]; // { dg-error "" }

// Stray tokens after a complete designator before the attribute ')'.
[[profiles::enforce(std::type +)]]; // { dg-error "" }

// Valid again: whole '(std::x)' is one balanced-token named value.
[[profiles::enforce(acme(rule: (std::x)))]];

// Valid again: nested '[[1]]' balances before the designator ')'.
[[profiles::enforce(acme(ids: [[1]]))]];

// Valid again: commas inside a string; the string is one token.
[[profiles::enforce(acme(j: "a, b, and c"))]];

// Valid again: ']]' inside a string does not end the attribute.
[[profiles::enforce(acme(j: "]]"))]];
