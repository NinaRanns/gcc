// { dg-do compile { target c++11 } }
// Attributes on a non-type template parameter (a parameter-declaration,
// [temp.param]) are processed as for a function parameter, so an unknown
// attribute is diagnosed rather than dropped silently.

template <[[acme::x]] int N1> struct T1 { };	// { dg-warning "scoped attribute directive ignored" }
template <int N2 [[acme::x]]> struct T2 { };	// { dg-warning "scoped attribute directive ignored" }
template <[[maybe_unused]] int N3> struct T3 { };

#if __cpp_nontype_template_parameter_auto
template <[[acme::x]] auto N4> struct T4 { };	// { dg-warning "scoped attribute directive ignored" "" { target c++17 } }
#endif

#if __cpp_concepts
template <class T> concept C = true;
template <[[acme::x]] C auto N5> struct T5 { };	// { dg-warning "scoped attribute directive ignored" "" { target c++20 } }
#endif

// Contrast: a function parameter has always diagnosed this.
void f ([[acme::x]] int);			// { dg-warning "scoped attribute directive ignored" }

T1<1> t1;
T2<2> t2;
T3<3> t3;
