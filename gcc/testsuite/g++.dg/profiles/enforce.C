// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Each profile-enforcement uses a distinct profile-name so that the
// translation unit's duplicate-enforcement checking does not interfere;
// repetition/duplication is exercised separately in enforce-dup.C.

// Paper-shaped: single simple profile-name.
[[profiles::enforce(sn1::type)]];

// Paper-shaped: single nested profile-name.
[[profiles::enforce(sn2::lib::hardened)]];

// Paper-shaped: profile-name with named profile-arguments.
[[profiles::enforce(sn3::hardened(fortify: 3, sanitize: thread))]];

// Paper-shaped: several profile-designators in one list.
[[profiles::enforce(sd1::hardened, sd1::type)]];

// Exact repetition of the same designator token sequence is allowed.
[[profiles::enforce(rep::hardened)]];
[[profiles::enforce(rep::hardened)]];

// Bare identifier as profile-name.
[[profiles::enforce(bareid)]];

// Two-segment profile-name.
[[profiles::enforce(seg2::type)]];

// Three-segment profile-name.
[[profiles::enforce(seg3::safety::bounds)]];

// Vendor-style nested profile-name.
[[profiles::enforce(Vend1::Ruleset)]];

// Designator with a single positional profile-argument.
[[profiles::enforce(Vend2::Ruleset("CoreCheck"))]];

// Designator with positional then named profile-arguments.
[[profiles::enforce(Vend3::Ruleset("CoreCheck", fortify: 3))]];

// Designator with only named profile-arguments.
[[profiles::enforce(a01(fortify: 3))]];

// Designator with several named profile-arguments.
[[profiles::enforce(a02(fortify: 3, sanitize: thread))]];

// Named argument whose value is a parenthesized balanced-token.
[[profiles::enforce(a03(opts: (fortify: 3)))]];

// Commas are OK inside a single balanced (...) value.
[[profiles::enforce(a04(opts: (sanitize: thread, fortify: 2)))]];

// Named argument whose value is a bracket balanced-token.
[[profiles::enforce(a05(ids: [1, 2, 3]))]];

// Named argument whose value is a brace balanced-token.
[[profiles::enforce(a06(map: {a: 1, b: 2}))]];

// Nested balanced punctuation inside one argument value.
[[profiles::enforce(a07(rule: (a::b<c, d>::e)))]];

// String literal as a named argument value.
[[profiles::enforce(a08(name: "CoreCheck"))]];

// String literal as a positional argument.
[[profiles::enforce(a09("CoreCheck"))]];

// Raw string literal as a named argument value.
[[profiles::enforce(a10(name: R"(CoreCheck)"))]];

// Raw string literal as a positional argument.
[[profiles::enforce(a11(R"(CoreCheck)"))]];

// Raw string literal with a custom delimiter as a named argument value.
[[profiles::enforce(a12(name: R"xyz(Core"Check)xyz"))]];

// Integer positional argument.
[[profiles::enforce(a13(3))]];

// Keyword positional argument.
[[profiles::enforce(a14(true))]];

// Identifier positional argument.
[[profiles::enforce(a15(thread))]];

// Mixed designators: bare name, then name with arguments.
[[profiles::enforce(m1::type, m1::acme(fortify: 3))]];

// Mixed designators: name with arguments, then bare name.
[[profiles::enforce(m2::acme(fortify: 3), m2::type)]];

// Three designators in one list.
[[profiles::enforce(t1::hardened, t2::type, t3::ruleset("CoreCheck"))]];

// Named argument value that is the ':' token itself.
[[profiles::enforce(a16(name: :))]];

// Empty balanced [] as a named argument value.
[[profiles::enforce(a17(x: []))]];

// Punctuator '+' as a named argument value.
[[profiles::enforce(a18(op: +))]];

// Punctuator '==' as a named argument value.
[[profiles::enforce(a19(op: ==))]];

// Punctuator '->' as a named argument value.
[[profiles::enforce(a20(op: ->))]];

// Punctuator '...' as a named argument value.
[[profiles::enforce(a21(op: ...))]];

// Punctuator '::' as a named argument value.
[[profiles::enforce(a22(op: ::))]];

// Five positional arguments inside a designator.
[[profiles::enforce(a23(a, b, c, d, e))]];

// Named then positional arguments inside a designator.
[[profiles::enforce(a24(rule: "x", "y"))]];

// Positional then named arguments inside a designator.
[[profiles::enforce(a25("x", rule: "y"))]];

// Empty profile-designator-list.
[[profiles::enforce()]]; // { dg-error "" }

// Trailing comma in the designator-list.
[[profiles::enforce(e01::type,)]]; // { dg-error "" }

// Empty designator between commas in the designator-list.
[[profiles::enforce(e02::type,, e03::type)]]; // { dg-error "" }

// Designator with empty argument-list parentheses.
[[profiles::enforce(e04())]]; // { dg-error "" }

// Trailing comma inside a designator's argument-list.
[[profiles::enforce(e05(fortify: 3,))]]; // { dg-error "" }

// Empty argument between commas inside a designator.
[[profiles::enforce(e06(a,, b))]]; // { dg-error "" }

// Positional argument that is a punctuator.
[[profiles::enforce(e07(+))]]; // { dg-error "" }

// Positional argument that is '::'.
[[profiles::enforce(e08(::))]]; // { dg-error "" }

// Positional argument that is '*'.
[[profiles::enforce(e09(*))]]; // { dg-error "" }

// Parenthesized expression is not a positional non-punctuator token.
[[profiles::enforce(e10((a)))]]; // { dg-error "" }

// Named form with no value after ':'.
[[profiles::enforce(e11(name:))]]; // { dg-error "" }

// Named form whose value is ')'.
[[profiles::enforce(e12(name: ))]]; // { dg-error "" }

// Named form whose value is ','.
[[profiles::enforce(e13(name: ,))]]; // { dg-error "" }

// Profile-name cannot start with '::'.
[[profiles::enforce(::type)]]; // { dg-error "" }

// Profile-name cannot end with a trailing '::'.
[[profiles::enforce(e14::)]]; // { dg-error "" }

// Profile-name cannot end with a trailing '::' after a segment.
[[profiles::enforce(e15::type::)]]; // { dg-error "" }

// Keyword is not an identifier profile-name.
[[profiles::enforce(int)]]; // { dg-error "" }

// After 'std' comes '::', so this is not a named argument 'std'.
[[profiles::enforce(e16(std::rule: "x"))]]; // { dg-error "" }

// Named value is only 'std'; '::x' is leftover inside the argument-list.
[[profiles::enforce(e17(rule: std::x))]]; // { dg-error "" }

// Qualified-id as a positional argument is more than one token.
[[profiles::enforce(e18(a::b))]]; // { dg-error "" }

// Unbalanced ')' inside a bracket value.
[[profiles::enforce(e19(x: [))]]; // { dg-error "" }

// Leftover tokens after a balanced value inside the argument-list.
[[profiles::enforce(e20(x: ([({ })]))"y"))]]; // { dg-error "" }

// 'class' is a keyword, so not a named-argument identifier.
[[profiles::enforce(e21(class: x))]]; // { dg-error "" }

// Missing closing ')' of the attribute-argument-clause.
[[profiles::enforce(e22::type]]; // { dg-error "" }

// Stray tokens after a complete designator before the attribute ')'.
[[profiles::enforce(e23::type +)]]; // { dg-error "" }

// Valid again: whole '(std::x)' is one balanced-token named value.
[[profiles::enforce(a26(rule: (std::x)))]];

// Valid again: nested '[[1]]' balances before the designator ')'.
[[profiles::enforce(a27(ids: [[1]]))]];

// Valid again: commas inside a string; the string is one token.
[[profiles::enforce(a28(j: "a, b, and c"))]];

// Valid again: ']]' inside a string does not end the attribute.
[[profiles::enforce(a29(j: "]]"))]];

// Valid again: ']]' inside a raw string does not end the attribute either.
[[profiles::enforce(a30(j: R"(]])"))]];

// Valid again: a raw string is one token, so its inner '(' ')' are not
// balanced-token delimiters.
[[profiles::enforce(a31(rule: R"(a(b)c)"))]];
