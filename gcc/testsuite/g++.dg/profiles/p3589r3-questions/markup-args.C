// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Review item [markup.args]: a profile-argument is a single token that is not
// an operator-or-punctuator, or "identifier :" followed by a single
// non-comma balanced-token ([decl.attr.grammar]/1 as amended by section 2.2).
// That grammar is both too loose and too strict.  The prototype implements it
// as written, which shows both halves.

// Too loose: accepted, although neither looks like a meaningful argument.
[[profiles::enforce(a1(name: :))]];
[[profiles::enforce(a2(x: []))]];
[[profiles::enforce(a3(op: ::))]];

// Too strict: rejected, although each looks reasonable.

// The key must be a single identifier.
[[profiles::enforce(b1(std::rule: "x"))]];	// { dg-error "expected '\\)' before '::' token" }

// A positional argument must be a single token.
[[profiles::enforce(b2(a::b))]];		// { dg-error "expected '\\)' before '::' token" }

// ... and not an operator, so a signed number is not an argument.
[[profiles::enforce(b3(+1))]];			// { dg-error "expected a non-operator-non-punctuator-token" }
[[profiles::enforce(b4(-1))]];			// { dg-error "expected a non-operator-non-punctuator-token" }

// A named argument's value must be a single balanced-token.
[[profiles::enforce(b5(level: -1))]];		// { dg-error "expected '\\)' before numeric constant" }

// The same cases on suppress, which shares the grammar.
// { dg-prune-output "attribute ignored" }
[[profiles::suppress(acme::type, name: :)]];
[[profiles::suppress(acme::type, x: [])]];
[[profiles::suppress(acme::type, std::rule: "x")]];	// { dg-error "expected '\\)' before '::' token" }
[[profiles::suppress(acme::type, a::b)]];		// { dg-error "expected '\\)' before '::' token" }
[[profiles::suppress(acme::type, +1)]];			// { dg-error "expected a non-operator-non-punctuator-token" }
[[profiles::suppress(acme::type, -1)]];			// { dg-error "expected a non-operator-non-punctuator-token" }

// The grammar fixes form, not meaning: positional and named arguments are
// both accepted, and whether they mean the same is the profile's to say ...
[[profiles::enforce(c1("CoreCheck", 3))]];
[[profiles::enforce(c2(rule: "CoreCheck", fortify: 3))]];
// ... but a trailing comma is rejected, and no profile can relax that.
[[profiles::enforce(c3(fortify: 3, rule: "CoreCheck", ))]]; // { dg-error "expected a non-operator-non-punctuator-token" }
