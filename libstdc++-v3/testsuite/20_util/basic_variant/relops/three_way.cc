// Copyright (C) 2020-2022 Free Software Foundation, Inc.
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

// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 } }

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
    int id=0;
    constexpr MyAlloc() = default;
    constexpr MyAlloc(int i) : id(i) {};

    template<typename U>
    operator MyAlloc<U>(){ return MyAlloc<U>{id};}
};

void
test01()
{
  using V1 = std::basic_variant<std::allocator<int>, int, int>;
  using V2 = std::basic_variant<MyAlloc<int>, int, int>;
  constexpr auto I0 = std::in_place_index<0>;
  constexpr auto I1 = std::in_place_index<1>;

  static_assert( std::is_eq(V1{I0, 0} <=> V2{I0, 0})  );
  static_assert( std::is_eq(V1{I0, 1} <=> V2{I0, 1})  );

  static_assert( std::is_lt(V1{I0, 0} <=> V2{I1, 0})  );
  static_assert( std::is_lt(V1{I0, 1} <=> V2{I1, 0})  );

  static_assert( std::is_gt(V1{I0, 1} <=> V2{I0, 0})  );
  static_assert( std::is_gt(V1{I1, 0} <=> V2{I0, 1})  );

  static_assert( V1{I0, 0} == V2{I0, 0}  );
  static_assert( V1{I0, 0} != V2{I1, 0}  );
  static_assert( V1{I1, 0} != V2{I1, 1}  );
}

void
test02()
{
  using V1 = std::basic_variant<std::allocator<int>, int, int>;
  using V = std::basic_variant<std::allocator<int>, int, int>;
  constexpr auto I0 = std::in_place_index<0>;
  constexpr auto I1 = std::in_place_index<1>;

  static_assert( std::is_eq(V{I0, 0} <=> V{I0, 0})  );
  static_assert( std::is_eq(V{I0, 1} <=> V{I0, 1})  );

  static_assert( std::is_lt(V{I0, 0} <=> V{I1, 0})  );
  static_assert( std::is_lt(V{I0, 1} <=> V{I1, 0})  );

  static_assert( std::is_gt(V{I0, 1} <=> V{I0, 0})  );
  static_assert( std::is_gt(V{I1, 0} <=> V{I0, 1})  );

  static_assert( V{I0, 0} == V{I0, 0}  );
  static_assert( V{I0, 0} != V{I1, 0}  );
  static_assert( V{I1, 0} != V{I1, 1}  );
}
