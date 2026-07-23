// D4324: a guaranteed-enforced control type whose assumable member is true
// lets the optimizer treat an ignored predicate as an assumption, so a
// downstream operation can be simplified.  At the ignore configuration the
// predicate is handed to the optimizer (no runtime evaluation), and a later
// branch that contradicts it is eliminated.
// { dg-do compile { target c++26 } }
// { dg-additional-options "-fcontracts -fcontract-control-objects -fcontract-evaluation-semantic=ignore -O2 -fdump-tree-optimized" }

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
enum class evaluation_config : unsigned {
  ignore = 0, observe = 1, enforce = 2, quick_enforce = 3
};
}
}

struct mandatory_t {
  static constexpr bool is_ignored (std::contracts::evaluation_config c)
  { return c == std::contracts::evaluation_config::ignore; }
  static constexpr bool constify = false;
  static constexpr bool assumable = true;
  void operator() (const char *, std::source_location,
		   std::contracts::evaluation_config) const {}
};
inline constexpr mandatory_t mandatory {};

void sink (int);

int f (int x) pre<mandatory>(x > 5)
{
  if (x <= 5)
    sink (x);		// dead: the predicate x > 5 is assumed
  return x;
}

// The dead branch guarded by the negation of the assumed predicate is gone.
// { dg-final { scan-tree-dump-not "sink" "optimized" } }
