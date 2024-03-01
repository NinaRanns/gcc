// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }
#include <testsuite_hooks.h>
#include "../../../../include/std/basic_variant.h"

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



int main()
{
  std::basic_variant<MyAlloc<int>, int, char, long> a;
  VERIFY(holds_alternative<int>(a));

/*  TODO
 std::basic_variant<Alloc,int, char> b(in_place_index<0>, int{});
  VERIFY(holds_alternative<int>(b));
  std::basic_variant<Alloc,int, char> c(in_place_type<int>, int{});
  VERIFY(holds_alternative<int>(c));
  std::basic_variant<Alloc,int, char> d(in_place_index<1>, char{});
  VERIFY(holds_alternative<char>(d));
  std::basic_variant<Alloc,int, char> e(in_place_type<char>, char{});
  VERIFY(holds_alternative<char>(e));
  std::basic_variant<Alloc,int, char> f(char{});
  VERIFY(holds_alternative<char>(f));
  */
}

