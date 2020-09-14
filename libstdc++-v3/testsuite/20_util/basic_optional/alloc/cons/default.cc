// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 }  }

// Copyright (C) 2013-2020 Free Software Foundation, Inc.
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

#include "../../../../../include/std/basic_optional"
#include <testsuite_hooks.h>



struct DummyAlloc
{
  int id{0};;
};


struct tracker
{
  DummyAlloc alloc{0};

  tracker() { ++count; }
  tracker(std::allocator_arg_t, DummyAlloc _alloc) : alloc(_alloc){ ++count; }

  ~tracker() { --count; }

  tracker(tracker const&) { ++count; }
  tracker(tracker&&) { ++count; }


  tracker& operator=(tracker const&) = default;
  tracker& operator=(tracker&&) = default;

  static int count;
};

int tracker::count = 0;

struct X { };



int main()
{
  {
    std::basic_optional<tracker, DummyAlloc> o;
    VERIFY( o.get_allocator().id == 0 );
    VERIFY( !o );
  }

  {
    DummyAlloc testAlloc{5};
    std::basic_optional<tracker, DummyAlloc> o(std::allocator_arg, testAlloc);
    VERIFY( o.get_allocator().id == 5 );
    VERIFY( !o );
  }

  VERIFY( tracker::count == 0 );
}
