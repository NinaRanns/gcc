// { dg-do compile { target c++17 } }
// { dg-additional-options "-fprofiles" }

// A name in the profiles namespace that the framework does not define is
// ignored, as any unknown attribute is ([dcl.attr.grammar]/8), with the same
// warning wherever it appears.  Its argument clause is skipped unparsed.

[[profiles::enfoce(std::init)]];	// { dg-warning "unknown attribute 'profiles::enfoce' ignored" }
[[using profiles: exampt(acme::type, angle_header: "a.h")]]; // { dg-warning "unknown attribute 'profiles::exampt' ignored" }

// An attribute-declaration whose attributes are all ignored does not end the
// region in which profiles::enforce may appear.
[[profiles::enforce(e1)]];

[[profiles::bogus]] int x;	// { dg-warning "unknown attribute 'profiles::bogus' ignored" }
[[profiles::nope(a, (b), [c]), gnu::unused]] int y; // { dg-warning "unknown attribute 'profiles::nope' ignored" }

void
f ()
{
  [[profiles::nope(a)]];		// { dg-warning "unknown attribute 'profiles::nope' ignored" }
  [[profiles::supress(acme::type)]] return; // { dg-warning "unknown attribute 'profiles::supress' ignored" }
}
