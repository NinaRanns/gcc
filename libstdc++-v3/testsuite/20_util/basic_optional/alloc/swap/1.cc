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

struct exception {};

template<typename T>
struct DummyAlloc1
{

  typedef T value_type;
  int id{0};

  DummyAlloc1 select_on_container_copy_construction() const
  { return DummyAlloc1{id+1}; }

  typedef std::true_type propagate_on_container_swap;
  typedef std::true_type           is_always_equal;

  typedef std::false_type propagate_on_container_copy_assignment;
  typedef std::false_type propagate_on_container_move_assignment;

};

int counter = 0;

struct mixin_counter
{
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const&) { ++counter; }
  ~mixin_counter() { --counter; }
};

namespace ns
{

struct value_type : private mixin_counter
{

  int state;
  DummyAlloc1<value_type> alloc;
  typedef DummyAlloc1<value_type> allocator_type;

  value_type(int val = 0): state(val){}
  value_type(const value_type& other): state(other.state){}

  value_type(std::allocator_arg_t, DummyAlloc1<value_type> _alloc, int val = 0): state(val), alloc(_alloc){}
  value_type(std::allocator_arg_t, DummyAlloc1<value_type> _alloc, const value_type& other): state(other.state), alloc(_alloc){}


};

int swaps = 0;

void
swap(value_type& lhs, value_type& rhs)
{
  ++swaps;
  using std::swap;
  swap(lhs.state, rhs.state);
}

} // namespace ns

int main()
{

  VERIFY( ns::swaps == 0 );

  {
    DummyAlloc1<ns::value_type> testAlloc1{5};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( !o1 );

    DummyAlloc1<ns::value_type> testAlloc2{3};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o2{std::allocator_arg, testAlloc2};
    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( !o2 );

    swap(o1, o2);
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( !o1 );

    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( !o2 );
  }

  {
    DummyAlloc1<ns::value_type> testAlloc1{5};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o1{std::allocator_arg, testAlloc1, 45};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );

    DummyAlloc1<ns::value_type> testAlloc2{3};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o2{std::allocator_arg, testAlloc2};
    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( !o2 );

    swap(o1, o2);
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( !o1 );

    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( o2 );
    VERIFY( o2.value().alloc.id == 3 );

  }

  {
    DummyAlloc1<ns::value_type> testAlloc1{5};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o1{std::allocator_arg, testAlloc1};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( !o1 );

    DummyAlloc1<ns::value_type> testAlloc2{3};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o2{std::allocator_arg, testAlloc2, 45};
    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( o2 );

    swap(o1, o2);
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );
    VERIFY( o1.value().alloc.id == 5 );


    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( !o2 );

  }

  {
    DummyAlloc1<ns::value_type> testAlloc1{5};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o1{std::allocator_arg, testAlloc1, 999};
    VERIFY( o1.get_allocator().id == 5 );
    VERIFY( o1 );

    DummyAlloc1<ns::value_type> testAlloc2{3};
    std::basic_optional<ns::value_type, DummyAlloc1<ns::value_type>> o2{std::allocator_arg, testAlloc2, 167};
    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( o2 );

    swap(o1, o2);
    VERIFY( o1.get_allocator().id == 5);
    VERIFY( o1 );
   // VERIFY( o1.value().alloc.id == 3 );


    VERIFY( o2.get_allocator().id == 3 );
    VERIFY( o2 );
   // VERIFY( o2.value().alloc.id == 5 );
    VERIFY( ns::swaps == 1 );
  }

  VERIFY( counter == 0 );
}
