// Copyright (C) 2018-2022 Free Software Foundation, Inc.
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
// { dg-do run { target c++20 }  }

#include "../../../../include/std/basic_variant"
#include <testsuite_hooks.h>

void
test01()
{
  std::basic_variant<std::allocator<std::monostate>,std::monostate> v1, v2;
  std::swap(v1, v2);
}

void
test02()
{
  std::basic_variant<std::allocator<int>,int> v1{1}, v2{2};
  std::swap(v1, v2);
  VERIFY( std::get<0>(v1) == 2 );
  VERIFY( std::get<0>(v2) == 1 );
}

void
test03()
{
  std::basic_variant<std::allocator<int>,double, int> v1{1}, v2{2.3};
  std::swap(v1, v2);
  VERIFY( std::get<double>(v1) == 2.3 );
  VERIFY( std::get<int>(v2) == 1 );
}

int
main()
{
  test01();
  test02();
  test03();
}
