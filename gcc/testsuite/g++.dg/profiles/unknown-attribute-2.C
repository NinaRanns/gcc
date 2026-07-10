// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -Werror=attributes -fmax-errors=1" }
// -Werror adds a note with no location, expected at line 0.
// { dg-message "some warnings being treated as errors" "" { target *-*-* } 0 }

// As unknown-attribute.C, at block scope, where the parser reads the
// attribute-specifier-seq leading a declaration twice: the warning is given
// once.  dg-warning cannot see a duplicate (BZ 30612), so -Werror makes the
// warning count towards -fmax-errors, which allows one and ends the
// compilation at any diagnostic after it.

void
f ()
{
  [[profiles::nope(a)]] int z = 0; // { dg-error "unknown attribute 'profiles::nope' ignored" }
}
