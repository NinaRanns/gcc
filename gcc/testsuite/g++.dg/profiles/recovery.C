// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles" }

// After an error in a profiles argument clause, the parser recovers as for
// any other attribute: the rest of the attribute-list is skipped, up to the
// `]' that closes the attribute-specifier.  So there is no follow-on error,
// and the declaration the attribute belongs to is kept: the uses at the end
// would be errors if it were lost.

[[profiles::enforce(acme::type)]];

[[profiles::suppress(acme::type, a::b)]] int one;	// { dg-error "expected '\\)' before '::' token" }
[[profiles::suppress(acme::type, a::b), gnu::unused]] int two; // { dg-error "expected '\\)' before '::' token" }

// An unbalanced argument clause is not taken to end on a later line.
[[profiles::suppress(acme::type, x: [)]];		// { dg-error "" }
[[profiles::suppress(acme::type, +1)]] int three;	// { dg-error "expected a non-operator-non-punctuator-token" }

int use = one + two + three;
