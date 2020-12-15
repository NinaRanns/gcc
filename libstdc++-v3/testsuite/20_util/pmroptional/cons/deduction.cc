// Copyright (C) 2017-2018 Free Software Foundation, Inc.
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
// { dg-do compile }
#include <type_traits>
#include "../../../../include/std/pmroptional"

struct value_type
{
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

struct value_type_aa
{
  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  value_type_aa(std::allocator_arg_t,allocator_type)
  : value_type_aa(){}


  value_type_aa(std::allocator_arg_t,allocator_type, int i)
  : value_type_aa(i){}

  value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa const& other)
    : value_type_aa(other.i){}

  value_type_aa(){};

  value_type_aa(int _i) : i(_i){};

  value_type_aa(value_type_aa const& other): i(other.i)
      {};

  value_type_aa& operator=(value_type_aa const& other)
  {
    i = other.i;
    return *this;
  }
  int i = 0;
};

struct move_only
{
  move_only() = default;
  move_only(move_only&&);
  move_only& operator=(move_only&&);

  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  move_only(std::allocator_arg_t,allocator_type)
       : move_only(){}

  move_only(std::allocator_arg_t,allocator_type, move_only&& other)
         : move_only(std::move(other)){}
};
struct move_only_aa
{
  move_only_aa() = default;
  move_only_aa(move_only_aa&&);
  move_only_aa& operator=(move_only_aa&&);

  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  move_only_aa(std::allocator_arg_t,allocator_type)
       : move_only_aa(){}

  move_only_aa(std::allocator_arg_t,allocator_type, move_only_aa&& other)
         : move_only_aa(std::move(other)){}
};

template<typename T>
void
test01()
{
  std::pmr::optional x = T(5);
  static_assert(std::is_same_v<decltype(x), std::pmr::optional<T>>);
  T y = 42;
  std::pmr::optional x2 = y;
  static_assert(std::is_same_v<decltype(x2), std::pmr::optional<T>>);
  const T z = 666;
  std::pmr::optional x3 = z;
  static_assert(std::is_same_v<decltype(x3), std::pmr::optional<T>>);

  std::pmr::optional copy = x;
  static_assert(std::is_same_v<decltype(copy), std::pmr::optional<T>>);

  std::optional so = T();
  std::pmr::optional po_copy = so;
  static_assert(std::is_same_v<decltype(po_copy), std::pmr::optional<T>>);

  std::pmr::optional po = T();
  std::optional so_copy = po;
  static_assert(std::is_same_v<decltype(so_copy), std::optional<T>>);

}


template<typename T>
void
test02()
{
  std::pmr::optional mo = T();
  static_assert(std::is_same_v<decltype(mo), std::pmr::optional<T>>);

  std::pmr::optional move = std::move(mo);
  static_assert(std::is_same_v<decltype(move), std::pmr::optional<T>>);


  std::optional so = T();
  std::pmr::optional po_move = std::move(so);
  static_assert(std::is_same_v<decltype(po_move), std::pmr::optional<T>>);


  std::pmr::optional po = T();
  std::optional so_move = std::move(po);
  static_assert(std::is_same_v<decltype(so_move), std::optional<T>>);

}
int main()
{
  test01<int>();
  test01<value_type>();
  test01<value_type_aa>();

  test02<move_only>();
  test02<move_only_aa>();


}

