// D4324: an assertion whose control type reports is_ignored(cfg) == true at
// compile time emits no code and never evaluates its predicate, even when the
// translation-unit default is enforce.  This is the compile-time residue only
// the compiler can provide: a library call form would have to evaluate the
// predicate to make its call.
// { dg-do compile { target c++26 } }
// { dg-additional-options "-fcontracts -fcontract-control-objects -fcontract-evaluation-semantic=enforce -fdump-tree-gimple" }

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
enum class violation_response { proceed, terminate };
struct ignore_t {
  static constexpr bool is_ignored (evaluation_config) { return true; }
  static constexpr bool constify = false;
  static constexpr bool assumable = false;
  void operator() (const char *, std::source_location, evaluation_config) const {}
};
inline constexpr ignore_t ignore {};
}
}

bool expensive_check (int);	// never called: the predicate is not evaluated

int f (int x) pre<std::contracts::ignore>(expensive_check (x)) { return x; }

// The predicate is not evaluated.
// { dg-final { scan-tree-dump-not "expensive_check" "gimple" } }
// No violation call and no violation object are emitted.
// { dg-final { scan-tree-dump-not "__tu_has_violation" "gimple" } }
// { dg-final { scan-tree-dump-not "contract_violation" "gimple" } }
