// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }
// TODO: remove warning when profiles attributes location is verified
// { dg-prune-output "scoped attribute directive ignored" }
// { dg-prune-output "warning: attribute ignored" }

// A profile-enforcement of a given profile-name may be repeated only if the
// argument clause is the same token sequence each time; a repetition of the
// same name with a different token sequence is an error.  Comparison is on
// tokens, not on source text.

// ---------------------------------------------------------------------------
// Valid duplications.
// ---------------------------------------------------------------------------

// No argument clause repeated: both sides have no arguments.
[[profiles::enforce(std::type)]];
[[profiles::enforce(std::type)]];

// Identical argument tokens.
[[profiles::enforce(acme::hardened(fortify: 3))]];
[[profiles::enforce(acme::hardened(fortify: 3))]];

// Same tokens but different whitespace: comparison is token-based, not text.
[[profiles::enforce(acme::bounded(check:  yes))]];
[[profiles::enforce(acme::bounded(check: yes))]];

// Repetition across a multi-designator list: 'core::a' (no arguments) is added
// by the list below and repeated on its own afterwards.
[[profiles::enforce(core::a, core::b)]];
[[profiles::enforce(core::a)]];

// The same name repeated within a single designator-list with matching
// arguments is also fine.
[[profiles::enforce(rep::a("x"), rep::a("x"))]];

// Distinct profile-names never conflict, with or without arguments.
[[profiles::enforce(one::p)]];
[[profiles::enforce(two::p(x: 1))]];
[[profiles::enforce(three::p)]];

// ---------------------------------------------------------------------------
// Invalid duplications: same name, different argument tokens.
// ---------------------------------------------------------------------------

// Different argument value (number).
[[profiles::enforce(dup::diff(fortify: 3))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::diff(fortify: 4))]]; // { dg-error "'dup::diff' already loaded with different arguments" }

// One side has an argument clause, the other does not.
[[profiles::enforce(dup::optarg)]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::optarg(x))]]; // { dg-error "already loaded with different arguments" }

// Different number of arguments (different token count).
[[profiles::enforce(dup::count(a, b))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::count(a))]]; // { dg-error "already loaded with different arguments" }

// Different named-argument identifier (different token value).
[[profiles::enforce(dup::key(a: 1))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::key(b: 1))]]; // { dg-error "already loaded with different arguments" }

// Different token type: number vs identifier.
[[profiles::enforce(dup::type(1))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::type(x))]]; // { dg-error "already loaded with different arguments" }

// Different keyword token.
[[profiles::enforce(dup::kw(true))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::kw(false))]]; // { dg-error "already loaded with different arguments" }

// Argument order is part of the token sequence: the same named arguments in a
// different order do not match.
[[profiles::enforce(dup::order(fortify: 3, sanitize: thread))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::order(sanitize: thread, fortify: 3))]]; // { dg-error "already loaded with different arguments" }

// Nested balanced-token argument values, repeated identically -> valid.
[[profiles::enforce(nest::a(opts: (ids: [1, 2])))]];
[[profiles::enforce(nest::a(opts: (ids: [1, 2])))]];

// Nested balanced-token argument values that differ -> conflict.
[[profiles::enforce(nest::b(opts: (ids: [1, 2])))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(nest::b(opts: (ids: [1, 3])))]]; // { dg-error "already loaded with different arguments" }

// A string-literal argument repeated identically -> valid; a different string
// -> conflict.
[[profiles::enforce(dup::str(name: "CoreCheck"))]]; // { dg-message "previously loaded here" }
[[profiles::enforce(dup::str(name: "OtherCheck"))]]; // { dg-error "already loaded with different arguments" }

// ---------------------------------------------------------------------------
// Repetition across standalone and (reordered) multi-designator lists.
//
// The same enforcement may be established on its own and then repeated as an
// element of a larger designator-list, in any order.
//
// NOTE: the first and last lines below contain a stray ')' before ']]'.  The
// enforcement is still recorded (before the error is detected), but the extra
// ')' is itself a syntax error, so those two lines are diagnosed.
// ---------------------------------------------------------------------------
[[profiles::enforce(acme::type(fortify: 3, sanitize: thread)))]]; // { dg-error "expected" }
[[profiles::enforce(lib::hardened)]];
[[profiles::enforce(acme::type(fortify: 3, sanitize: thread), lib::hardened)]];
[[profiles::enforce(lib::hardened, acme::type(fortify: 3, sanitize: thread)))]]; // { dg-error "expected" }
