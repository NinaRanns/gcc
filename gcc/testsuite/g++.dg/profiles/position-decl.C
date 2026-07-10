// { dg-do compile { target c++26 } }
// { dg-additional-options "-fprofiles" }

// Position audit, declarations outside function bodies: namespace and class
// scope, declarators, parameters, class and enum heads, templates, lambdas.
// The block-scope positions are in position-stmt.C.  Grammar checked against
// the draft at cplusplus/draft d4d6f381 (2026-10-25).
//
// Purpose: show that an attribute-specifier-seq is ACCEPTED at each position,
// and pin what currently happens to a profiles attribute there.  Fates today:
//   D  reaches decl_attributes, which does not know the profiles namespace
//      ("scoped attribute directive ignored")
//   T  appertains to a type, or a construct that drops attributes itself
//      ("attribute ignored", "attributes on base specifiers are ignored")
//   N  the namespace and using-directive paths handle their own attributes;
//      their message omits the attribute's namespace
// Nothing is recorded yet (wip-review.md [b.layer-c], [b.dispatch]).  When
// suppress on a declaration is kept in DECL_ATTRIBUTES (design note §5.1),
// these expectations are meant to change: a FAIL here is the signal to
// update them, not to prune.  No dg-prune-output.

struct B { };
struct P { int a, b; };

[[profiles::suppress(acme::type)]] int v1;		// { dg-warning "scoped attribute directive ignored" } D simple-declaration, leading
int v2 [[profiles::suppress(acme::type)]];		// { dg-warning "scoped attribute directive ignored" } D after declarator-id
int [[profiles::suppress(acme::type)]] * v3;		// { dg-warning "attribute ignored" } T after decl-specifier-seq (the type)
int * [[profiles::suppress(acme::type)]] v4;		// { dg-warning "scoped attribute directive ignored" } D ptr-operator
int v5 [3] [[profiles::suppress(acme::type)]];		// { dg-warning "scoped attribute directive ignored" } D array declarator
void f1 [[profiles::suppress(acme::type)]] ();		// { dg-warning "scoped attribute directive ignored" } D function, after declarator-id
void f2 () [[profiles::suppress(acme::type)]];		// { dg-warning "scoped attribute directive ignored" } D function type
[[profiles::suppress(acme::type)]] void f3 () { }	// { dg-warning "scoped attribute directive ignored" } D function definition, leading
void f4 ([[profiles::suppress(acme::type)]] int);	// { dg-warning "scoped attribute directive ignored" } D parameter, leading
struct SX { void mx ([[profiles::suppress(acme::type)]] this SX self); };	// { dg-warning "scoped attribute directive ignored" } D explicit object parameter
void f5 (int p [[profiles::suppress(acme::type)]]);	// { dg-warning "scoped attribute directive ignored" } D parameter, after declarator-id
struct [[profiles::suppress(acme::type)]] C1 { };	// { dg-warning "scoped attribute directive ignored" } D class-head
struct [[profiles::suppress(acme::type)]] C2;		// { dg-warning "scoped attribute directive ignored" } D elaborated forward declaration
enum [[profiles::suppress(acme::type)]] E1 {		// { dg-warning "scoped attribute directive ignored" } D enum-head
  e1 [[profiles::suppress(acme::type)]]			// { dg-warning "scoped attribute directive ignored" } D enumerator
};
enum class [[profiles::suppress(acme::type)]] E2 : int;	// { dg-warning "scoped attribute directive ignored" } D opaque-enum-declaration
namespace [[profiles::suppress(acme::type)]] N1 { }	// { dg-warning "'suppress' attribute directive ignored" } N namespace-definition
[[profiles::suppress(acme::type)]] using namespace N1;	// { dg-warning "'suppress' attribute directive ignored" } N using-directive
[[profiles::suppress(acme::type)]];			// { dg-warning "attribute ignored" } T attribute-declaration (not enforce)

struct S1 {
  [[profiles::suppress(acme::type)]] int m1;		// { dg-warning "scoped attribute directive ignored" } D member-declaration, leading
  int m2 [[profiles::suppress(acme::type)]];		// { dg-warning "scoped attribute directive ignored" } D member, after declarator-id
  int m3 [[profiles::suppress(acme::type)]] : 3;	// { dg-warning "scoped attribute directive ignored" } D bit-field
  [[profiles::suppress(acme::type)]] void mf1 ();	// { dg-warning "scoped attribute directive ignored" } D member function declaration
  [[profiles::suppress(acme::type)]] void mf2 () { }	// { dg-warning "scoped attribute directive ignored" } D member function definition
  [[profiles::suppress(acme::type)]] S1 ();		// { dg-warning "scoped attribute directive ignored" } D constructor
  [[profiles::suppress(acme::type)]] friend void ff ();	// { dg-warning "attribute ignored" } T friend declaration (ill-formed: [dcl.attr.grammar] requires a friend declaration with an attribute to be a definition; GCC only warns)
  [[profiles::suppress(acme::type)]] static constexpr int sm = 1;	// { dg-warning "scoped attribute directive ignored" } D static data member
};

struct D1 : [[profiles::suppress(acme::type)]] B { };	// { dg-warning "attributes on base specifiers are ignored" } T base-specifier

template <class T> [[profiles::suppress(acme::type)]] void tf1 () { }	// { dg-warning "scoped attribute directive ignored" } D template declaration
template <class T> using AT1 [[profiles::suppress(acme::type)]] = T;	// { dg-warning "scoped attribute directive ignored" } D alias template
template [[profiles::suppress(acme::type)]] void tf1<int> ();	// { dg-warning "scoped attribute directive ignored" } D explicit instantiation (ill-formed: [temp.explicit] "No attribute-specifier-seq shall appertain to an explicit instantiation"; GCC only warns)
template <> [[profiles::suppress(acme::type)]] void tf1<char> ();	// { dg-warning "scoped attribute directive ignored" } D explicit specialization
template <class T> concept C3 [[profiles::suppress(acme::type)]] = true;	// { dg-warning "scoped attribute directive ignored" } D concept-definition
template <[[profiles::suppress(acme::type)]] int N> struct TN { };	// { dg-warning "scoped attribute directive ignored" } D template parameter-declaration

auto l1 = [] [[profiles::suppress(acme::type)]] () { };	// { dg-warning "scoped attribute directive ignored" } D lambda-declarator
auto l2 = [] () [[profiles::suppress(acme::type)]] { };	// { dg-warning "scoped attribute directive ignored" } D lambda function type
auto [sb1 [[profiles::suppress(acme::type)]], sb2] = P { };	// { dg-warning "scoped attribute directive ignored" } D structured binding, namespace scope
extern "C" { [[profiles::suppress(acme::type)]] int lc1; }	// { dg-warning "scoped attribute directive ignored" } D inside linkage-specification

// Not a position: an alias-declaration takes its attribute after the identifier.
namespace N1 { [[profiles::suppress(acme::type)]] using B2 = B; }	// { dg-error "expected unqualified-id" }
