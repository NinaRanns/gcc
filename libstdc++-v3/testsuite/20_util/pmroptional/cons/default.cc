// { dg-options "-std=gnu++20" }
// { dg-do run }

// Copyright (C) 2013-2018 Free Software Foundation, Inc.
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

struct tracker
{
  tracker() { ++count; }
  ~tracker() { --count; }

  tracker(tracker const&) { ++count; }
  tracker(tracker&&) { ++count; }

  tracker& operator=(tracker const&) = default;
  tracker& operator=(tracker&&) = default;

  static int count;
};

int tracker::count = 0;


struct tracker_aa
{
  tracker_aa() { ++count; }
  ~tracker_aa() { --count; }

  tracker_aa(tracker_aa const&) { ++count; }
  tracker_aa(tracker_aa&&) { ++count; }


  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  tracker_aa(std::allocator_arg_t,allocator_type)
     : tracker_aa(){}
  tracker_aa(std::allocator_arg_t,allocator_type, const tracker_aa& other)
     : tracker_aa(other){}
  tracker_aa(std::allocator_arg_t,allocator_type, tracker_aa&& other)
       : tracker_aa(std::move(other)){}

  tracker_aa& operator=(tracker_aa const&) = default;
  tracker_aa& operator=(tracker_aa&&) = default;

  static int count;
};

int tracker_aa::count = 0;

template<typename T>
void test01()
{
  // [20.5.4.1] Constructors

  {
    std::pmr::optional<T> o;
    VERIFY( !o );
  }

  {
    std::pmr::optional<T> o {};
    VERIFY( !o );
  }

  {
    std::pmr::optional<T> o = {};
    VERIFY( !o );
  }

  VERIFY( T::count == 0 );
}
int main()
{
  test01<tracker>();
  test01<tracker_aa>();


}
