// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [identity.require.args]: a require matches the designator's
// tokens, so reordered named arguments do not meet it.

import Req6 [[profiles::require(Vendor::Ruleset(rule: "CoreCheck", fortify: 3))]];
import Req6 [[profiles::require(Vendor::Ruleset(fortify: 3, rule: "CoreCheck"))]]; // { dg-error "with the required arguments" }
