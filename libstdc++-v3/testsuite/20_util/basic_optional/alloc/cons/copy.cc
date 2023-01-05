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


template<typename T>
struct DummyAlloc1
{

  typedef T value_type;
  int id{0};

  DummyAlloc1 select_on_container_copy_construction() const
  { return DummyAlloc1{id+1}; }

};

template<typename T>
struct DummyAlloc2
{

  typedef T value_type;
  int id{0};

  DummyAlloc2 select_on_container_copy_construction() const
  { return DummyAlloc2{id+2}; }
};

struct tracker1
{

  typedef DummyAlloc1<tracker1> allocator_type;

  DummyAlloc1<tracker1> alloc{0};

  tracker1() { ++count; }

  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc) : alloc(_alloc){ ++count; }

  ~tracker1() { --count; }

  tracker1(tracker1 const&) { ++count; }
  tracker1(tracker1&&) { ++count; }

  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, tracker1 const&) : alloc(_alloc){ ++count; }
  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, tracker1 &&) : alloc(_alloc){ ++count; }

  tracker1& operator=(tracker1 const&) = default;
  tracker1& operator=(tracker1&&) = default;

  static int count;
};
int tracker1::count = 0;




int main()
{
  // copy construction of AA type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    tracker1 obj;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc, obj};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 6 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 6 );
  }

  // copy construction of non AA type
  {
    DummyAlloc2<tracker1> testAlloc{5};
    tracker1 obj;
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1{std::allocator_arg, testAlloc, obj};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 0 );

    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 7 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 0 );
  }

  // allocator extended copy construction
  {
    DummyAlloc1<tracker1> testAlloc{5};
    tracker1 obj;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc, obj};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 15 );
  }

  // allocator extended copy construction of non AA type
  {
    DummyAlloc2<tracker1> testAlloc{5};
    tracker1 obj;
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1{std::allocator_arg, testAlloc, obj};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 0 );

    DummyAlloc2<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 0 );
  }

  VERIFY( tracker1::count == 0 );

}
