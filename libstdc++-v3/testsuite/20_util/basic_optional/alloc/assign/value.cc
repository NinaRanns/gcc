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

  typedef std::false_type propagate_on_container_copy_assignment;
  typedef std::false_type propagate_on_container_move_assignment;

};
template<typename T>
bool operator==(DummyAlloc1<T> l, DummyAlloc1<T> r) { return l.id==r.id; }

template<typename T>
bool operator!=(DummyAlloc1<T> l, DummyAlloc1<T> r) { return l.id!=r.id; }

template<typename T>
struct DummyAlloc2
{

  typedef T value_type;
  int id{0};

  DummyAlloc2 select_on_container_copy_construction() const
  { return DummyAlloc2{id+2}; }

  typedef std::true_type propagate_on_container_copy_assignment;
  typedef std::true_type propagate_on_container_move_assignment;

};

template<typename T>
bool operator==(DummyAlloc2<T> l, DummyAlloc2<T> r) { return l.id==r.id; }

template<typename T>
bool operator!=(DummyAlloc2<T> l, DummyAlloc2<T> r) { return l.id!=r.id; }


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

struct tracker2
{

  typedef DummyAlloc2<tracker2> allocator_type;

  DummyAlloc2<tracker2> alloc{0};

  tracker2() { ++count; }

  tracker2(std::allocator_arg_t, DummyAlloc2<tracker2> _alloc) : alloc(_alloc){ ++count; }

  ~tracker2() { --count; }

  tracker2(tracker2 const&) { ++count; }
  tracker2(tracker2&&) { ++count; }

  tracker2(std::allocator_arg_t, DummyAlloc2<tracker2> _alloc, tracker2 const&) : alloc(_alloc){ ++count; }
  tracker2(std::allocator_arg_t, DummyAlloc2<tracker2> _alloc, tracker2 &&) : alloc(_alloc){ ++count; }

  tracker2& operator=(tracker2 const&) = default;
  tracker2& operator=(tracker2&&) = default;

  static int count;
};
int tracker2::count = 0;





int main()
{

  {
    DummyAlloc1<tracker1> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );

    DummyAlloc1<tracker1> testAlloc2{15};
    tracker1  val{std::allocator_arg, testAlloc2};

    VERIFY( !o1 );
    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
  }
  {
    DummyAlloc1<tracker1> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );

    tracker1   val{std::allocator_arg, testAlloc1};

    VERIFY( !o1 );
    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );


    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    o1.reset();
    VERIFY( !o1 );
    o1 = std::move(val);
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );


    o1 = std::move(val);
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
  }
  VERIFY( tracker1::count == 0 );

  {
    DummyAlloc2<tracker2> testAlloc1{5};
    std::basic_optional<tracker2, DummyAlloc2<tracker2>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );

    DummyAlloc2<tracker2> testAlloc2{15};
    tracker2   val{std::allocator_arg, testAlloc2};

    VERIFY( !o1 );
    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );


    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
  }
  {
    DummyAlloc2<tracker2> testAlloc1{5};
    std::basic_optional<tracker2, DummyAlloc2<tracker2>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );

    tracker2   val{std::allocator_arg, testAlloc1};

    VERIFY( !o1 );
    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );


    o1 = val;
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
  }
  VERIFY( tracker2::count == 0 );

}
