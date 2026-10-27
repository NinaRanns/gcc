// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Review item [interact.unenforced]: P3589R3 does not say whether suppressing
// or exempting a profile that the translation unit does not enforce is an
// error or a no-op.  The prototype accepts both.

[[profiles::exempt(test::profile, angle_header: "unistd.h")]];
[[profiles::suppress(test::profile)]] int x;
