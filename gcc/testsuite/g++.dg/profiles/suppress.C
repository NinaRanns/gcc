// { dg-do compile }
// { dg-options "-fprofiles" }

// Paper-shaped examples: named rule / justification with string values.
[[profiles::suppress(std::type, justification: "buffer is filled in by read()")]];
[[profiles::suppress(acme::type, rule: "acme.cast.reinterpret", justification: "thumbstone pointer value")]];

// Named justification whose value is a parenthesized balanced-token sequence.
[[profiles::suppress(acme::type, rule: "acme.cast.reinterpret", justification: (thumbstone pointer value))]];

// Suppress with profile-name only (no argument list).
[[profiles::suppress(acme)]];
[[profiles::suppress(acme::type)]];

// Various valid profile-names (1 to 3 nested name segments).
[[profiles::suppress(type, justification: "local")]] int x;
[[profiles::suppress(std::type, justification: "local")]] int x1;
[[profiles::suppress(std::lib::hardened, justification: "local")]] int x2;
[[profiles::suppress(acme::type, justification: "local")]] int x3;
[[profiles::suppress(Vendor::Ruleset, justification: "local")]] int x4;
[[profiles::suppress(acme::safety::bounds, justification: "local")]] int x5;

// Named args with simple non-comma-balanced-token values (string / identifier / literal).
[[profiles::suppress(acme::type, rule: "acme.cast.reinterpret")]];
[[profiles::suppress(acme::type, justification: "thumbstone pointer value")]];

// Identifier as rule id.
[[profiles::suppress(acme::type, rule: reinterpret)]];
[[profiles::suppress(acme::type, level: 3)]];
[[profiles::suppress(acme::type, mode: thread)]];
[[profiles::suppress(acme::type, enabled: true)]];
[[profiles::suppress(acme::type, tag: 'X')]];
[[profiles::suppress(acme::type, score: 1.5)]];
[[profiles::suppress(acme::type, mask: 0xdeadbeef)]];

// Named args whose values are parenthesized balanced-tokens.
[[profiles::suppress(acme::type, rule: (cast.reinterpret))]];
[[profiles::suppress(acme::type, rule: (a + b * c))]];
[[profiles::suppress(acme::type, opts: (fortify: 3))]];

// Commas are OK inside a single balanced (...) token.
[[profiles::suppress(acme::type, opts: (sanitize: thread, fortify: 2))]];
[[profiles::suppress(acme::type, expr: ((nested)))]];

// Named args whose values are bracket / brace balanced-tokens.
[[profiles::suppress(acme::type, ids: [1, 2, 3])]];
[[profiles::suppress(acme::type, names: ["a", "b"])]];
[[profiles::suppress(acme::type, map: {x: 1})]];
[[profiles::suppress(acme::type, map: {a: 1, b: 2})]];
[[profiles::suppress(acme::type, nested: [{a: 1}, {b: 2}])]];
[[profiles::suppress(acme::type, pack: ([1], (2), {3}))]];

// Positional args: a single non-operator-non-punctuator-token each.
[[profiles::suppress(acme::type, "acme.cast.reinterpret")]];
[[profiles::suppress(acme::type, reinterpret)]];
[[profiles::suppress(acme::type, 3)]];
[[profiles::suppress(acme::type, true)]];
[[profiles::suppress(acme::type, thread)]];
[[profiles::suppress(Vendor::Ruleset, "CoreCheck")]];

// Mixed named + positional, 2 arguments.
[[profiles::suppress(acme::type, "acme.cast.reinterpret", justification: "thumbstone")]];

// Mixed named args, 3 arguments.
[[profiles::suppress(std::type,
  rule: "init.var",
  justification: "filled by read()",
  severity: note)]];

// Positional then named, 3 arguments.
[[profiles::suppress(Vendor::Ruleset,
  "CoreCheck",
  fortify: 3,
  sanitize: thread)]];

// Named args, 4 arguments.
[[profiles::suppress(acme::safety::bounds,
  rule: "bounds.index",
  justification: "checked manually",
  level: 2,
  mode: audit)]];

// Positional then named, 4 arguments.
[[profiles::suppress(acme::type,
  reinterpret,
  justification: "tag pointer",
  owner: "lib",
  ticket: "BUG-42")]];

// Named args, 5 arguments.
[[profiles::suppress(acme::type,
  rule: "acme.cast.reinterpret",
  justification: "thumbstone pointer value",
  owner: "platform",
  ticket: "BUG-42",
  severity: warning)]];

// Positional + named including a brace balanced-token, 5 arguments.
[[profiles::suppress(Vendor::Ruleset,
  "CoreCheck",
  fortify: 3,
  sanitize: thread,
  plugin: {name: "bounds", level: 2},
  justification: "legacy FFI")]];

// Named args with several balanced-token shapes, 5 arguments.
[[profiles::suppress(std::lib::hardened,
  rule: ("bounds", "null"),
  ids: [1, 2, 3],
  opts: (fortify: 3),
  map: {a: 1, b: 2},
  justification: "third-party header")]];

// Nested punctuation inside a single balanced-token value.
[[profiles::suppress(acme::type, rule: (a::b<c, d>::e))]];
[[profiles::suppress(acme::type, rule: [a[b], c])]];
[[profiles::suppress(acme::type, rule: {a{b}, c})]];

// Commas inside a string are fine; the string is one token.
[[profiles::suppress(acme::type, justification: "a, b, and c")]];

// Raw string literal as a named value.
[[profiles::suppress(acme::type, rule: R"(acme.cast.reinterpret)")]];

// Five positional non-punctuator tokens (keywords / literals / string).
[[profiles::suppress(p, 1, true, "x", nullptr, alignas)]];

// Curious but valid: named value is the ':' token itself.
[[profiles::suppress(acme::type, name: :)]];

// Curious but valid: empty balanced [] as the value.
[[profiles::suppress(acme::type, x: [])]];

// ')' where a profile-argument is expected; ')' is a punctuator.
[[profiles::suppress(acme::type, )]]; // { dg-error "" }

// ']' where a profile-argument is expected; ']' is a punctuator.
[[profiles::suppress(acme::type,]]; // { dg-error "" }

// Named form with no value after ':'.
[[profiles::suppress(acme::type, name:)]]; // { dg-error "" }

// Named form whose value is ')'; not a balanced-token.
[[profiles::suppress(acme::type, name: )]]; // { dg-error "" }

// Named form whose value is ']'; not a balanced-token.
[[profiles::suppress(acme::type, name:]]]; // { dg-error "" }

// Named form whose value is ','; comma is forbidden as the value.
[[profiles::suppress(acme::type, name: ,)]]; // { dg-error "" }

// Valid: ']' closes the '[', then ')' and ']]' close normally.
[[profiles::suppress(acme::type, ids: [1, 2])]];

// Invalid: ']]' appears before the closing ')'.
[[profiles::suppress(acme::type, ids: [1, 2]])]; // { dg-error "" }

// Valid: nested '[[1]]' balances before ')]]'.
[[profiles::suppress(acme::type, ids: [[1]])]];

// Invalid: ')' inside '[]' before the matching ']'.
[[profiles::suppress(acme::type, x: [)]]; // { dg-error "" }

// 'name:::x' lexes as name :: : x; not a valid named or positional arg list.
[[profiles::suppress(acme::type, name:::x)]]; // { dg-error "" }

// Named value is '::', then 'x' is leftover junk.
[[profiles::suppress(acme::type, name: ::x)]]; // { dg-error "" }

// Named value is first ':', then second ':' is leftover junk.
[[profiles::suppress(acme::type, name: : :)]]; // { dg-error "" }

// Positional takes 'a' only; '::b' is leftover.
[[profiles::suppress(acme::type, a::b)]]; // { dg-error "" }

// Positional takes 'a' only; not named 'a::b' — arg name is a single identifier.
[[profiles::suppress(acme::type, a::b: c)]]; // { dg-error "" }

// After 'std' comes '::', not ':', so this is not a named argument.
[[profiles::suppress(acme::type, std::rule: "x")]]; // { dg-error "" }

// Named value is only 'std'; '::x' is leftover.
[[profiles::suppress(acme::type, rule: std::x)]]; // { dg-error "" }

// Valid: whole '(std::x)' is one balanced-token value.
[[profiles::suppress(acme::type, rule: (std::x))]];

// 'class' is a keyword, so this is not identifier ':' named form.
[[profiles::suppress(acme::type, class: x)]]; // { dg-error "" }

// Valid: commas inside the string; the string is one token.
[[profiles::suppress(acme::type, "a,b")]];

// Valid raw string literal as a positional arg.
[[profiles::suppress(acme::type, R"(a)b)")]];

// Hex float preprocessing-number is typically one token.
[[profiles::suppress(acme::type, 0x1p+2)]];

// Keywords / literals are non-operator-non-punctuator tokens.
[[profiles::suppress(acme::type, true)]];
[[profiles::suppress(acme::type, nullptr)]];

// Invalid positional: '*' is a punctuator.
[[profiles::suppress(acme::type, *)]]; // { dg-error "" }

// Valid named: '*' is a balanced-token other than comma.
[[profiles::suppress(acme::type, rule: *)]];

// Punctuator as named value (allowed) vs positional (not)
[[profiles::suppress(acme::type, op: +)]];
[[profiles::suppress(acme::type, op: ==)]];
[[profiles::suppress(acme::type, op: ->)]];
[[profiles::suppress(acme::type, op: ...)]];
[[profiles::suppress(acme::type, op: ::)]];

// Invalid positional: '+' is a punctuator.
[[profiles::suppress(acme::type, +)]]; // { dg-error "" }

// Invalid positional: '::' is a punctuator.
[[profiles::suppress(acme::type, ::)]]; // { dg-error "" }

// Valid: commas inside a single '(...)' balanced-token.
[[profiles::suppress(acme::type, x: (a, b, c))]];
[[profiles::suppress(acme::type, x: (a:1, b:2))]];
[[profiles::suppress(acme::type, x: {a[1], b(2)})]];

// Balancers match for the value, then ')"y"' is leftover junk.
[[profiles::suppress(acme::type, x: ([({ })]))"y")]]; // { dg-error "" }

// Two named args; inner '[[]]' must not be mistaken for attribute end.
[[profiles::suppress(acme::type, x: (()), y: [[]] )]];

// Valid: five positional arguments.
[[profiles::suppress(acme::type, a, b, c, d, e)]];

// Invalid: empty argument between commas.
[[profiles::suppress(acme::type, a,, b)]]; // { dg-error "" }

// Invalid: trailing comma then ')'.
[[profiles::suppress(acme::type, a, )]]; // { dg-error "" }

// Valid: named then positional.
[[profiles::suppress(acme::type, rule: "x", "y")]];

// Valid: positional then named.
[[profiles::suppress(acme::type, "x", rule: "y")]];

// Valid: named, named, positional.
[[profiles::suppress(acme::type, rule: a, b: c, d)]];

// Invalid name: cannot start with '::'.
[[profiles::suppress(::type, x)]]; // { dg-error "" }

// Invalid name: trailing '::'.
[[profiles::suppress(acme::, x)]]; // { dg-error "" }

// Invalid name: trailing '::' after type.
[[profiles::suppress(acme::type::, x)]]; // { dg-error "" }

// Valid name with three segments, plus one positional arg.
[[profiles::suppress(acme::type::rule, x)]];

// 'int' is a keyword; profile-name requires an identifier.
[[profiles::suppress(int, x)]]; // { dg-error "" }

// Valid: relative order of named args does not matter.
[[profiles::suppress(acme::type, justification: "why", rule: "r")]];

// Valid: ']]' inside a string does not end the attribute.
[[profiles::suppress(acme::type, j: "]]")]];
[[profiles::suppress(acme::type, j: "a\"b")]];
[[profiles::suppress(acme::type, j: L"wide")]];
[[profiles::suppress(acme::type, j: u8"x")]];

// Invalid positional: '(' is a punctuator.
[[profiles::suppress(acme::type, (a))]]; // { dg-error "" }

// Valid named: '(a)' is one balanced-token.
[[profiles::suppress(acme::type, x: (a))]];

// Valid named: value is a single identifier.
[[profiles::suppress(acme::type, x: a)]];

// Valid: named balanced value, then a positional.
[[profiles::suppress(acme::type, x: (a, b), y)]];
