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
struct value_type_non_aa : private mixin_counter
{
  enum state_type
  {
    zero,
    moved_from,
    throwing_construction,
    throwing_copy,
    throwing_copy_assignment,
    throwing_move,
    throwing_move_assignment,
    threw,
  };

  value_type_non_aa() = default;

  explicit value_type_non_aa(state_type state_)
  : state(state_)
  {
    throw_if(throwing_construction);
  }

  value_type_non_aa(value_type_non_aa const& other)
  : state(other.state)
  {
    throw_if(throwing_copy);
  }

  value_type_non_aa&
  operator=(value_type_non_aa const& other)
  {
    state = other.state;
    throw_if(throwing_copy_assignment);
    return *this;
  }

  value_type_non_aa(value_type_non_aa&& other)
  : state(other.state)
  {
    other.state = moved_from;
    throw_if(throwing_move);
  }

  value_type_non_aa&
  operator=(value_type_non_aa&& other)
  {
    state = other.state;
    other.state = moved_from;
    throw_if(throwing_move_assignment);
    return *this;
  }

  void throw_if(state_type match)
  {
    if(state == match)
    {
      state = threw;
      throw exception {};
    }
  }

  state_type state = zero;
};
struct value_type_aa : private mixin_counter
{
  enum state_type
  {
    zero,
    moved_from,
    throwing_construction,
    throwing_copy,
    throwing_copy_assignment,
    throwing_move,
    throwing_move_assignment,
    threw,
  };

  value_type_aa() = default;

  explicit value_type_aa(state_type state_)
  : state(state_)
  {
    throw_if(throwing_construction);
  }

  value_type_aa(value_type_aa const& other)
  : state(other.state)
  {
    throw_if(throwing_copy);
  }

  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  value_type_aa(std::allocator_arg_t,allocator_type)
  : value_type_aa(){}


  value_type_aa(std::allocator_arg_t,allocator_type, state_type i)
  : value_type_aa(i){}

  value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa const& other)
    : value_type_aa(other){}

  value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa &&other)
      : value_type_aa(std::move(other)){}


  value_type_aa&
  operator=(value_type_aa const& other)
  {
    state = other.state;
    throw_if(throwing_copy_assignment);
    return *this;
  }

  value_type_aa(value_type_aa&& other)
  : state(other.state)
  {
    other.state = moved_from;
    throw_if(throwing_move);
  }

  value_type_aa&
  operator=(value_type_aa&& other)
  {
    state = other.state;
    other.state = moved_from;
    throw_if(throwing_move_assignment);
    return *this;
  }

  void throw_if(state_type match)
  {
    if(state == match)
    {
      state = threw;
      throw exception {};
    }
  }

  state_type state = zero;
};

template<typename T, typename U, typename V>
void
test01()
{
  using S = V::state_type;
  auto const make = [](S s = S::zero) { return U { std::in_place, s }; };

  enum outcome_type { nothrow, caught, bad_catch };

  // Check copy/move assignment for disengaged optional

  // From disengaged optional
  {
    T o;
    VERIFY( !o );
    U p;
    o = p;
    VERIFY( !o );
    VERIFY( !p );
  }

  {
    T o;
    VERIFY( !o );
    U p;
    o = std::move(p);
    VERIFY( !o );
    VERIFY( !p );
  }

  {
    T o;
    VERIFY( !o );
    o = {};
    VERIFY( !o );
  }

  // From engaged optional
  {
    T o;
    VERIFY( !o );
    U p = make(S::throwing_copy_assignment);
    o = p;
    VERIFY( o && o->state == S::throwing_copy_assignment );
    VERIFY( p && p->state == S::throwing_copy_assignment );
  }

  {
    T o;
    VERIFY( !o );
    U p = make(S::throwing_move_assignment);
    o = std::move(p);
    VERIFY( o && o->state == S::throwing_move_assignment );
    VERIFY( p && p->state == S::moved_from );
  }

  {
    outcome_type outcome {};
    T o;
    VERIFY( !o );
    U p = make(S::throwing_copy);

    try
    {
      o = p;
    }
    catch(exception const&)
    { outcome = caught; }
    catch(...)
    { outcome = bad_catch; }

    VERIFY( outcome == caught );
    VERIFY( !o );
    VERIFY( p && p->state == S::throwing_copy );
  }

  {
    outcome_type outcome {};
    T o;
    VERIFY( !o );
    U p = make(S::throwing_move);

    try
    {
      o = std::move(p);
    }
    catch(exception const&)
    { outcome = caught; }
    catch(...)
    { outcome = bad_catch; }

    VERIFY( outcome == caught );
    VERIFY( !o );
    VERIFY( p && p->state == S::moved_from );
  }

  VERIFY( counter == 0 );
}

int main()
{
  test01<std::pmr::optional<value_type_non_aa>,std::pmr::optional<value_type_non_aa>, value_type_non_aa>();
  test01<std::pmr::optional<value_type_non_aa>,std::optional<value_type_non_aa>, value_type_non_aa>();
  test01<std::optional<value_type_non_aa>,std::pmr::optional<value_type_non_aa>, value_type_non_aa>();

  test01<std::pmr::optional<value_type_aa>,std::pmr::optional<value_type_aa>, value_type_aa>();
  test01<std::optional<value_type_aa>,std::pmr::optional<value_type_aa>, value_type_aa>();
  test01<std::pmr::optional<value_type_non_aa>,std::optional<value_type_non_aa>, value_type_non_aa>();

}
