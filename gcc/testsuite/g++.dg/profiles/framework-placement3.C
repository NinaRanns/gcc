// { dg-do compile { target c++11 } }
// { dg-additional-options "-fprofiles" }

// A misplaced profiles::enforce is diagnosed and NOT recorded, so a second
// misplaced one with different arguments draws only its own placement
// error, not "already loaded with different arguments".

int x;
[[profiles::enforce(dup::p(1))]];	// { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
[[profiles::enforce(dup::p(2))]];	// { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
void f ()
{
  [[profiles::enforce(blk::p(1))]];	// { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
  [[profiles::enforce(blk::p(2))]];	// { dg-error "may only appear on a module-declaration, or in an attribute-declaration" }
}
