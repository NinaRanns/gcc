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

    template<typename U>
    constexpr MyAlloc(const MyAlloc<U>& other): id(other.id){ }
};



struct X {
  using allocator_type = MyAlloc<X>;
  constexpr X() = default;
  constexpr X(std::allocator_arg_t, const allocator_type& a) : alloc(a) {};

  constexpr X(int i) : value(i){};
  constexpr X(std::allocator_arg_t, const allocator_type& a, int i):
          value(i), alloc(a) {};

  constexpr X(const X& other) : value(other.value), alloc(other.alloc){};
  constexpr X(std::allocator_arg_t, const allocator_type& a, const X& other):
          value(other.value), alloc(a) {};



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
    constexpr basic_variant<MyAlloc<X>, int, X> copy = obj;
    static_assert(get<int>(copy) == 0);
    static_assert(copy.get_allocator().id == 0);
  }
  {
    constexpr basic_variant<MyAlloc<X>, X> obj;
    constexpr basic_variant<MyAlloc<X>, X> copy = obj;

    static_assert(get<X>(copy).value == 0);
    static_assert(copy.get_allocator().id == 0);
    static_assert(get<X>(copy).alloc.id == 0);
  }
  {
    constexpr basic_variant<std::allocator<X>, X> obj;
    constexpr basic_variant<std::allocator<X>, X> copy = obj;

    static_assert(get<X>(copy).value == 0);
    static_assert(get<X>(copy).alloc.id == -1);
  }

  static_assert(is_copy_constructible_v<basic_variant<MyAlloc<X>, int, string>>);
  static_assert(is_copy_constructible_v<basic_variant<MyAlloc<X>, string, string>>);
  static_assert(!is_copy_constructible_v<basic_variant<MyAlloc<X>, string, AllDeleted>>);
  static_assert(is_copy_constructible_v<basic_variant<MyAlloc<X>, DeletedMoves>>);


  static_assert(!is_trivially_copy_constructible_v<basic_variant<MyAlloc<X>, int>>);
  static_assert(!is_trivially_copy_constructible_v<basic_variant<MyAlloc<X>, X>>);
  static_assert(!is_trivially_copy_constructible_v<basic_variant<std::allocator<X>, X, int>>);
  static_assert(!is_trivially_copy_constructible_v<basic_variant<std::allocator<int>, char, int>>);


  {
    constexpr basic_variant<MyAlloc<X>, int, X> obj;
    constexpr basic_variant<MyAlloc<int>, int, X> copy = obj;
    static_assert(get<int>(copy) == 0);
    static_assert(copy.get_allocator().id == 0);
  }
  {
    constexpr basic_variant<MyAlloc<X>, X> obj;
    constexpr basic_variant<MyAlloc<int>, X> copy = obj;

    static_assert(get<X>(copy).value == 0);
    static_assert(copy.get_allocator().id == 0);
    static_assert(get<X>(copy).alloc.id == 0);
  }
  {
    constexpr basic_variant<std::allocator<X>, X> obj;
    constexpr basic_variant<std::allocator<int>, X> copy = obj;

    static_assert(get<X>(copy).value == 0);
    static_assert(get<X>(copy).alloc.id == -1);
  }

  static_assert(is_constructible_v<basic_variant<MyAlloc<X>, int, string>, basic_variant<MyAlloc<int>, int, string>&>);
  static_assert(is_constructible_v<basic_variant<MyAlloc<X>, string, string>, basic_variant<MyAlloc<int>, string, string>& >);
  static_assert(!is_constructible_v<basic_variant<MyAlloc<X>, string, AllDeleted>, basic_variant<MyAlloc<int>, string, AllDeleted>& >);
  static_assert(is_constructible_v<basic_variant<MyAlloc<X>, DeletedMoves>, basic_variant<MyAlloc<int>, DeletedMoves>&>);

  static_assert(!is_constructible_v<basic_variant<MyAlloc<X>, int, string>, basic_variant<std::pmr::polymorphic_allocator<int>, int, string>&>);
  static_assert(!is_constructible_v<basic_variant<MyAlloc<X>, string, string>, basic_variant<std::pmr::polymorphic_allocator<int>, string, string>& >);
  static_assert(!is_constructible_v<basic_variant<MyAlloc<X>, string, AllDeleted>, basic_variant<std::pmr::polymorphic_allocator<int>, string, AllDeleted>& >);
  static_assert(!is_constructible_v<basic_variant<MyAlloc<X>, DeletedMoves>, basic_variant<std::pmr::polymorphic_allocator<int>, DeletedMoves>&>);

}

int main()
{

  compile_time();

  using pmrAlloc = std::pmr::polymorphic_allocator<>;
  using MyAllocVar =  std::basic_variant<MyAlloc<X>, X, int>;
  using PmrVar = std::basic_variant<pmrAlloc, X>;

  {
    MyAllocVar obj{std::allocator_arg, MyAlloc<int>{5}};

    MyAllocVar copy1(obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == 5);
    VERIFY(copy1.get_allocator().id == 5);
  }

  {
    MyAllocVar obj{std::allocator_arg, MyAlloc<X>{5}};

    MyAllocVar copy1(obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == 5);
    VERIFY(copy1.get_allocator().id == 5);
  }

  {
    PmrVar obj;

    PmrVar copy1(obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == -1);
  }


  {
    MyAllocVar obj{std::allocator_arg, MyAlloc<int>{5}};

    MyAllocVar copy1(std::allocator_arg, MyAlloc<int>{6},obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == 6);
    VERIFY(copy1.get_allocator().id == 6);
  }

  {
    MyAllocVar obj{std::allocator_arg, MyAlloc<X>{5}};

    MyAllocVar copy1(std::allocator_arg, MyAlloc<X>{6},obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == 6);
    VERIFY(copy1.get_allocator().id == 6);
  }

  {
    std::basic_variant<MyAlloc<int>, X, int> obj{std::allocator_arg, MyAlloc<int>{5}};

    MyAllocVar copy1(std::allocator_arg, MyAlloc<X>{6},obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == 6);
    VERIFY(copy1.get_allocator().id == 6);
  }

  {
    PmrVar obj;

    PmrVar copy1(std::allocator_arg, pmrAlloc{}, obj);
    VERIFY(get<X>(copy1).value == 0);
    VERIFY(get<X>(copy1).alloc.id == -1);
  }
  {
    std::basic_variant<MyAlloc<X>, X, int> obj1;
    std::basic_variant<MyAlloc<int>, X, int>  obj2 = obj1;
  }


}

