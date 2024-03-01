// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

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

#include "../../../../include/std/basic_variant.h"
#include "memory_resource"
#include <testsuite_hooks.h>

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
    static int next_id;
    int id = next_id++;
};
template<typename _Tp>
int MyAlloc<_Tp>::next_id = 0;

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
  X(int i, const allocator_type&) : value(i){ constructor_called = _E_AA_constructor_called;};

  static void reset(){ X::constructor_called = _E_no_constructor_called;}
  static int constructor_called;

  int id = 0;
  int value = 0;
};
// using a static variable because get isn't implemented yet
int X::constructor_called = _E_no_constructor_called;
int main()
{

  using pmrAlloc = std::pmr::polymorphic_allocator<>;
  using MyAllocVar =  std::basic_variant<MyAlloc<X>, X>;
  using PmrVar = std::basic_variant<pmrAlloc, X>;

  static_assert(is_same<decltype(get<0>(std::basic_variant<MyAlloc<X>, X, int, const char>())), X&&>::value);
  static_assert(is_same<decltype(get<2>(std::basic_variant<MyAlloc<X>, X, int, const char>())), const char&&>::value);

  { //TOTO, check for non first alternative
      std::basic_variant<MyAlloc<X>, X, int> a{};
      VERIFY(get<0>(a).value == 0);
      VERIFY(get<X>(a).value == 0);
      try {
        get<1>(a) == 0;
        VERIFY(false);
      } catch (std::bad_variant_access &){

      } catch(...)
      {
        VERIFY(false);
      }

      try {
        get<int>(a) == 0;
        VERIFY(false);
      } catch (std::bad_variant_access &){

      } catch(...)
      {
        VERIFY(false);
      }
  }
}
