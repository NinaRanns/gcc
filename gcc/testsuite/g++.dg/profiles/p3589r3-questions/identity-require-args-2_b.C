// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [identity.require.args]: the paper suggests, but does not say,
// that a require matches only the exact designator, arguments included.

// 3 and 0x3 are different tokens, so this require is not met.
import Req5 [[profiles::require(acme(f: 3))]];	// { dg-error "with the required arguments" }
