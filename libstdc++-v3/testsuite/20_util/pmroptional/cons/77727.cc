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


struct NonTransferable
{
  int x;
  NonTransferable(int x) : x(x) {}
  NonTransferable(NonTransferable&&) = delete;
  NonTransferable& operator=(NonTransferable&&) = delete;
  operator int() {return x;}
};

struct NonTransferable_aa
{
  int x;
  NonTransferable_aa(int x) : x(x) {}
  NonTransferable_aa(NonTransferable_aa&&) = delete;
  NonTransferable_aa& operator=(NonTransferable_aa&&) = delete;
  operator int() {return x;}

  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  NonTransferable_aa(std::allocator_arg_t,allocator_type, int x)
  : NonTransferable_aa(x){}
};
template<typename T>
void
test01()
{
  std::pmr::optional<int> oi;
  std::pmr::optional<T> ot(std::move(oi));
  VERIFY(!ot);

  std::pmr::optional<int> oi2;
  std::pmr::optional<T> ot2(oi2);
  VERIFY(!ot);

  std::pmr::optional<int> oi3{42};
  std::pmr::optional<T> ot3(std::move(oi3));
  VERIFY(ot3 && *ot3 == 42);

  std::pmr::optional<int> oi4{666};
  std::pmr::optional<T> ot4(oi4);
  VERIFY(ot4 && *ot4 == 666);
}
int main()
{
  test01<NonTransferable>();
  test01<NonTransferable_aa>();
}
