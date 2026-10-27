// { dg-do compile { target c++26 } }
// { dg-additional-options "-fprofiles -fdump-lang-profiles" }

// Position audit, declarations outside function bodies: namespace and class
// scope, declarators, parameters, class and enum heads, templates, lambdas.
// The block-scope positions are in position-stmt.C.  Grammar checked against
// the draft at cplusplus/draft d4d6f381 (2026-10-25).
//
// Purpose: show that an attribute-specifier-seq is ACCEPTED at each position,
// and pin what currently happens to a profiles attribute there.  Fates today:
//   K  kept with the entity declared (DECL_ATTRIBUTES; for a class or
//      enumeration, TYPE_ATTRIBUTES), checked below in -fdump-lang-profiles
//   T  appertains to a type, or a construct that drops attributes itself
//      ("'profiles::suppress' on a type is ignored", "attribute ignored",
//      "attributes on base specifiers are ignored")
//   N  the namespace and using-directive paths handle their own attributes;
//      their message omits the attribute's namespace
// A FAIL here is the signal to update these expectations, not to prune.
// No dg-prune-output.

struct B { };
struct P { int a, b; };

[[profiles::suppress(acme::type)]] int v1;		// K simple-declaration, leading
int v2 [[profiles::suppress(acme::type)]];		// K after declarator-id
int [[profiles::suppress(acme::type)]] * v3;		// { dg-warning "attribute ignored" } T after decl-specifier-seq (the type)
int * [[profiles::suppress(acme::type)]] v4;		// { dg-warning "'profiles::suppress' on a type is ignored" } T ptr-operator
int v5 [3] [[profiles::suppress(acme::type)]];		// { dg-warning "'profiles::suppress' on a type is ignored" } T array declarator
void f1 [[profiles::suppress(acme::type)]] ();		// K function, after declarator-id
void f2 () [[profiles::suppress(acme::type)]];		// { dg-warning "'profiles::suppress' on a type is ignored" } T function type
[[profiles::suppress(acme::type)]] void f3 () { }	// K function definition, leading
void f4 ([[profiles::suppress(acme::type)]] int);	// K parameter, leading
struct SX { void mx ([[profiles::suppress(acme::type)]] this SX self); };	// K explicit object parameter
void f5 (int p [[profiles::suppress(acme::type)]]);	// K parameter, after declarator-id
struct [[profiles::suppress(acme::type)]] C1 { };	// K class-head
struct [[profiles::suppress(acme::type)]] C2;		// K elaborated forward declaration
enum [[profiles::suppress(acme::type)]] E1 {		// K enum-head
  e1 [[profiles::suppress(acme::type)]]			// K enumerator
};
enum class [[profiles::suppress(acme::type)]] E2 : int;	// K opaque-enum-declaration
namespace [[profiles::suppress(acme::type)]] N1 { }	// { dg-warning "'suppress' attribute directive ignored" } N namespace-definition
[[profiles::suppress(acme::type)]] using namespace N1;	// { dg-warning "'suppress' attribute directive ignored" } N using-directive
[[profiles::suppress(acme::type)]];			// { dg-warning "attribute ignored" } T attribute-declaration (not enforce)

struct S1 {
  [[profiles::suppress(acme::type)]] int m1;		// K member-declaration, leading
  int m2 [[profiles::suppress(acme::type)]];		// K member, after declarator-id
  int m3 [[profiles::suppress(acme::type)]] : 3;	// K bit-field
  [[profiles::suppress(acme::type)]] void mf1 ();	// K member function declaration
  [[profiles::suppress(acme::type)]] void mf2 () { }	// K member function definition
  [[profiles::suppress(acme::type)]] S1 ();		// K constructor
  [[profiles::suppress(acme::type)]] friend void ff ();	// { dg-warning "attribute ignored" } T friend declaration (ill-formed: [dcl.attr.grammar] requires a friend declaration with an attribute to be a definition; GCC only warns)
  [[profiles::suppress(acme::type)]] static constexpr int sm = 1;	// K static data member
};

struct D1 : [[profiles::suppress(acme::type)]] B { };	// { dg-warning "attributes on base specifiers are ignored" } T base-specifier

template <class T> [[profiles::suppress(acme::type)]] void tf1 () { }	// K template declaration
template <class T> using AT1 [[profiles::suppress(acme::type)]] = T;	// K (kept with the template: recorded when it is instantiated, which this test does not do) alias template
template [[profiles::suppress(acme::type)]] void tf1<int> ();	// K (recorded on the instantiation, at the template's line) explicit instantiation (ill-formed: [temp.explicit] "No attribute-specifier-seq shall appertain to an explicit instantiation"; GCC only warns)
template <> [[profiles::suppress(acme::type)]] void tf1<char> ();	// K explicit specialization
template <class T> concept C3 [[profiles::suppress(acme::type)]] = true;	// K concept-definition
template <[[profiles::suppress(acme::type)]] int N> struct TN { };	// K template parameter-declaration

auto l1 = [] [[profiles::suppress(acme::type)]] () { };	// K lambda-declarator
auto l2 = [] () [[profiles::suppress(acme::type)]] { };	// { dg-warning "'profiles::suppress' on a type is ignored" } T lambda function type
auto [sb1 [[profiles::suppress(acme::type)]], sb2] = P { };	// K structured binding, namespace scope
extern "C" { [[profiles::suppress(acme::type)]] int lc1; }	// K inside linkage-specification

// Not a position: an alias-declaration takes its attribute after the identifier.
namespace N1 { [[profiles::suppress(acme::type)]] using B2 = B; }	// { dg-error "expected unqualified-id" }

// What each K position kept, and on which entity.
// { dg-final { scan-lang-dump {suppress acme::type on var_decl v1 at line 24\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl v2 at line 25\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl f1 at line 29\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl f3 at line 31\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on parm_decl <anonymous> at line 32\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on parm_decl self at line 33\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on parm_decl p at line 34\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on record_type C1 at line 35\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on record_type C2 at line 36\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on enumeral_type E1 at line 37\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on const_decl e1 at line 38\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on enumeral_type E2 at line 40\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on field_decl m1 at line 46\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on field_decl m2 at line 47\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on field_decl m3 at line 48\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl mf1 at line 49\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl mf2 at line 50\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl S1 at line 51\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl sm at line 53\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl tf1 at line 58\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl tf1<int> at line 58\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl tf1<char> at line 61\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on concept_decl C3 at line 62\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on parm_decl N at line 63\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on function_decl operator\(\) at line 65\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl sb1 at line 67\n} profiles } }
// { dg-final { scan-lang-dump {suppress acme::type on var_decl lc1 at line 68\n} profiles } }
