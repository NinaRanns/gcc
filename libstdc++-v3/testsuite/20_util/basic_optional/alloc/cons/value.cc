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

  DummyAlloc1(int _id = 0) : id{_id}{};

  template<typename U>
  DummyAlloc1(DummyAlloc1<U> other): id{other.id}{};

  DummyAlloc1 select_on_container_copy_construction() const
  { return DummyAlloc1{id+1}; }


};

template<typename T>
struct DummyAlloc2
{

  typedef T value_type;
  int id{0};

  DummyAlloc2(int _id = 0) : id{_id}{};

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

  tracker1(tracker1 const& other) : id{other.id}{ ++count; }
  tracker1(tracker1&& other) :id{other.id}{ ++count; }
  tracker1(int _id) : id{_id}{ ++count; }

  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, tracker1 const& other) : alloc(_alloc), id{other.id}{ ++count; }
  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, tracker1 && other) : alloc(_alloc), id{other.id}{ ++count; }
  tracker1(std::allocator_arg_t, DummyAlloc1<tracker1> _alloc, int _id) : alloc(_alloc), id{_id}{ ++count; }

  tracker1& operator=(tracker1 const&) = default;
  tracker1& operator=(tracker1&&) = default;

  static int count;
};
int tracker1::count = 0;


int main()
{
  // basic_optional of convertible type, same allocator
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 0 );

  }

  // basic_optional of same type, convertible allocator ?
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<tracker1, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, std::in_place};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 0 );

  }

  // basic_optional of convertible type, convertible allocator ?
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 0 );

  }

  // std::optional of same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::optional<tracker1> o1{std::in_place, std::allocator_arg, testAlloc, 3};
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 0 );
  }

  // std::optional of convertible type
  {
    std::optional<int> o1{3};

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = o1;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 0 );
  }

  // same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    tracker1 val{std::allocator_arg, testAlloc, 3};


    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o = val;
    VERIFY( o.get_allocator().id == 0 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 0 );
  }

  // convertible type
  {

    int val = 3;

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o = val;
    VERIFY( o.get_allocator().id == 0 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 0 );
  }


// allocator extended constructors
  // basic_optional of convertible type, same allocator
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 15 );
  }

  // basic_optional of same type, convertible allocator
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<tracker1, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, std::in_place};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 15 );

  }

  // basic_optional of convertible type, convertible allocator
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 15 );

  }

  // std::optional of same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::optional<tracker1> o1{std::in_place, std::allocator_arg, testAlloc, 3};
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 15 );
  }

  // std::optional of convertible type
  {
    std::optional<int> o1{3};

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, o1};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 15 );
  }

  // same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    tracker1 val{std::allocator_arg, testAlloc, 3};

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o{std::allocator_arg, testAlloc2, val};
    VERIFY( o.get_allocator().id == 15 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 15 );
  }

  // convertible type
  {
    int val = 3;

    DummyAlloc1<tracker1> testAlloc{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o{std::allocator_arg, testAlloc, val};
    VERIFY( o.get_allocator().id == 15 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 15 );
  }


//move construction

  // basic_optional of convertible type, same allocator
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1);
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 0 );

  }

  // basic_optional of same type, convertible allocator ?
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<tracker1, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, std::in_place};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1);;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 0 );

  }

  // basic_optional of convertible type, convertible allocator ?
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1);;
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 0 );

  }

  // std::optional of same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::optional<tracker1> o1{std::in_place, std::allocator_arg, testAlloc, 3};
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1);
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 0 );
  }

  // std::optional of convertible type
  {
    std::optional<int> o1{3};

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2 = std::move(o1);
    VERIFY( o2.get_allocator().id == 0 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 0 );
  }


  // same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    tracker1 val{std::allocator_arg, testAlloc, 3};


    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o = std::move(val);
    VERIFY( o.get_allocator().id == 0 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 0 );
  }

  // convertible type
  {

    int val = 3;

    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o = std::move(val);
    VERIFY( o.get_allocator().id == 0 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 0 );
  }


// allocator extended constructors
  // basic_optional of convertible type, same allocator
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, std::move(o1)};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 15 );
  }

  // basic_optional of same type, convertible allocator
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<tracker1, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, std::in_place};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, std::move(o1)};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 15 );

  }

  // basic_optional of convertible type, convertible allocator
  {
    DummyAlloc1<int> testAlloc{5};
    std::basic_optional<int, DummyAlloc1<int>> o1{std::allocator_arg, testAlloc, 3};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, std::move(o1)};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 15 );

  }

  // std::optional of same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    std::optional<tracker1> o1{std::in_place, std::allocator_arg, testAlloc, 3};
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, std::move(o1)};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 15 );
  }

  // std::optional of convertible type
  {
    std::optional<int> o1{3};

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o2{std::allocator_arg, testAlloc2, std::move(o1)};
    VERIFY( o2.get_allocator().id == 15 );
    VERIFY( o2 );
    VERIFY( o2.value().id == 3 );
    VERIFY( o2.value().alloc.id == 15 );
  }


  // same type
  {
    DummyAlloc1<tracker1> testAlloc{5};
    tracker1 val{std::allocator_arg, testAlloc, 3};

    DummyAlloc1<tracker1> testAlloc2{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o{std::allocator_arg, testAlloc2, std::move(val)};
    VERIFY( o.get_allocator().id == 15 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 15 );
  }

  // convertible type
  {
    int val = 3;

    DummyAlloc1<tracker1> testAlloc{15};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o{std::allocator_arg, testAlloc, std::move(val)};
    VERIFY( o.get_allocator().id == 15 );
    VERIFY( o );
    VERIFY( o.value().id == 3 );
    VERIFY( o.value().alloc.id == 15 );
  }

  VERIFY( tracker1::count == 0 );

}
