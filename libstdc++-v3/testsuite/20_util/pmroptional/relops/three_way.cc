// Copyright (C) 2020 Free Software Foundation, Inc.
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

// { dg-options "-std=gnu++2a" }
// { dg-do run { target c++2a } }
#include <testsuite_hooks.h>
#include "../../../../include/std/pmroptional"

struct value_type
{

    typedef std::pmr::polymorphic_allocator<void> allocator_type;

    value_type(std::allocator_arg_t,allocator_type)
    : value_type(){}


    value_type(std::allocator_arg_t,allocator_type, int i)
    : value_type(i){}

    value_type(std::allocator_arg_t,allocator_type, value_type const& other)
      : value_type(other){}

    value_type(){};

    value_type(int _i) : i(_i){};

    value_type(value_type const& other): i(other.i)
        {};

    value_type& operator=(value_type const& other)
    {
      i = other.i;
      return *this;
    }
    int i = 0;
};

std::strong_ordering
operator<=>(value_type lhs, value_type rhs) noexcept
{ return lhs.i <=> rhs.i; }

std::strong_ordering
operator<=>(value_type lhs, int rhs) noexcept
{ return lhs.i <=> rhs; }

std::strong_ordering
operator<=>(int lhs, value_type rhs) noexcept
{ return lhs <=> rhs.i; }

bool
operator==(value_type const& lhs, value_type const& rhs)
{ return lhs.i == rhs.i; }

bool
operator!=(value_type const& lhs, value_type const& rhs)
{ return lhs.i != rhs.i; }

bool
operator<(value_type const& lhs, value_type const& rhs)
{ return lhs.i < rhs.i; }

bool
operator>(value_type const& lhs, value_type const& rhs)
{ return lhs.i > rhs.i; }

bool
operator<=(value_type const& lhs, value_type const& rhs)
{ return lhs.i <= rhs.i; }

bool
operator>=(value_type const& lhs, value_type const& rhs)
{ return lhs.i >= rhs.i; }

bool
operator==(int lhs, value_type const& rhs)
{ return lhs == rhs.i; }

bool
operator!=(int lhs, value_type const& rhs)
{ return lhs != rhs.i; }

bool
operator<(int lhs, value_type const& rhs)
{ return lhs < rhs.i; }

bool
operator>(int lhs, value_type const& rhs)
{ return lhs > rhs.i; }

bool
operator<=(int lhs, value_type const& rhs)
{ return lhs <= rhs.i; }

bool
operator>=(int lhs, value_type const& rhs)
{ return lhs >= rhs.i; }

bool
operator==(value_type const& lhs, int rhs)
{ return lhs.i == rhs; }

bool
operator!=(value_type const& lhs, int rhs)
{ return lhs.i != rhs; }

bool
operator<(value_type const& lhs, int rhs)
{ return lhs.i < rhs; }

bool
operator>(value_type const& lhs, int rhs)
{ return lhs.i > rhs; }

bool
operator<=(value_type const& lhs, int rhs)
{ return lhs.i <= rhs; }

bool
operator>=(value_type const& lhs, int rhs)
{ return lhs.i >= rhs; }


template<typename T, typename U>
void
test_a()
{

  VERIFY( std::is_eq(T{} <=> T{}) );
  VERIFY( std::is_lt(T{} <=> T{1}) );
  VERIFY( std::is_gt(T{1} <=> T{}) );
  VERIFY( std::is_eq(T{1} <=> T{1}) );
  VERIFY( std::is_lt(T{1} <=> T{2}) );

  VERIFY( std::is_eq(T{} <=> U{}) );
  VERIFY( std::is_lt(T{} <=> U{1}) );
  VERIFY( std::is_gt(T{1} <=> U{}) );
  VERIFY( std::is_eq(T{1} <=> U{1}) );
  VERIFY( std::is_lt(T{1} <=> U{2}) );

  VERIFY( std::is_eq(U{} <=> T{}) );
  VERIFY( std::is_lt(U{} <=> T{1}) );
  VERIFY( std::is_gt(U{1} <=> T{}) );
  VERIFY( std::is_eq(U{1} <=> T{1}) );
  VERIFY( std::is_lt(U{1} <=> T{2}) );


}
template<typename T, typename U>
void
test_b()
{

  VERIFY( T{} == T{} );
  VERIFY( T{} < T{1} );
  VERIFY( T{1} > T{} );
  VERIFY( T{1} == T{1} );
  VERIFY( T{1} != T{2} );
  VERIFY( T{1} < T{2} );

  VERIFY( T{} == U{} );
  VERIFY( T{} < U{1} );
  VERIFY( T{1} > U{} );
  VERIFY( T{1} == U{1} );
  VERIFY( T{1} != U{2} );
  VERIFY( T{1} < U{2} );

  VERIFY( U{} == T{} );
  VERIFY( U{} < T{1} );
  VERIFY( U{1} > T{} );
  VERIFY( U{1} == T{1} );
  VERIFY( U{1} != T{2} );
  VERIFY( U{1} < T{2} );

}
void
test01()
{

  test_a<std::pmr::optional<int>, std::pmr::optional<short>>();
  test_a<std::pmr::optional<int>, std::optional<short>>();

  test_a<int, value_type>(); // for sanity purposes
  test_a<std::pmr::optional<int>, std::pmr::optional<value_type>>();
  test_a<std::pmr::optional<int>, std::optional<value_type>>();
  test_a<std::pmr::optional<value_type>, std::optional<int>>();

  test_b<std::pmr::optional<int>, std::pmr::optional<short>>();
  test_b<std::pmr::optional<int>, std::optional<short>>();

  test_b<int, value_type>(); // for sanity purposes
  test_b<std::pmr::optional<int>, std::pmr::optional<value_type>>();
  test_b<std::pmr::optional<int>, std::optional<value_type>>();
  test_b<std::pmr::optional<value_type>, std::optional<int>>();


  // Would requires narrowing conversion to make operands the same type:
  static_assert( !std::three_way_comparable_with<std::pmr::optional<int>,
      std::pmr::optional<unsigned>> );
  static_assert( !std::three_way_comparable_with<std::pmr::optional<int>,
        std::optional<unsigned>> );
  static_assert( !std::three_way_comparable_with<std::optional<int>,
        std::pmr::optional<unsigned>> );

}

void
test02()
{
  using O = std::pmr::optional<int>;
  using std::nullopt;

  static_assert( std::is_eq(O{} <=> nullopt) );
  static_assert( std::is_gt(O{1} <=> nullopt) );
  static_assert( std::is_lt(nullopt <=> O{1}) );

  static_assert( O{} == nullopt );
  static_assert( O{1} != nullopt );
  static_assert( nullopt != O{1} );
  static_assert( O{1} > nullopt );
  static_assert( nullopt < O{1} );
  static_assert( nullopt <= O{} );
  static_assert( nullopt <= O{1} );
}

int main()
{
  test01();
  test02();
}
