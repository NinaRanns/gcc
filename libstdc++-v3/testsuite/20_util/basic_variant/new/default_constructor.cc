// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

#include <testsuite_hooks.h>
#include "../../../../include/std/basic_variant.h"
#include "memory_resource"

using namespace std;
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
    int id=0;

    constexpr MyAlloc() = default;
    constexpr MyAlloc(int i) : id(i) {};

};

struct X {
  using allocator_type = MyAlloc<X>;
  constexpr X() = default;
  constexpr X(const allocator_type& a) : alloc(a) {};

  constexpr X(int i) : value(i){};
  constexpr X(std::allocator_arg_t, const allocator_type& a, int i):
          value(i), alloc(a) {};


  int value = 0;
  allocator_type alloc = allocator_type{-1};

};



struct AllDeleted
{
  AllDeleted() = delete;
  AllDeleted(const AllDeleted&) = delete;
  AllDeleted(AllDeleted&&) = delete;
  AllDeleted& operator=(const AllDeleted&) = delete;
  AllDeleted& operator=(AllDeleted&&) = delete;
};


struct DefaultNoexcept
{
  DefaultNoexcept() noexcept = default;
  DefaultNoexcept(const DefaultNoexcept&) noexcept = default;
  DefaultNoexcept(DefaultNoexcept&&) noexcept = default;
  DefaultNoexcept& operator=(const DefaultNoexcept&) noexcept = default;
  DefaultNoexcept& operator=(DefaultNoexcept&&) noexcept = default;
};


struct DeletedMoves
{
  DeletedMoves() = default;
  DeletedMoves(const DeletedMoves&) = default;
  DeletedMoves(DeletedMoves&&) = delete;
  DeletedMoves& operator=(const DeletedMoves&) = default;
  DeletedMoves& operator=(DeletedMoves&&) = delete;
};

struct Empty
{
  Empty() { };
  Empty(const Empty&) { };
  Empty(Empty&&) { };
  Empty& operator=(const Empty&) { return *this; };
  Empty& operator=(Empty&&) { return *this; };
};

struct virtual_default_dtor {
   virtual ~virtual_default_dtor() = default;
};
void compile_time()
{
  {
    constexpr basic_variant<MyAlloc<X>, int, X> obj;
    static_assert(get<int>(obj) == 0);
    static_assert(obj.get_allocator().id == 0);
  }
  {
    constexpr basic_variant<MyAlloc<X>, X> obj;
    static_assert(get<X>(obj).value == 0);
    static_assert(obj.get_allocator().id == 0);
    static_assert(get<X>(obj).alloc.id == 0);
  }
  {
    constexpr basic_variant<std::allocator<X>, X> obj;
    static_assert(get<X>(obj).value == 0);
    static_assert(get<X>(obj).alloc.id == -1);
  }

  static_assert(is_default_constructible_v<basic_variant<MyAlloc<X>, int, string>>);
  static_assert(is_default_constructible_v<basic_variant<MyAlloc<X>, string, string>>);
  static_assert(!is_default_constructible_v<basic_variant<MyAlloc<X>, AllDeleted, string>>);
  static_assert(is_default_constructible_v<basic_variant<MyAlloc<X>, string, AllDeleted>>);
  static_assert(is_default_constructible_v<basic_variant<MyAlloc<X>, DeletedMoves>>);

  static_assert(noexcept(basic_variant<MyAlloc<X>, int>()));
  static_assert(!noexcept(basic_variant<MyAlloc<X>, Empty>()));
  static_assert(noexcept(basic_variant<MyAlloc<X>, DefaultNoexcept>()));
  {
    basic_variant<MyAlloc<X>, virtual_default_dtor> a;
  }
}


int main()
{

  compile_time();


  {
    basic_variant<MyAlloc<int>, int, X> obj{allocator_arg, MyAlloc<int>{5}};
    VERIFY(get<int>(obj) == 0);
    VERIFY(obj.get_allocator().id == 5);
  }

  {
    basic_variant<MyAlloc<X>, X, int> obj{allocator_arg, MyAlloc<X>{5}};
    VERIFY(get<X>(obj).value == 0);
    VERIFY(obj.get_allocator().id == 5);
    VERIFY(get<X>(obj).alloc.id == 5);
  }

  {
    basic_variant<std::allocator<X>, X, int> obj{allocator_arg, std::allocator<X>{}};
    VERIFY(get<X>(obj).value == 0);
    VERIFY(get<X>(obj).alloc.id == -1);
  }


}

