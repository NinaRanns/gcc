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
  tracker(int value) : value(value) { ++count; }
  ~tracker() { --count; }

  tracker(tracker const& other) : value(other.value) { ++count; }
  tracker(tracker&& other) : value(other.value)
  {
    other.value = -1;
    ++count;
  }

  tracker& operator=(tracker const&) = default;
  tracker& operator=(tracker&&) = default;

  int value;

  static int count;


};
int tracker::count = 0;

struct tracker_aa
{
  tracker_aa(int value) : value(value) { ++count; }
  ~tracker_aa() { --count; }

  tracker_aa(tracker_aa const& other) : value(other.value) { ++count; }
  tracker_aa(tracker_aa&& other) : value(other.value)
  {
    other.value = -1;
    ++count;
  }

  tracker_aa& operator=(tracker_aa const&) = default;
  tracker_aa& operator=(tracker_aa&&) = default;

  int value;

  static int count;

  typedef std::pmr::polymorphic_allocator<void> allocator_type;

  tracker_aa(std::allocator_arg_t,allocator_type, int x)
   : tracker_aa(x){}
  tracker_aa(std::allocator_arg_t,allocator_type, const tracker_aa& other)
     : tracker_aa(other){}
  tracker_aa(std::allocator_arg_t,allocator_type, tracker_aa&& other)
       : tracker_aa(std::move(other)){}

};
int tracker_aa::count = 0;

struct exception { };

struct throwing_copy
{
  throwing_copy() = default;
  throwing_copy(throwing_copy const&) { throw exception {}; }
};

struct throwing_copy_aa
{
    throwing_copy_aa() = default;
    throwing_copy_aa(throwing_copy_aa const&) { throw exception {}; }

  typedef std::pmr::polymorphic_allocator<void> allocator_type;
  throwing_copy_aa(std::allocator_arg_t,allocator_type)
   {}
  throwing_copy_aa(std::allocator_arg_t,allocator_type, const throwing_copy_aa& other)
   { throw exception {}; }
};
template<typename T, typename U=T>
void
test01()
{
  U o;
  T copy = o;
  VERIFY( !copy );
  VERIFY( !o );

}

template<typename T, typename U = T>
void
test02()
{
  const long val = 0x1234ABCD;
  U o { std::in_place, val};
  T copy = o;
  VERIFY( copy );
  VERIFY( *copy == val );
  VERIFY( o && o == val );
}

template<typename T, typename U = T>
void
test03()
{
  U o;
  T copy = o;
  VERIFY( !copy );
  VERIFY( T::value_type::count == 0 );
  VERIFY( !o );
}
template<typename T, typename U = T>
void
test04()
{
  U o { std::in_place, 333 };
  T copy = o;
  VERIFY( copy );
  VERIFY( copy->value == 333 );
  VERIFY( T::value_type::count == 2 );
  VERIFY( o && o->value == 333 );
}

template<typename T, typename U = T>
void
test05()
{
  enum outcome { nothrow, caught, bad_catch };

    {
      outcome result = nothrow;
      U o;

      try
      {
        T copy = o;
      }
      catch(exception const&)
      { result = caught; }
      catch(...)
      { result = bad_catch; }

      VERIFY( result == nothrow );
    }

    {
      outcome result = nothrow;
      U o { std::in_place };

      try
      {
        T copy = o;
      }
      catch(exception const&)
      { result = caught; }
      catch(...)
      { result = bad_catch; }

      VERIFY( result == caught );
    }
}
int main()
{
  // [20.5.4.1] Constructors

  test01<std::pmr::optional<long>>();
  test01<std::pmr::optional<long>, std::optional<long>>();
  test01<std::optional<long>, std::pmr::optional<long>>();

  test02<std::pmr::optional<long>>();
  test02<std::pmr::optional<long>, std::optional<long>>();
  test02<std::optional<long>, std::pmr::optional<long>>();

  test03<std::pmr::optional<tracker>>();
  test03<std::pmr::optional<tracker>, std::optional<tracker>>();
  test03<std::optional<tracker>, std::pmr::optional<tracker>>();
  test03<std::pmr::optional<tracker_aa>>();
  test03<std::pmr::optional<tracker_aa>, std::optional<tracker_aa>>();
  test03<std::optional<tracker_aa>, std::pmr::optional<tracker_aa>>();


  test04<std::pmr::optional<tracker>>();
  test04<std::pmr::optional<tracker>, std::optional<tracker>>();
  test04<std::optional<tracker>, std::pmr::optional<tracker>>();
  test04<std::pmr::optional<tracker_aa>>();
  test04<std::pmr::optional<tracker_aa>, std::optional<tracker_aa>>();
  test04<std::optional<tracker_aa>, std::pmr::optional<tracker_aa>>();

  enum outcome { nothrow, caught, bad_catch };
  {
    outcome result = nothrow;
    throwing_copy_aa o;

    try
    {
      throwing_copy_aa copy( std::allocator_arg, std::pmr::get_default_resource(),  o);
    }
    catch(exception const&)
    { result = caught; }
    catch(...)
    { result = bad_catch; }

    VERIFY( result == caught );
  }

  test05<std::pmr::optional<throwing_copy>>();
  test05<std::pmr::optional<throwing_copy>, std::optional<throwing_copy>>();
  test05<std::optional<throwing_copy>, std::pmr::optional<throwing_copy>>();
  test05<std::pmr::optional<throwing_copy_aa>>();
  test05<std::pmr::optional<throwing_copy_aa>, std::optional<throwing_copy_aa>>();
  test05<std::optional<throwing_copy_aa>, std::pmr::optional<throwing_copy_aa>>();

}
