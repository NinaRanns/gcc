// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

#include <testsuite_hooks.h>
#include "../../../../include/std/basic_variant.h"
#include "memory_resource"

template<typename _Tp = void>
struct MyAlloc{
   /// The allocator type
    using allocator_type = MyAlloc<_Tp>;

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

    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

    static allocator_type
    select_on_container_copy_construction(const allocator_type& other) noexcept
    { return other; }
    /// @}

    /// Whether all instances of the allocator type compare equal.
    using is_always_equal = std::false_type;
};

enum {
    _E_no_constructor_called,
    _E_plain_constructor_called,
    _E_AA_constructor_called

};

struct X {
  using allocator_type = std::pmr::polymorphic_allocator<X>;
  X(){ constructor_called = _E_plain_constructor_called;};
  X(const X& _x) : id(_x.id){ constructor_called = _E_plain_constructor_called;};
  X(int i) : id(i){ constructor_called = _E_plain_constructor_called;};



  X(const allocator_type&){ constructor_called = _E_AA_constructor_called;};
  X(const X& _x, const allocator_type&) : id(_x.id){ constructor_called = _E_AA_constructor_called;};
  X(int i, const allocator_type&) : id(i){ constructor_called = _E_AA_constructor_called;};

  static void reset(){ X::constructor_called = _E_no_constructor_called;}
  static int constructor_called;

  int id = 0;
};
// using a static variable because get isn't implemented yet
int X::constructor_called = _E_no_constructor_called;

void test_constexpr()
{
  using namespace std;
  constexpr basic_variant<MyAlloc<int>, int> a;
  static_assert(holds_alternative<int>(a));
  constexpr basic_variant<MyAlloc<int>, int, char> b(in_place_index<0>, int{});
  static_assert(holds_alternative<int>(b));
  constexpr basic_variant<MyAlloc<int>, int, char> c(in_place_type<int>, int{});
  static_assert(holds_alternative<int>(c));
  constexpr basic_variant<MyAlloc<int>, int, char> d(in_place_index<1>, char{});
  static_assert(holds_alternative<char>(d));
  constexpr basic_variant<MyAlloc<int>, int, char> e(in_place_type<char>, char{});
  static_assert(holds_alternative<char>(e));
  constexpr basic_variant<MyAlloc<int>, int, char> f(char{});
  static_assert(holds_alternative<char>(f));

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

  {
    struct literal {
    constexpr literal() = default;
    };

    constexpr basic_variant<MyAlloc<int>, literal, nonliteral> v{};
    constexpr basic_variant<MyAlloc<int>, literal, nonliteral> v1{in_place_type<literal>};
    constexpr basic_variant<MyAlloc<int>, literal, nonliteral> v2{in_place_index<0>};
  }

  {
    constexpr basic_variant<MyAlloc<int>, int> a(42);
    static_assert(get<0>(a) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int, nonliteral> a(42);
    static_assert(get<0>(a) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, nonliteral, int> a(42);
    static_assert(get<1>(a) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int> a(42);
    static_assert(get<int>(a) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int, nonliteral> a(42);
    static_assert(get<int>(a) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, nonliteral, int> a(42);
    static_assert(get<int>(a) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int> a(42);
    static_assert(get<0>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int, nonliteral> a(42);
    static_assert(get<0>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, nonliteral, int> a(42);
    static_assert(get<1>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int> a(42);
    static_assert(get<int>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, int, nonliteral> a(42);
    static_assert(get<int>(std::move(a)) == 42);
  }
  {
    constexpr basic_variant<MyAlloc<int>, nonliteral, int> a(42);
    static_assert(get<int>(std::move(a)) == 42);
  }
}
int main()
{

  using pmrAlloc = std::pmr::polymorphic_allocator<>;
  using MyAllocVar =  std::basic_variant<MyAlloc<X>, X, int>;
  using PmrVar = std::basic_variant<pmrAlloc, X, int>;


  X x;
  X::reset();
  MyAllocVar copy1(x);
  VERIFY( X::constructor_called == _E_plain_constructor_called );

  X::reset();
  MyAllocVar copy2(std::allocator_arg, MyAlloc<X>{}, x);
  VERIFY( X::constructor_called == _E_plain_constructor_called );


  X::reset();
  PmrVar copy3(x);
  VERIFY( X::constructor_called == _E_AA_constructor_called );

  X::reset();
  PmrVar copy4(std::allocator_arg, pmrAlloc{}, x);
  VERIFY( X::constructor_called == _E_AA_constructor_called );

  {
      MyAllocVar obj(4);
      VERIFY( std::get<int>(obj) == 4 );
  }

  {
      MyAllocVar obj(std::allocator_arg, MyAlloc<X>{},5);
      VERIFY( std::get<int>(obj) == 5 );
  }

  {
      PmrVar obj(6);
      VERIFY( std::get<int>(obj) == 6 );
  }

  {
      PmrVar obj(std::allocator_arg, pmrAlloc{},7);
      VERIFY( std::get<int>(obj) == 7 );
  }



}

