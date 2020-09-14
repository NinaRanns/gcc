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

  typedef std::true_type propagate_on_container_copy_assignment;
  typedef std::true_type  propagate_on_container_move_assignment;
};
template<typename T>
bool operator==(DummyAlloc1<T> l, DummyAlloc1<T> r) { return l.id==r.id; }

template<typename T>
bool operator!=(DummyAlloc1<T> l, DummyAlloc1<T> r) { return l.id!=r.id; }



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
  tracker1& operator=(int i) {
    id = i;
    return *this;
  }

  static int count;
};
int tracker1::count = 0;




int main()
{
// copy assign from basic_optional, same type, different allocator
  {
    DummyAlloc1<tracker1> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    DummyAlloc1<tracker1> testAlloc2{15};
    std::optional<tracker1> o2;

    VERIFY( !o1 );
    VERIFY( !o2 );
    o1 = o2;
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.emplace(std::allocator_arg, testAlloc2);
    VERIFY( !o1 );
    VERIFY( o2 );
    o1 = o2;
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1.value().alloc.id == 5 );  // checks allocator construction was
                                         // done

    VERIFY( o1 );
    VERIFY( o2 );
    o1 = o2;
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    // we don't check the o1.value() allocator as allocator propagation is not
    // specified for basic_optional for this assignment

    o2.reset();
    VERIFY( o1 );
    VERIFY( !o2 );
    o1 = o2;
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );
  }
// move assign from std::optional, same type, same allocator
  {
    DummyAlloc1<tracker1> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    DummyAlloc1<tracker1> testAlloc2{15};
    std::optional<tracker1> o2;

    VERIFY( !o1 );
    VERIFY( !o2 );
    o1 = std::move(o2);
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.emplace(std::allocator_arg, testAlloc2);
    VERIFY( !o1 );
    VERIFY( o2 );
    o1 = std::move(o2);
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1.value().alloc.id == 5 );

    VERIFY( o1 );
    VERIFY( o2 );
    o1 = std::move(o2);
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.reset();
    VERIFY( o1 );
    VERIFY( !o2 );
    o1 = std::move(o2);
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );
  }
// copy assign from std::optional, same type, convertible allocator
  {
    DummyAlloc1<char> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<char>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );

    DummyAlloc1<tracker1> testAlloc2{15};
    std::optional<tracker1> o2;

    VERIFY( !o1 );
    VERIFY( !o2 );
    o1 = o2;
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.emplace(std::allocator_arg, testAlloc2);
    VERIFY( !o1 );
    VERIFY( o2 );
    o1 = o2;
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1.value().alloc.id == 5 );

    VERIFY( o1 );
    VERIFY( o2 );
    o1 = o2;
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.reset();
    VERIFY( o1 );
    VERIFY( !o2 );
    o1 = o2;
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );
  }
// move assign from std::optional, same type, convertible allocator
  {
    DummyAlloc1<char> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<char>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    DummyAlloc1<tracker1> testAlloc2{15};
    std::optional<tracker1> o2;

    VERIFY( !o1 );
    VERIFY( !o2 );
    o1 = std::move(o2);
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.emplace(std::allocator_arg, testAlloc2);
    VERIFY( !o1 );
    VERIFY( o2 );
    o1 = std::move(o2);
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1.value().alloc.id == 5 );

    VERIFY( o1 );
    VERIFY( o2 );
    o1 = std::move(o2);
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.reset();
    VERIFY( o1 );
    VERIFY( !o2 );
    o1 = std::move(o2);
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );
  }
// copy assign from std::optional, convertible type
  {
    DummyAlloc1<tracker1> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    DummyAlloc1<tracker1> testAlloc2{15};
    std::optional<int> o2;

    VERIFY( !o1 );
    VERIFY( !o2 );
    o1 = o2;
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.emplace(4);
    VERIFY( !o1 );
    VERIFY( o2 );
    o1 = o2;
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1.value().alloc.id == 5 );

    VERIFY( o1 );
    VERIFY( o2 );
    o1 = o2;
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.reset();
    VERIFY( o1 );
    VERIFY( !o2 );
    o1 = o2;
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );
  }
// move assign from std::optional, convertible type
  {
    DummyAlloc1<tracker1> testAlloc1{5};
    std::basic_optional<tracker1, DummyAlloc1<tracker1>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    DummyAlloc1<tracker1> testAlloc2{15};
    std::optional<int> o2;

    VERIFY( !o1 );
    VERIFY( !o2 );
    o1 = std::move(o2);
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.emplace(4);
    VERIFY( !o1 );
    VERIFY( o2 );
    o1 = std::move(o2);
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1.value().alloc.id == 5 );

    VERIFY( o1 );
    VERIFY( o2 );
    o1 = std::move(o2);
    VERIFY( o1 );
    VERIFY( o1.get_allocator().id == 5 );

    o2.reset();
    VERIFY( o1 );
    VERIFY( !o2 );
    o1 = std::move(o2);
    VERIFY( !o1 );
    VERIFY( o1.get_allocator().id == 5 );
  }
  VERIFY( tracker1::count == 0 );

}
