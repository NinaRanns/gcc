// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 } }

// Copyright (C) 2016-2022 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <vector>
#include <string>
#include "../../../../include/std/basic_variant.h"

using namespace std;
using namespace std;


template<typename _Tp = void,
         bool POCCA = false,
         bool POCMA = false,
         bool POCS = false,
         bool IAE = false>
struct TrivialAlloc{
   /// The allocator type
    using allocator_type = TrivialAlloc<_Tp>;

    /// The allocated type
    using value_type = _Tp;

    /// The allocator's pointer type.
    using pointer = _Tp*;

    /// The allocator's const pointer type.
    using const_pointer = const _Tp*;

    /// The allocator's void pointer type.
    using void_pointer = void*;

    /// The allocator's const void pointer type.
    using const_void_pointer = const void*;

    /// The allocator's difference type
    using difference_type = std::ptrdiff_t;

    /// The allocator's size type
    using size_type = std::size_t;

    using propagate_on_container_copy_assignment = integral_constant<bool, POCCA>;
    using propagate_on_container_move_assignment = integral_constant<bool, POCMA>;
    using propagate_on_container_swap = integral_constant<bool, POCS>;

    static allocator_type
    select_on_container_copy_construction(const allocator_type& other) noexcept
    { return other; }
    /// @}

    /// Whether all instances of the allocator type compare equal.
    using is_always_equal = integral_constant<bool, IAE>;

    constexpr TrivialAlloc() = default;
    template<typename U>
    constexpr TrivialAlloc(const TrivialAlloc<U>&){ }

};


struct AllDeleted
{
  AllDeleted() = delete;
  AllDeleted(const AllDeleted&) = delete;
  AllDeleted(AllDeleted&&) = delete;
  AllDeleted& operator=(const AllDeleted&) = delete;
  AllDeleted& operator=(AllDeleted&&) = delete;
};

struct Empty
{
  Empty() { };
  Empty(const Empty&) { };
  Empty(Empty&&) { };
  Empty& operator=(const Empty&) { return *this; };
  Empty& operator=(Empty&&) { return *this; };
};

struct DefaultNoexcept
{
  DefaultNoexcept() noexcept = default;
  DefaultNoexcept(const DefaultNoexcept&) noexcept = default;
  DefaultNoexcept(DefaultNoexcept&&) noexcept = default;
  DefaultNoexcept& operator=(const DefaultNoexcept&) noexcept = default;
  DefaultNoexcept& operator=(DefaultNoexcept&&) noexcept = default;
};

struct MoveCtorOnly
{
  MoveCtorOnly() noexcept = delete;
  MoveCtorOnly(const MoveCtorOnly&) noexcept = delete;
  MoveCtorOnly(MoveCtorOnly&&) noexcept { }
  MoveCtorOnly& operator=(const MoveCtorOnly&) noexcept = delete;
  MoveCtorOnly& operator=(MoveCtorOnly&&) noexcept = delete;
};

struct MoveCtorAndSwapOnly : MoveCtorOnly { };
void swap(MoveCtorAndSwapOnly&, MoveCtorAndSwapOnly&) { }

struct DeletedMoves
{
  DeletedMoves() = default;
  DeletedMoves(const DeletedMoves&) = default;
  DeletedMoves(DeletedMoves&&) = delete;
  DeletedMoves& operator=(const DeletedMoves&) = default;
  DeletedMoves& operator=(DeletedMoves&&) = delete;
};

struct nonliteral
{
  nonliteral() { }

  bool operator<(const nonliteral&) const;
  bool operator<=(const nonliteral&) const;
  bool operator==(const nonliteral&) const;
  bool operator!=(const nonliteral&) const;
  bool operator>=(const nonliteral&) const;
  bool operator>(const nonliteral&) const;
};

struct virtual_default_dtor {
   virtual ~virtual_default_dtor() = default;
};

void default_ctor()
{
  static_assert(is_default_constructible_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(is_default_constructible_v<basic_variant<TrivialAlloc<int>, string, string>>);
  static_assert(!is_default_constructible_v<basic_variant<TrivialAlloc<int>, AllDeleted, string>>);
  static_assert(is_default_constructible_v<basic_variant<TrivialAlloc<int>, string, AllDeleted>>);
  static_assert(is_default_constructible_v<basic_variant<TrivialAlloc<int>, DeletedMoves>>);

  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int>()));
  static_assert(!noexcept(basic_variant<TrivialAlloc<int>, Empty>()));
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, DefaultNoexcept>()));
  {
    basic_variant<TrivialAlloc<int>, virtual_default_dtor> a;
  }
}

void copy_ctor()
{
  static_assert(is_copy_constructible_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(!is_copy_constructible_v<basic_variant<TrivialAlloc<int>, AllDeleted, string>>);
  static_assert(!is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int>, int>>);
  static_assert(!is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int>, std::string>>);
  static_assert(!is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int>, DeletedMoves>>);

  {
    basic_variant<TrivialAlloc<int>, int> a;
    static_assert(!noexcept(basic_variant<TrivialAlloc<int>, int>(a)));
  }
  {
    basic_variant<TrivialAlloc<int>, string> a;
    static_assert(!noexcept(basic_variant<TrivialAlloc<int>, string>(a)));
  }
  {
    basic_variant<TrivialAlloc<int>, int, string> a;
    static_assert(!noexcept(basic_variant<TrivialAlloc<int>, int, string>(a)));
  }
  {
    basic_variant<TrivialAlloc<int>, int, char> a;
    static_assert(!noexcept(basic_variant<TrivialAlloc<int>, int, char>(a)));
  }
}

void move_ctor()
{
  static_assert(is_move_constructible_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(!is_move_constructible_v<basic_variant<TrivialAlloc<int>, AllDeleted, string>>);
  static_assert(is_move_constructible_v<basic_variant<TrivialAlloc<int>, int, DeletedMoves>>); // uses copy ctor
  static_assert(is_trivially_move_constructible_v<basic_variant<TrivialAlloc<int>, int>>);
  static_assert(!is_trivially_move_constructible_v<basic_variant<TrivialAlloc<int>, std::string>>);
  static_assert(!noexcept(basic_variant<TrivialAlloc<int>, int, Empty>(declval<basic_variant<TrivialAlloc<int>, int, Empty>>())));
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, DefaultNoexcept>(declval<basic_variant<TrivialAlloc<int>, int, DefaultNoexcept>>())));
}

void arbitrary_ctor()
{
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, string, string>, const char*>);
  static_assert(is_constructible_v<basic_variant<TrivialAlloc<int>, int, string>, const char*>);
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, Empty>(int{})));
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, DefaultNoexcept>(int{})));
  static_assert(!noexcept(basic_variant<TrivialAlloc<int>, int, Empty>(Empty{})));
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, DefaultNoexcept>(DefaultNoexcept{})));

  // P0608R3 disallow narrowing conversions and boolean conversions
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, float>, int>);
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, float, vector<int>>, int>);
  static_assert(is_constructible_v<basic_variant<TrivialAlloc<int>, float, int>, char>);
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, float, char>, int>);
  static_assert(is_constructible_v<basic_variant<TrivialAlloc<int>, float, long>, int>);
  struct big_int { big_int(int) { } };
  static_assert(is_constructible_v<basic_variant<TrivialAlloc<int>, float, big_int>, int>);

  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, int>, unsigned>);
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, bool>, int>);
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, bool>, void*>);

  // P1957R2 Converting from T* to bool should be considered narrowing
  struct ConvertibleToBool
  {
    operator bool() const { return true; }
  };
  static_assert(is_constructible_v<basic_variant<TrivialAlloc<int>, bool>, ConvertibleToBool>);
  static_assert(is_constructible_v<basic_variant<TrivialAlloc<int>, bool, int>, ConvertibleToBool>);
}

struct None { None() = delete; };
struct Any { template <typename T> Any(T&&) {} };

void in_place_index_ctor()
{
  basic_variant<TrivialAlloc<int>, string, string> a(in_place_index<0>, "a");
  basic_variant<TrivialAlloc<int>, string, string> b(in_place_index<1>, {'a'});

  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, None, Any>, std::in_place_index_t<0>>, "PR libstdc++/90165");
}

void in_place_type_ctor()
{
  basic_variant<TrivialAlloc<int>, int, string, int> a(in_place_type<string>, "a");
  basic_variant<TrivialAlloc<int>, int, string, int> b(in_place_type<string>, {'a'});
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, string, string>, in_place_type_t<string>, const char*>);
  static_assert(!is_constructible_v<basic_variant<TrivialAlloc<int>, None, Any>, std::in_place_type_t<None>>, "PR libstdc++/90165");
}

void dtor()
{
  static_assert(is_destructible_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(is_destructible_v<basic_variant<TrivialAlloc<int>, AllDeleted, string>>);
}

void copy_assign()
{
  static_assert(is_copy_assignable_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(!is_copy_assignable_v<basic_variant<TrivialAlloc<int>, AllDeleted, string>>);
  static_assert(!is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int>, int>>);
  static_assert(!is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int>, string>>);
  static_assert(!is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int>, DeletedMoves>>);
  {
    basic_variant<TrivialAlloc<int>, Empty> a;
    static_assert(!noexcept(a = a));
  }
  {
    basic_variant<TrivialAlloc<int>, DefaultNoexcept> a;
    static_assert(!noexcept(a = a));
  }
}

void move_assign()
{
  static_assert(is_move_assignable_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(!is_move_assignable_v<basic_variant<TrivialAlloc<int>, AllDeleted, string>>);
  static_assert(is_move_assignable_v<basic_variant<TrivialAlloc<int>, int, DeletedMoves>>); // uses copy assignment
  static_assert(!is_trivially_move_assignable_v<basic_variant<TrivialAlloc<int>, int>>);
  static_assert(!is_trivially_move_assignable_v<basic_variant<TrivialAlloc<int>, string>>);
  {
    basic_variant<TrivialAlloc<int>, Empty> a;
    static_assert(!noexcept(a = std::move(a)));
  }
  {
    basic_variant<TrivialAlloc<int>, DefaultNoexcept> a;
    static_assert(!noexcept(a = std::move(a)));
  }
}

void arbitrary_assign()
{
  static_assert(!is_assignable_v<basic_variant<TrivialAlloc<int>, string, string>, const char*>);
  static_assert(is_assignable_v<basic_variant<TrivialAlloc<int>, int, string>, const char*>);
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, Empty>() = int{}));
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, DefaultNoexcept>() = int{}));
  static_assert(!noexcept(basic_variant<TrivialAlloc<int>, int, Empty>() = Empty{}));
  static_assert(noexcept(basic_variant<TrivialAlloc<int>, int, DefaultNoexcept>() = DefaultNoexcept{}));
}

void test_get()
{
  static_assert(is_same<decltype(get<0>(basic_variant<TrivialAlloc<int>, int, string>())), int&&>::value);
  static_assert(is_same<decltype(get<1>(basic_variant<TrivialAlloc<int>, int, string>())), string&&>::value);
  static_assert(is_same<decltype(get<1>(basic_variant<TrivialAlloc<int>, int, const string>())), const string&&>::value);

  static_assert(is_same<decltype(get<int>(basic_variant<TrivialAlloc<int>, int, string>())), int&&>::value);
  static_assert(is_same<decltype(get<string>(basic_variant<TrivialAlloc<int>, int, string>())), string&&>::value);
  static_assert(is_same<decltype(get<const string>(basic_variant<TrivialAlloc<int>, int, const string>())), const string&&>::value);
}

void test_relational()
{
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(42), b(43);
    static_assert((a < b));
    static_assert(!(a > b));
    static_assert((a <= b));
    static_assert(!(a == b));
    static_assert((a != b));
    static_assert(!(a >= b));
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(42), b(42);
    static_assert(!(a < b));
    static_assert(!(a > b));
    static_assert((a <= b));
    static_assert((a == b));
    static_assert(!(a != b));
    static_assert((a >= b));
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(43), b(42);
    static_assert(!(a < b));
    static_assert((a > b));
    static_assert(!(a <= b));
    static_assert(!(a == b));
    static_assert((a != b));
    static_assert((a >= b));
  }
  {
    constexpr monostate a, b;
    static_assert(!(a < b));
    static_assert(!(a > b));
    static_assert((a <= b));
    static_assert((a == b));
    static_assert(!(a != b));
    static_assert((a >= b));
  }
}

// Not swappable, and basic_variant<TrivialAlloc<int>, C> not swappable via the generic std::swap.
struct C { };
void swap(C&, C&) = delete;

static_assert( !std::is_swappable_v<basic_variant<TrivialAlloc<int>, C>> );
static_assert( !std::is_swappable_v<basic_variant<TrivialAlloc<int>, int, C>> );
static_assert( !std::is_swappable_v<basic_variant<TrivialAlloc<int>, C, int>> );

// Not swappable, and basic_variant<TrivialAlloc<int>, D> not swappable via the generic std::swap.
struct D { D(D&&) = delete; };

static_assert( !std::is_swappable_v<basic_variant<TrivialAlloc<int>, D>> );
static_assert( !std::is_swappable_v<basic_variant<TrivialAlloc<int>, int, D>> );
static_assert( !std::is_swappable_v<basic_variant<TrivialAlloc<int>, D, int>> );

void test_swap()
{
  static_assert(is_swappable_v<basic_variant<TrivialAlloc<int>, int, string>>);
  static_assert(!is_swappable_v<basic_variant<TrivialAlloc<int>, MoveCtorOnly>>);
  static_assert(is_swappable_v<basic_variant<TrivialAlloc<int>, MoveCtorAndSwapOnly>>);
  static_assert(!is_swappable_v<basic_variant<TrivialAlloc<int>, AllDeleted>>);
}

void test_visit()
{
  {
    struct Visitor
    {
      void operator()(monostate) {}
      void operator()(const int&) {}
    };
    struct CVisitor
    {
      void operator()(monostate) const {}
      void operator()(const int&) const {}
    };
  }
  {
    struct Visitor
    {
      bool operator()(int, float) { return false; }
      bool operator()(int, double) { return false; }
      bool operator()(char, float) { return false; }
      bool operator()(char, double) { return false; }
    };
    visit(Visitor(), basic_variant<TrivialAlloc<int>, int, char>(), basic_variant<TrivialAlloc<int>, float, double>());
  }
  {
    struct Visitor
    {
      constexpr bool operator()(const int&) { return true; }
      constexpr bool operator()(const nonliteral&) { return false; }
    };
    static_assert(visit(Visitor(), basic_variant<TrivialAlloc<int>, int, nonliteral>(0)));
  }
  {
    struct Visitor
    {
      constexpr bool operator()(const int&) { return true; }
      constexpr bool operator()(const nonliteral&) { return false; }
    };
    static_assert(visit(Visitor(), basic_variant<TrivialAlloc<int>, int, nonliteral>(0)));
  }
  // PR libstdc++/79513
  {
    std::basic_variant<TrivialAlloc<int>, int> v [[gnu::unused]] (5);
    std::visit([](int&){}, v);
    std::visit([](int&&){}, std::move(v));
  }
}

void test_constexpr()
{
  constexpr basic_variant<TrivialAlloc<int>, int> a;
  static_assert(holds_alternative<int>(a));
  constexpr basic_variant<TrivialAlloc<int>, int, char> b(in_place_index<0>, int{});
  static_assert(holds_alternative<int>(b));
  constexpr basic_variant<TrivialAlloc<int>, int, char> c(in_place_type<int>, int{});
  static_assert(holds_alternative<int>(c));
  constexpr basic_variant<TrivialAlloc<int>, int, char> d(in_place_index<1>, char{});
  static_assert(holds_alternative<char>(d));
  constexpr basic_variant<TrivialAlloc<int>, int, char> e(in_place_type<char>, char{});
  static_assert(holds_alternative<char>(e));
  constexpr basic_variant<TrivialAlloc<int>, int, char> f(char{});
  static_assert(holds_alternative<char>(f));

  {
    struct literal {
	constexpr literal() = default;
    };

    constexpr basic_variant<TrivialAlloc<int>, literal, nonliteral> v{};
    constexpr basic_variant<TrivialAlloc<int>, literal, nonliteral> v1{in_place_type<literal>};
    constexpr basic_variant<TrivialAlloc<int>, literal, nonliteral> v2{in_place_index<0>};
  }

  {
    constexpr basic_variant<TrivialAlloc<int>, int> a(42);
    static_assert(get<0>(a) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(42);
    static_assert(get<0>(a) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, nonliteral, int> a(42);
    static_assert(get<1>(a) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int> a(42);
    static_assert(get<int>(a) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(42);
    static_assert(get<int>(a) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, nonliteral, int> a(42);
    static_assert(get<int>(a) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int> a(42);
    static_assert(get<0>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(42);
    static_assert(get<0>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, nonliteral, int> a(42);
    static_assert(get<1>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int> a(42);
    static_assert(get<int>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, int, nonliteral> a(42);
    static_assert(get<int>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<TrivialAlloc<int>, nonliteral, int> a(42);
    static_assert(get<int>(std::move(a)) == 42);
  }
}

void test_pr77641()
{
  struct X {
    constexpr X() { }
  };

  constexpr std::basic_variant<TrivialAlloc<int>, X> v1 = X{};
}

namespace adl_trap
{
  struct X {
    X() = default;
    X(int) { }
    X(std::initializer_list<int>, const X&) { }
  };
  template<typename T> void move(T&) { }
  template<typename T> void forward(T&) { }

  struct Visitor {
    template<typename T> void operator()(T&&) { }
  };
}

void test_adl()
{
   using adl_trap::X;
   X x;
   std::initializer_list<int> il;
   adl_trap::Visitor vis;

   std::basic_variant<TrivialAlloc<int>, X> v0(x);
   v0 = x;
   v0.emplace<0>(x);
   v0.emplace<0>(il, x);
   visit(vis, v0);
   basic_variant<TrivialAlloc<int>, X> v1{in_place_index<0>, x};
   basic_variant<TrivialAlloc<int>, X> v2{in_place_type<X>, x};
   basic_variant<TrivialAlloc<int>, X> v3{in_place_index<0>, il, x};
   basic_variant<TrivialAlloc<int>, X> v4{in_place_type<X>, il, x};
}

void test_variant_alternative()
{
  static_assert(is_same_v<variant_alternative_t<0, basic_variant<TrivialAlloc<int>, int, string>>, int>);
  static_assert(is_same_v<variant_alternative_t<1, basic_variant<TrivialAlloc<int>, int, string>>, string>);

  static_assert(is_same_v<variant_alternative_t<0, const basic_variant<TrivialAlloc<int>, int>>, const int>);
  static_assert(is_same_v<variant_alternative_t<0, volatile basic_variant<TrivialAlloc<int>, int>>, volatile int>);
  static_assert(is_same_v<variant_alternative_t<0, const volatile basic_variant<TrivialAlloc<int>, int>>, const volatile int>);
}

template<typename V, typename T>
  constexpr auto has_type_emplace(int) -> decltype((declval<V>().template emplace<T>(), true))
  { return true; };

template<typename V, typename T>
  constexpr bool has_type_emplace(...)
  { return false; };

template<typename V, size_t N>
  constexpr auto has_index_emplace(int) -> decltype((declval<V>().template emplace<N>(), true))
  { return true; };

template<typename V, size_t T>
  constexpr bool has_index_emplace(...)
  { return false; };

void test_emplace()
{
  static_assert(has_type_emplace<basic_variant<TrivialAlloc<int>, int>, int>(0));
  static_assert(!has_type_emplace<basic_variant<TrivialAlloc<int>, long>, int>(0));
  static_assert(has_index_emplace<basic_variant<TrivialAlloc<int>, int>, 0>(0));
  static_assert(!has_type_emplace<basic_variant<TrivialAlloc<int>, AllDeleted>, AllDeleted>(0));
  static_assert(!has_index_emplace<basic_variant<TrivialAlloc<int>, AllDeleted>, 0>(0));
  static_assert(has_type_emplace<basic_variant<TrivialAlloc<int>, int, AllDeleted>, int>(0));
  static_assert(has_index_emplace<basic_variant<TrivialAlloc<int>, int, AllDeleted>, 0>(0));
  static_assert(has_type_emplace<basic_variant<TrivialAlloc<int>, int, vector<int>, AllDeleted>, vector<int>>(0));
  static_assert(has_index_emplace<basic_variant<TrivialAlloc<int>, int, vector<int>, AllDeleted>, 1>(0));

  // The above tests only check the emplace members are available for
  // overload resolution. The following odr-uses will instantiate them:
  basic_variant<TrivialAlloc<int>, int, vector<int>, AllDeleted> v;
  v.emplace<0>(1);
  v.emplace<int>(1);
  v.emplace<1>(1, 1);
  v.emplace<vector<int>>(1, 1);
  v.emplace<1>({1, 2, 3, 4});
  v.emplace<vector<int>>({1, 2, 3, 4});
}

void test_triviality()
{

#define TEST_TEMPLATE(DT, CC, MC, CA, MA, CC_VAL, MC_VAL, CA_VAL, MA_VAL) \
  { \
    struct A \
    { \
      ~A() DT; \
      A(const A&) CC; \
      A(A&&) MC; \
      A& operator=(const A&) CA; \
      A& operator=(A&&) MA; \
    }; \
    static_assert(CC_VAL == is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int, false, false, false, true>, A>>); \
    static_assert(MC_VAL == is_trivially_move_constructible_v<basic_variant<TrivialAlloc<int, false, false, false, true>, A>>); \
    static_assert(CA_VAL == is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int, false, false, false, true>, A>>); \
    static_assert(MA_VAL == is_trivially_move_assignable_v<basic_variant<TrivialAlloc<int, false, false, false, true>, A>>); \
  }
  TEST_TEMPLATE(=default, =default, =default, =default, =default,  true,  true,  true,  true)
  TEST_TEMPLATE(=default, =default, =default, =default,         ,  true,  true,  true, false)
  TEST_TEMPLATE(=default, =default, =default,         , =default,  true,  true, false,  true)
  TEST_TEMPLATE(=default, =default, =default,         ,         ,  true,  true, false, false)
  TEST_TEMPLATE(=default, =default,         , =default, =default,  true, false,  true, false)
  TEST_TEMPLATE(=default, =default,         , =default,         ,  true, false,  true, false)
  TEST_TEMPLATE(=default, =default,         ,         , =default,  true, false, false, false)
  TEST_TEMPLATE(=default, =default,         ,         ,         ,  true, false, false, false)
  TEST_TEMPLATE(=default,         , =default, =default, =default, false,  true, false,  true)
  TEST_TEMPLATE(=default,         , =default, =default,         , false,  true, false, false)
  TEST_TEMPLATE(=default,         , =default,         , =default, false,  true, false,  true)
  TEST_TEMPLATE(=default,         , =default,         ,         , false,  true, false, false)
  TEST_TEMPLATE(=default,         ,         , =default, =default, false, false, false, false)
  TEST_TEMPLATE(=default,         ,         , =default,         , false, false, false, false)
  TEST_TEMPLATE(=default,         ,         ,         , =default, false, false, false, false)
  TEST_TEMPLATE(=default,         ,         ,         ,         , false, false, false, false)
  TEST_TEMPLATE(        , =default, =default, =default, =default, false, false, false, false)
  TEST_TEMPLATE(        , =default, =default, =default,         , false, false, false, false)
  TEST_TEMPLATE(        , =default, =default,         , =default, false, false, false, false)
  TEST_TEMPLATE(        , =default, =default,         ,         , false, false, false, false)
  TEST_TEMPLATE(        , =default,         , =default, =default, false, false, false, false)
  TEST_TEMPLATE(        , =default,         , =default,         , false, false, false, false)
  TEST_TEMPLATE(        , =default,         ,         , =default, false, false, false, false)
  TEST_TEMPLATE(        , =default,         ,         ,         , false, false, false, false)
  TEST_TEMPLATE(        ,         , =default, =default, =default, false, false, false, false)
  TEST_TEMPLATE(        ,         , =default, =default,         , false, false, false, false)
  TEST_TEMPLATE(        ,         , =default,         , =default, false, false, false, false)
  TEST_TEMPLATE(        ,         , =default,         ,         , false, false, false, false)
  TEST_TEMPLATE(        ,         ,         , =default, =default, false, false, false, false)
  TEST_TEMPLATE(        ,         ,         , =default,         , false, false, false, false)
  TEST_TEMPLATE(        ,         ,         ,         , =default, false, false, false, false)
  TEST_TEMPLATE(        ,         ,         ,         ,         , false, false, false, false)
#undef TEST_TEMPLATE

#define TEST_TEMPLATE(POCCA, POCMA, POCS, IAE, CC_VAL, MC_VAL, CA_VAL, MA_VAL) \
  { \
    static_assert(CC_VAL == is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int,POCCA, POCMA, POCS, IAE >, int>>); \
    static_assert(MC_VAL == is_trivially_move_constructible_v<basic_variant<TrivialAlloc<int,POCCA, POCMA, POCS, IAE >, int>>); \
    static_assert(CA_VAL == is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int, POCCA, POCMA, POCS, IAE >, int>>); \
    static_assert(MA_VAL == is_trivially_move_assignable_v<basic_variant<TrivialAlloc<int, POCCA, POCMA, POCS, IAE >, int >>); \
  }

  TEST_TEMPLATE(false, false, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, false, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(false, true, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, true, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(false, false, true, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, false, true, true, true,  true,  true,  true)
  TEST_TEMPLATE(false, true, true, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, true, true, true, true,  true,  true,  true)

  TEST_TEMPLATE(false, false, false, false, false,  true,  false,  false)
  TEST_TEMPLATE(true, false, false, false, false,  true,  false,  false)
  TEST_TEMPLATE(false, true, false, false, false,  true,  false,  true)
  TEST_TEMPLATE(true, true, false, false, false,  true,  false,  true)
  TEST_TEMPLATE(false, false, true, false, false,  true,  false,  false)
  TEST_TEMPLATE(true, false, true, false, false,  true,  false,  false)
  TEST_TEMPLATE(false, true, true, false, false,  true,  false,  true)
  TEST_TEMPLATE(true, true, true, false, false,  true,  false,  true)
#undef TEST_TEMPLATE

#define TEST_TEMPLATE(CC, MC, CA, MA) \
  { \
    struct A \
    { \
      A(const A&) CC; \
      A(A&&) MC; \
      A& operator=(const A&) CA; \
      A& operator=(A&&) MA; \
    }; \
    static_assert(!is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int>, AllDeleted, A>>); \
    static_assert(!is_trivially_move_constructible_v<basic_variant<TrivialAlloc<int>, AllDeleted, A>>); \
    static_assert(!is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int>, AllDeleted, A>>); \
    static_assert(!is_trivially_move_assignable_v<basic_variant<TrivialAlloc<int>, AllDeleted, A>>); \
  }
  TEST_TEMPLATE(=default, =default, =default, =default)
  TEST_TEMPLATE(=default, =default, =default,         )
  TEST_TEMPLATE(=default, =default,         , =default)
  TEST_TEMPLATE(=default, =default,         ,         )
  TEST_TEMPLATE(=default,         , =default, =default)
  TEST_TEMPLATE(=default,         , =default,         )
  TEST_TEMPLATE(=default,         ,         , =default)
  TEST_TEMPLATE(=default,         ,         ,         )
  TEST_TEMPLATE(        , =default, =default, =default)
  TEST_TEMPLATE(        , =default, =default,         )
  TEST_TEMPLATE(        , =default,         , =default)
  TEST_TEMPLATE(        , =default,         ,         )
  TEST_TEMPLATE(        ,         , =default, =default)
  TEST_TEMPLATE(        ,         , =default,         )
  TEST_TEMPLATE(        ,         ,         , =default)
  TEST_TEMPLATE(        ,         ,         ,         )
#undef TEST_TEMPLATE

  static_assert(!is_trivially_copy_constructible_v<basic_variant<TrivialAlloc<int>, DefaultNoexcept, int, char, float, double>>);
  static_assert(is_trivially_move_constructible_v<basic_variant<TrivialAlloc<int>, DefaultNoexcept, int, char, float, double>>);
  static_assert(!is_trivially_copy_assignable_v<basic_variant<TrivialAlloc<int>, DefaultNoexcept, int, char, float, double>>);
  static_assert(!is_trivially_move_assignable_v<basic_variant<TrivialAlloc<int>, DefaultNoexcept, int, char, float, double>>);
}

void test_noexcept()
{

#define TEST_TEMPLATE(POCCA, POCMA, POCS, IAE, CC_VAL, MC_VAL, CA_VAL, MA_VAL) \
  { \
    static_assert(CC_VAL == is_nothrow_copy_constructible_v<basic_variant<TrivialAlloc<int,POCCA, POCMA, POCS, IAE >, int>>); \
    static_assert(MC_VAL == is_nothrow_move_constructible_v<basic_variant<TrivialAlloc<int,POCCA, POCMA, POCS, IAE >, int>>); \
    static_assert(CA_VAL == is_nothrow_copy_assignable_v<basic_variant<TrivialAlloc<int, POCCA, POCMA, POCS, IAE >, int>>); \
    static_assert(MA_VAL == is_nothrow_move_assignable_v<basic_variant<TrivialAlloc<int, POCCA, POCMA, POCS, IAE >, int >>); \
  }

  TEST_TEMPLATE(false, false, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, false, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(false, true, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, true, false, true, true,  true,  true,  true)
  TEST_TEMPLATE(false, false, true, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, false, true, true, true,  true,  true,  true)
  TEST_TEMPLATE(false, true, true, true, true,  true,  true,  true)
  TEST_TEMPLATE(true, true, true, true, true,  true,  true,  true)

  TEST_TEMPLATE(false, false, false, false, false,  true,  false,  false)
  TEST_TEMPLATE(true, false, false, false, false,  true,  false,  false)
  TEST_TEMPLATE(false, true, false, false, false,  true,  false,  true)
  TEST_TEMPLATE(true, true, false, false, false,  true,  false,  true)
  TEST_TEMPLATE(false, false, true, false, false,  true,  false,  false)
  TEST_TEMPLATE(true, false, true, false, false,  true,  false,  false)
  TEST_TEMPLATE(false, true, true, false, false,  true,  false,  true)
  TEST_TEMPLATE(true, true, true, false, false,  true,  false,  true)

#undef TEST_TEMPLATE

}
