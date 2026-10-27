// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi SupMod }

// A PROFILED_STMT in an exported inline function, function template and
// constexpr function is streamed with the body.

export module SupMod [[profiles::enforce(P)]];
export inline int inl (int x) { [[profiles::suppress(P)]] if (x) return x + 1; return 0; }
export template <typename T> T tpl (T x) { [[profiles::suppress(P)]] for (int i = 0; i < 2; ++i) x += i; return x; }
export constexpr int cx (int x) { [[profiles::suppress(P)]] return x * 3; }
