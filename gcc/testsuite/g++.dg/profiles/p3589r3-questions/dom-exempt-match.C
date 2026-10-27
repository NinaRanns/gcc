// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -I${srcdir}/g++.dg/profiles/p3589r3-questions -fdump-lang-profiles" }

// Review item [dom.exempt.match]: P3589R3 can be read as matching an
// exemption against the #include directive, or against the file the named
// search finds.  The prototype matches the directive as written, its
// delimiters and its spelling, and only after the exemption
// ([markup.exempt.position]).

[[profiles::enforce(acme::type)]];
#include <dom-exempt-match-a.h>

// After its #include: diagnosed, and it does not cover that directive.
[[profiles::exempt(acme::type, angle_header: "dom-exempt-match-a.h")]]; // { dg-warning "follows an '#include' of it, which it does not cover" }
// { dg-message "included here" "" { target *-*-* } 11 }

[[profiles::exempt(acme::type, angle_header: "dom-exempt-match-b.h")]];
[[profiles::exempt(acme::type, quote_header: "dom-exempt-match-c.h")]];

#include "dom-exempt-match-a.h"	// the other delimiters: not covered
#include <./dom-exempt-match-a.h>	// another spelling of the file: not covered
#include <dom-exempt-match-b.h>	// covered; and it includes "dom-exempt-match-c.h", covered by name
#include "dom-exempt-match-c.h"	// covered, though #pragma once keeps it out

// { dg-final { scan-lang-dump {exempt acme::type from <dom-exempt-match-a.h> at line 14\nexempt} profiles } }
// { dg-final { scan-lang-dump {exempt acme::type from <dom-exempt-match-b.h> at line 17\n  covers the #include at dom-exempt-match.C:22\n} profiles } }
// { dg-final { scan-lang-dump {exempt acme::type from "dom-exempt-match-c.h" at line 18\n  covers the #include at dom-exempt-match-b.h:3\n  covers the #include at dom-exempt-match.C:23\n} profiles } }
