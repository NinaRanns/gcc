// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -I${srcdir}/g++.dg/profiles -fdump-lang-profiles" }

// An exemption is recorded, and covers the #include directives after it
// that name its header as written ([decl.profile.exempt]).  A recorded
// exemption is not reported as an ignored attribute, whether it is an
// attribute-declaration or on an empty statement.

[[profiles::enforce(acme::type)]];
[[profiles::exempt(acme::type, angle_header: "exempt-record-a.h")]];
[[profiles::exempt(acme::type, quote_header: "exempt-record-b.h", justification: "legacy")]];

#include <exempt-record-a.h>
#include "exempt-record-b.h"

void
f ()
{
  [[profiles::exempt(acme::type, quote_header: "exempt-record-c.h")]];
}

#include "exempt-record-c.h"

// { dg-final { scan-lang-dump {exempt acme::type from <exempt-record-a.h> at line 10\n  covers the #include at exempt-record.C:13\n} profiles } }
// { dg-final { scan-lang-dump {exempt acme::type from "exempt-record-b.h" at line 11\n  covers the #include at exempt-record.C:14\n} profiles } }
// { dg-final { scan-lang-dump {exempt acme::type from "exempt-record-c.h" at line 19\n  covers the #include at exempt-record.C:22\n} profiles } }
