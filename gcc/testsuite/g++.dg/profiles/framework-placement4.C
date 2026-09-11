// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles -fmax-errors=2" }

// A misplaced profiles attribute leading a block-scope declaration is
// diagnosed once, although the parser reads that attribute-specifier-seq
// twice (as the statement's, then as the declaration's).  dg-error cannot
// see a duplicate (BZ 30612), so the count is checked by
// -fmax-errors, which allows N errors and ends the compilation, with a
// message that fails the excess-errors test, at any diagnostic after them.
// There are two errors here, and nothing may follow them.

void
f ()
{
  [[profiles::enforce(std::init)]] int y = 0; // { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
  [[profiles::require(std::init)]] int z = y; // { dg-error "may only appear on an import declaration" }
}
