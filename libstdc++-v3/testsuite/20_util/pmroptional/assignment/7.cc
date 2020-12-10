// { dg-options "-std=gnu++20" }
// { dg-do run }

// Copyright (C) 2016-2018 Free Software Foundation, Inc.
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

#include <testsuite_hooks.h>
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
template<typename T>
void
test01()
{
  std::pmr::optional<T> o{666};
  VERIFY(o && o->i == 666);
  o.reset();
  VERIFY(!o);
  static_assert(noexcept(std::declval<std::pmr::optional<T>>().reset()));
}
int main()
{
  test01<value_type>();
  test01<value_type_aa>();
}
