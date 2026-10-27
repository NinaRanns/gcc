// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [ed.profile-ns]: the first example of [decl.profile.enforce]/2
// writes the require with a singular namespace, profile::require, and marks
// it as accepted.  It is not a profiles attribute, so it requires nothing: it
// is ignored, with a warning, as any attribute on an import-declaration is.
// Compare _c, where the same designator, correctly spelled, is an error.

import Kai.Utils [[profile::require(acme::ranges)]];	// { dg-warning "attribute ignored" }
