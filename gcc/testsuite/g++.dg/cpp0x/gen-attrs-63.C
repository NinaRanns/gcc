// PR c++/79967
// { dg-do compile { target c++11 } }
// The original problem was an ICE.  [[noreturn]] may only be applied to a
// function ([dcl.attr.noreturn]), so this is ill-formed ([dcl.attr.grammar]);
// now that template parameter attributes are processed, the attribute gives
// the parameter a noreturn function type and A<f> is rejected.

template <void f [[noreturn]]()>
struct A
{
  int g () { f (); return 0; }
};

void f ();

void g (A<f> a) { a.g (); }	// { dg-error "could not convert template argument|is not a valid template argument" }
// { dg-error "request for member" "" { target *-*-* } .-1 }
