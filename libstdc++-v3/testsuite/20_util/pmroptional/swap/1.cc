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

struct exception {};

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
  explicit value_type(int state) : state(state) { }
  int state;

};


struct value_type_aa : private mixin_counter
{
  explicit value_type_aa(int state) : state(state) { }
  int state;


  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  value_type_aa(std::allocator_arg_t,allocator_type, int i)
  : value_type_aa(i){}

  value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa const& other)
    : value_type_aa(other){}

  value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa &&other)
    : value_type_aa(std::move(other)){}

};

int swaps = 0;


void
swap(value_type& lhs, value_type& rhs)
{
  ++swaps;
  using std::swap;
  swap(lhs.state, rhs.state);
}

void
swap(value_type_aa& lhs, value_type_aa& rhs)
{
  ++swaps;
  using std::swap;
  swap(lhs.state, rhs.state);
}


} // namespace ns

template<typename T, typename U>
void
test01()
{

  ns::swaps = 0;

  {
    T o; U p;
    swap(o, p);
    VERIFY( !o );
    VERIFY( !p );
  }

  {
    T o { std::in_place, 45 }; U p;
    swap(o, p);
    VERIFY( !o );
    VERIFY( p && p->state == 45 );
  }

  {
    T o; U p { std::in_place, 45 };
    swap(o, p);
    VERIFY( o && o->state == 45 );
    VERIFY( !p );
  }

  {
    T o { std::in_place, 167 }; U p { std::in_place, 999 };
    VERIFY( ns::swaps == 0 );

    swap(o, p);

    VERIFY( o && o->state == 999 );
    VERIFY( p && p->state == 167 );
    VERIFY( ns::swaps == 1 );
  }

  VERIFY( counter == 0 );

}
int main()
{

  test01<std::pmr::optional<ns::value_type>, std::pmr::optional<ns::value_type>>();
  test01<std::pmr::optional<ns::value_type>, std::optional<ns::value_type>>();
  test01<std::optional<ns::value_type>, std::pmr::optional<ns::value_type>>();

  test01<std::pmr::optional<ns::value_type_aa>, std::pmr::optional<ns::value_type_aa>>();
  test01<std::pmr::optional<ns::value_type_aa>, std::optional<ns::value_type_aa>>();
  test01<std::optional<ns::value_type_aa>, std::pmr::optional<ns::value_type_aa>>();

}
