// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// TODO: remove warning when profiles attributes location is verified
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

// A profile-enforcement attribute-declaration must precede any declaration
// that is not an attribute-declaration or empty-declaration, so these come
// first.  Exact token-sequence repetitions of an enforcement are allowed.
[[profiles::enforce(std::lib::hardened)]];
[[profiles::enforce(acme::hardened(fortify: 3, sanitize: thread))]];
[[profiles::enforce(std::lib::hardened)]];
[[profiles::enforce(std::type)]];
[[profiles::enforce(std::lib::hardened)]]; // OK.
[[profiles::enforce(acme::hardened(fortify: 3, sanitize: thread))]];
[[profiles::enforce(std::type)]];

[[profiles::suppress(std::type, justification: "buffer is filled in by read()")]];
[[profiles::suppress(acme::type,  rule: "acme.cast.reinterpret",  justification: "thumbstone pointer value")]];
[[profiles::suppress(acme::type,  rule: "acme.cast.reinterpret",  justification: (thumbstone pointer value))]];

[[profiles::suppress(acme)]];
[[profiles::suppress(acme::type)]];

[[profiles::suppress(type, justification: "local")]] int x;
[[profiles::suppress(std::type, justification: "local")]] int x1;
[[profiles::suppress(std::lib::hardened, justification: "local")]] int x2;
[[profiles::suppress(acme::type, justification: "local")]] int x3;
[[profiles::suppress(Vendor::Ruleset, justification: "local")]] int x4;
[[profiles::suppress(acme::safety::bounds, justification: "local")]] int x5;

// Simple tokens (literals / identifiers):

[[profiles::suppress(acme::type, rule: "acme.cast.reinterpret")]] int* p = nullptr;
[[profiles::suppress(acme::type, justification: "thumbstone pointer value")]] int* p2 = nullptr;
[[profiles::suppress(acme::type, rule: reinterpret)]];  // identifier as rule id
[[profiles::suppress(acme::type, level: 3)]];
[[profiles::suppress(acme::type, mode: thread)]];
[[profiles::suppress(acme::type, enabled: true)]];
[[profiles::suppress(acme::type, tag: 'X')]];
[[profiles::suppress(acme::type, score: 1.5)]];
[[profiles::suppress(acme::type, mask: 0xdeadbeef)]];

// Parenthesized balanced tokens:

[[profiles::suppress(acme::type, rule: (cast.reinterpret))]];
[[profiles::suppress(acme::type, rule: (a + b * c))]];
[[profiles::suppress(acme::type, opts: (fortify: 3))]];
[[profiles::suppress(acme::type, opts: (sanitize: thread, fortify: 2))]];  // commas OK inside ()
[[profiles::suppress(acme::type, expr: ((nested)))]];

// Bracket / brace balanced tokens:

[[profiles::suppress(acme::type, ids: [1, 2, 3])]];
[[profiles::suppress(acme::type, names: ["a", "b"])]];
[[profiles::suppress(acme::type, map: {x: 1})]];
[[profiles::suppress(acme::type, map: {a: 1, b: 2})]];
[[profiles::suppress(acme::type, nested: [{a: 1}, {b: 2}])]];
[[profiles::suppress(acme::type, pack: ([1], (2), {3}))]];

// Positional args: non-operator-non-punctuator-token
[[profiles::suppress(acme::type, "acme.cast.reinterpret")]];
[[profiles::suppress(acme::type, reinterpret)]];
[[profiles::suppress(acme::type, 3)]];
[[profiles::suppress(acme::type, true)]];
[[profiles::suppress(acme::type, thread)]];
[[profiles::suppress(Vendor::Ruleset, "CoreCheck")]];

//Mixed named + positional, 2–5 arguments
[[profiles::suppress(acme::type, "acme.cast.reinterpret", justification: "thumbstone")]];

[[profiles::suppress(std::type,
  rule: "init.var",
  justification: "filled by read()",
  severity: note)]];
[[profiles::suppress(Vendor::Ruleset,
  "CoreCheck",
  fortify: 3,
  sanitize: thread)]];

[[profiles::suppress(acme::safety::bounds,
  rule: "bounds.index",
  justification: "checked manually",
  level: 2,
  mode: audit)]];

[[profiles::suppress(acme::type,
  reinterpret,
  justification: "tag pointer",
  owner: "lib",
  ticket: "BUG-42")]];

[[profiles::suppress(acme::type,
  rule: "acme.cast.reinterpret",
  justification: "thumbstone pointer value",
  owner: "platform",
  ticket: "BUG-42",
  severity: warning)]];

[[profiles::suppress(Vendor::Ruleset,
  "CoreCheck",
  fortify: 3,
  sanitize: thread,
  plugin: {name: "bounds", level: 2},
  justification: "legacy FFI")]];

[[profiles::suppress(std::lib::hardened,
  rule: ("bounds", "null"),
  ids: [1, 2, 3],
  opts: (fortify: 3),
  map: {a: 1, b: 2},
  justification: "third-party header")]];

// nested punctuation inside a single balanced token
[[profiles::suppress(acme::type, rule: (a::b<c, d>::e))]];
[[profiles::suppress(acme::type, rule: [a[b], c])]];
[[profiles::suppress(acme::type, rule: {a{b}, c})]];
// string / raw-ish literals as values
[[profiles::suppress(acme::type, justification: "a, b, and c")]];  // commas inside string OK
[[profiles::suppress(acme::type, rule: R"(acme.cast.reinterpret)")]];
// keyword / literal positional soup
[[profiles::suppress(p, 1, true, "x", nullptr, alignas)]];  // 5 positional non-punctuator tokens


// curious, but valid
[[profiles::suppress(acme::type, name: :)]];       // valid: value is ':'
[[profiles::suppress(acme::type, x: [])]];         // valid: empty balanced []

// looks sensible, but invalid
[[profiles::suppress(acme::type, std::rule: "x")]]; // { dg-error "" } after std comes :: not :
[[profiles::suppress(acme::type, a::b)]];           // { dg-error "" } positional a, then :: leftover
[[profiles::suppress(acme::type, +1)]];             // { dg-error "" } + is punctuator
[[profiles::suppress(acme::type, -1)]];             // { dg-error "" } - is punctuator

// Closers / "empty" values
[[profiles::suppress(acme::type, )]];              // { dg-error "" } ) is punctuator
[[profiles::suppress(acme::type,]];                // { dg-error "" } ] is punctuator
[[profiles::suppress(acme::type, name:)]];         // { dg-error "" } missing value after :
[[profiles::suppress(acme::type, name: )]];        // { dg-error "" } ) not a balanced-token
[[profiles::suppress(acme::type, name:]]];         // { dg-error "" } ] not a balanced-token
[[profiles::suppress(acme::type, name: ,)]];       // { dg-error "" } comma forbidden as the value

// ]] vs balanced [...]
[[profiles::suppress(acme::type, ids: [1, 2])]];   // valid: ] closes [, then ) ]], classic OK
[[profiles::suppress(acme::type, ids: [1, 2]])]];  // { dg-error "" } ] inside )
[[profiles::suppress(acme::type, ids: [[1]])]];    // valid: nested [] balance before )]]
[[profiles::suppress(acme::type, x: [)]];          // { dg-error "" } ) inside [] before ]

// Max-munch / colon soup
[[profiles::suppress(acme::type, name:::x)]];      // { dg-error "" }
[[profiles::suppress(acme::type, name: ::x)]];     // { dg-error "" } value ::, then x leftover
[[profiles::suppress(acme::type, name: : :)]];     // { dg-error "" } value :, then : leftover
[[profiles::suppress(acme::type, a::b)]];          // { dg-error "" } positional a; :: leftover
[[profiles::suppress(acme::type, a::b: c)]];       // { dg-error "" } positional a, then ::
                                                   // NOT named a::b — profile-argument name is a single identifier
// Named arg name must be a single identifier
[[profiles::suppress(acme::type, rule: std::x)]];  // { dg-error "" } value std, then ::x leftover
[[profiles::suppress(acme::type, rule: (std::x))]]; // valid: whole (std::x) is one balanced-token
[[profiles::suppress(acme::type, class: x)]];      // { dg-error "" } class is keyword, not a named-arg id
// Positional = exactly one non-punctuator token
[[profiles::suppress(acme::type, "a,b")]];         // valid: commas inside string
[[profiles::suppress(acme::type, R"(a)b)")]];      // valid raw string
[[profiles::suppress(acme::type, 1.5e+2)]];        // valid: '+' is inside a single preprocessing number
[[profiles::suppress(acme::type, true)]];          // valid keyword
[[profiles::suppress(acme::type, nullptr)]];       // valid
[[profiles::suppress(acme::type, *)]];             // { dg-error "" } * is a punctuator
[[profiles::suppress(acme::type, rule: *)]];       // valid named: * is a balanced-token != comma
// Punctuator as named value (allowed) vs positional (not)
[[profiles::suppress(acme::type, op: +)]];
[[profiles::suppress(acme::type, op: ==)]];
[[profiles::suppress(acme::type, op: ->)]];
[[profiles::suppress(acme::type, op: ...)]];
[[profiles::suppress(acme::type, op: ::)]];
[[profiles::suppress(acme::type, +)]];             // { dg-error "" } invalid positional
[[profiles::suppress(acme::type, ::)]];            // { dg-error "" } invalid positional
// Nested balanced junk as a single value
[[profiles::suppress(acme::type, x: (a, b, c))]];          // valid: commas inside ()
[[profiles::suppress(acme::type, x: (a:1, b:2))]];         // valid
[[profiles::suppress(acme::type, x: {a[1], b(2)})]];       // valid
[[profiles::suppress(acme::type, x: ([({ })]))"y")]];      // { dg-error "" } )"y" is junk
[[profiles::suppress(acme::type, x: (()), y: [[]] )]];     // valid: two named args
// Multi-arg boundaries
[[profiles::suppress(acme::type, a, b, c, d, e)]];         // valid: 5 positionals
[[profiles::suppress(acme::type, a,, b)]];                 // { dg-error "" } empty arg between commas
[[profiles::suppress(acme::type, a, )]];                   // { dg-error "" } trailing comma then )
[[profiles::suppress(acme::type, rule: "x", "y")]];        // valid: named then positional
[[profiles::suppress(acme::type, "x", rule: "y")]];        // valid: positional then named
[[profiles::suppress(acme::type, rule: a, b: c, d)]];      // valid: named, named, positional
// Profile-name edges
[[profiles::suppress(::type, x)]];                 // { dg-error "" } can't start with ::
[[profiles::suppress(acme::, x)]];                 // { dg-error "" } trailing ::
[[profiles::suppress(acme::type::, x)]];           // { dg-error "" } trailing ::
[[profiles::suppress(acme::type::rule, x)]];       // valid name (3 segments), arg x
[[profiles::suppress(int, x)]];                    // { dg-error "" } int is keyword, name needs identifier
[[profiles::suppress(acme::type, justification: "why", rule: "r")]];  // valid: order free
// Attribute / string gotchas
[[profiles::suppress(acme::type, j: "]]")]];       // valid: ]] inside string, not attr end
[[profiles::suppress(acme::type, j: "a\"b")]];
[[profiles::suppress(acme::type, j: L"wide")]];
[[profiles::suppress(acme::type, j: u8"x")]];
// The "looks deep but is shallow" family
[[profiles::suppress(acme::type, (a))]];           // { dg-error "" } ( is a punctuator
[[profiles::suppress(acme::type, x: (a))]];        // valid named
[[profiles::suppress(acme::type, x: a)]];          // valid
[[profiles::suppress(acme::type, x: (a, b), y)]];  // valid


extern int read(char* buf, int n);
int main()
{
 [[profiles::suppress(std::type)]] char buffer[1024]; // OK.
 int len = read(buffer, 1024);

 [[profiles::suppress(std::type, justification: "buffer is filled in by read()")]]
 char buff[1024]; // OK

 [[profiles::suppress(acme::type,  rule: "acme.cast.reinterpret")]]
 int* ptr = reinterpret_cast<int*>(0xdeadbeef);

 [[profiles::suppress(acme::type,  rule: "acme.cast.reinterpret",  justification: "thumbstone pointer value")]]
 int* ptr2 = reinterpret_cast<int*>(0xdeadbeef);
}
