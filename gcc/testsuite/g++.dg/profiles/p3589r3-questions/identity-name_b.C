// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [identity.name]: P3589R3 does not say whether a profile-name is
// one name or a hierarchy.  The prototype treats it as one name, so a require
// of the parent is not met by an enforcement of the child, nor the reverse.

import Nm [[profiles::require(test::profile)]];
import Nm [[profiles::require(test)]];			// { dg-error "does not enforce required profile 'test'" }
import Nm [[profiles::require(test::profile::sub)]];	// { dg-error "does not enforce required profile 'test::profile::sub'" }
