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
  int                   id{0};

  tracker1() { ++count; }

  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc) : alloc(_alloc){ ++count; }

  ~tracker1() { --count; }

  tracker1(tracker1 const&) { ++count; }
  tracker1(tracker1&&) { ++count; }
  tracker1(int) { ++count; }

  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, tracker1 const&) : alloc(_alloc){ ++count; }
  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, tracker1 &&) : alloc(_alloc){ ++count; }
  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, int _id) : alloc(_alloc), id{_id}{ ++count; }

  tracker1& operator=(tracker1 const&) = default;
  tracker1& operator=(tracker1&&) = default;

  static int count;
};
int tracker1::count = 0;




int main()
{
  // basic_optional of same type, different allocator
  {
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1; // { dg-error "conversion" }
  }

  // allocator extended copy construction
  {

    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, DummyAlloc1<tracker1>{}, o1}; // { dg-error "no matching" }

  }

  // basic_optional of convertible type, different allocator
  {
    std::basic_optional<int, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1; // { dg-error "conversion" }
  }

  // allocator extended copy construction
  {
    std::basic_optional<int, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, DummyAlloc1<tracker1>{}, o1}; // { dg-error "no matching" }

  }

  // basic_optional of same type, different allocator
  {
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1; // { dg-error "conversion" }
  }

  // allocator extended copy construction
  {

    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, DummyAlloc1<tracker1>{}, o1}; // { dg-error "no matching" }

  }

// move construction
  // basic_optional of same type, different allocator
  {
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1); // { dg-error "conversion" }
  }

  // allocator extended copy construction
  {

    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, DummyAlloc1<tracker1>{}, std::move(o1)}; // { dg-error "no matching" }

  }

  // basic_optional of convertible type, different allocator
  {
    std::basic_optional<int, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1); // { dg-error "conversion" }
  }

  // allocator extended copy construction
  {
    std::basic_optional<int, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, DummyAlloc1<tracker1>{}, std::move(o1)}; // { dg-error "no matching" }

  }

  // basic_optional of same type, different allocator
  {
    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1); // { dg-error "conversion" }
  }

  // allocator extended copy construction
  {

    std::basic_optional<tracker1, DummyAlloc2<tracker1>> o1;
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, DummyAlloc1<tracker1>{}, std::move(o1)}; // { dg-error "no matching" }

  }


  // { dg-prune-output "no type named 'type'" }

}
