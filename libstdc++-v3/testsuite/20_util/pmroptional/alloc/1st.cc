// { dg-options "-std=gnu++20 -fconcepts" }
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

#include <any>
#include "../../../../include/std/pmroptional"

using std::pmr::optional;

#include <testsuite_hooks.h>

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

void test01()
{

    static_assert(std::uses_allocator<
        optional<value_type>,
        std::pmr::polymorphic_allocator<void>>::value, "Error optional<Thingy>");

    static_assert(std::uses_allocator<
        optional<optional<value_type>>,
        std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");

  static_assert(!std::uses_allocator<
        optional<std::optional<value_type>>,
        std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");


    static_assert(!std::uses_allocator<
        optional<int>,
        std::pmr::polymorphic_allocator<void>>::value, "Error optional<Thingy>");

    static_assert(!std::uses_allocator<
        optional<optional<int>>,
        std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");

    static_assert(!std::uses_allocator<
        optional<std::optional<int>>,
        std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");


    static_assert(!std::uses_allocator<
          optional<std::optional<value_type>>,
          std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");


    static_assert(!std::uses_allocator<
        optional<std::any>,
        std::pmr::polymorphic_allocator<void>>::value, "Error optional<Thingy>");

    static_assert(!std::uses_allocator<
        optional<optional<std::any>>,
        std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");

    static_assert(!std::uses_allocator<
        optional<std::optional<std::any>>,
        std::pmr::polymorphic_allocator<void>>::value, "Error nested optional");



}

int main()
{
  optional<value_type> ov;
  ov = {};
  optional<int> oi;
  oi = {};

  test01();

}
