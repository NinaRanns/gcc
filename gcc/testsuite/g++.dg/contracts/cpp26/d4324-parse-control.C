// D4324: parse the optional contract control object on pre/post/
// contract_assert.  pre<obj>(cond), post<obj>(r: cond) and
// contract_assert<obj>(cond) parse, where obj is an id-expression looked up as
// an ordinary name; the bare forms still default; a '<' that starts a predicate
// is still less-than; a variable-template control object parses; and a
// malformed empty control specifier is rejected.
// { dg-do compile { target c++26 } }
// { dg-additional-options "-fcontracts -fcontract-control-objects -fsyntax-only" }

static_assert (__cpp_contracts >= 202502L);

namespace std {
struct source_location {
  struct __impl {
    const char *_M_file_name;
    const char *_M_function_name;
    unsigned _M_line;
    unsigned _M_column;
  };
  const __impl *_M_impl = nullptr;
  constexpr source_location () = default;
};
namespace contracts {
enum class evaluation_config : unsigned { ignore = 0, observe = 1, enforce = 2, quick_enforce = 3 };
}
}
namespace sc = std::contracts;

// Control-object types model the assertion_control interface (constify/assumable
// convertible to bool, a static is_ignored, and a call operator) via a base.
struct ctl_base {
  static constexpr bool is_ignored (sc::evaluation_config) { return false; }
  static constexpr bool constify = false;
  static constexpr bool assumable = false;
  void operator() (const char *, std::source_location, sc::evaluation_config) const {}
};

struct review_t : ctl_base {};
struct mandatory_t : ctl_base {};
template<class T> struct ctl_t : ctl_base {};

inline constexpr review_t review {};
inline constexpr mandatory_t mandatory {};
template<class T> inline constexpr ctl_t<T> ctl {};

int f (int x) pre<review>(x > 0) { return x; }
int g (int x) pre(x > 0) { return x; }
int p (int x) post<review>(r: r > 0) { return x; }
int q (int x) post(r: r > 0) { return x; }
int b (int x) pre<ctl<int>>(x > 0) { return x; }
int lt (int x, int y) pre(x < y) { return x; }

void h (int x)
{
  contract_assert<mandatory>(x > 0);
  contract_assert(x > 0);
  contract_assert(x < 0);
  contract_assert<>(x > 0);   // { dg-error "expected primary-expression" }
}
