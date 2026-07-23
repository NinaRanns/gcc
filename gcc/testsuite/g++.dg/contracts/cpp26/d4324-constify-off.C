// D4324: constification is off by default, so a contract predicate binds the
// same overload the function body would - no silent divergence under overload
// resolution.  Naming a control type whose constify member is true restores
// constification for the assertions that name it.
// { dg-do compile { target c++26 } }
// { dg-additional-options "-fcontracts -fcontract-control-objects -fcontract-evaluation-semantic=enforce" }

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

struct constified_t {
  static constexpr bool is_ignored (std::contracts::evaluation_config) { return false; }
  static constexpr bool constify = true;
  static constexpr bool assumable = false;
  void operator() (const char *, std::source_location,
		   std::contracts::evaluation_config) const {}
};
inline constexpr constified_t constified {};

struct S { bool probe (); bool probe () const; };
struct T { bool probe (); bool probe () const; };

int f (S s) pre (s.probe ()) { return 0; }		// default: non-const
int g (T t) pre<constified>(t.probe ()) { return 0; }	// opt-in: const

// Default: the predicate binds the non-const overload the body would bind ...
// { dg-final { scan-assembler "_ZN1S5probeEv" } }
// ... and never the const overload.
// { dg-final { scan-assembler-not "_ZNK1S5probeEv" } }
// A constify=true control restores constification: the const overload binds.
// { dg-final { scan-assembler "_ZNK1T5probeEv" } }
