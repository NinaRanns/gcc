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

#include <tuple>
#include <string>
#include "../../../../include/std/pmroptional"

namespace ns
{
  struct value_type
  {


    value_type() = default;
    value_type(int _i, std::string _s) : i(_i),s(_s) { }
    int i = 0;
    std::string s;

  };

  bool
  operator==(value_type const& lhs, value_type const& rhs)
  { return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s); }

  bool
  operator!=(value_type const& lhs, value_type const& rhs)
  { return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s); }

  bool
  operator<(value_type const& lhs, value_type const& rhs)
  { return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s); }

  bool
  operator>(value_type const& lhs, value_type const& rhs)
  { return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s); }

  bool
  operator<=(value_type const& lhs, value_type const& rhs)
  { return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s); }

  bool
  operator>=(value_type const& lhs, value_type const& rhs)
  { return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s); }



  struct value_type_aa
    {


      value_type_aa() = default;
      value_type_aa(int _i, std::string _s) : i(_i),s(_s) { }
      int i = 0;
      std::string s;

      typedef std::pmr::polymorphic_allocator<void> allocator_type;

      value_type_aa(std::allocator_arg_t,allocator_type)
      : value_type_aa(){}

      value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa const& other)
        : value_type_aa(other){}

      value_type_aa(std::allocator_arg_t,allocator_type, value_type_aa &&other)
          : value_type_aa(std::move(other)){}

      value_type_aa(std::allocator_arg_t,allocator_type, int _i, std::string const&_s)
        : value_type_aa(i,s){}

      value_type_aa(std::allocator_arg_t,allocator_type, int _i, std::string &&_s)
            : value_type_aa(i,std::move(s)){}
    };

    bool
    operator==(value_type_aa const& lhs, value_type_aa const& rhs)
    { return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s); }

    bool
    operator!=(value_type_aa const& lhs, value_type_aa const& rhs)
    { return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s); }

    bool
    operator<(value_type_aa const& lhs, value_type_aa const& rhs)
    { return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s); }

    bool
    operator>(value_type_aa const& lhs, value_type_aa const& rhs)
    { return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s); }

    bool
    operator<=(value_type_aa const& lhs, value_type_aa const& rhs)
    { return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s); }

    bool
    operator>=(value_type_aa const& lhs, value_type_aa const& rhs)
    { return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s); }

} // namespace ns
template<typename T, typename U>
void test01()
{
  using value_type = typename T::value_type;
  {
    T o;  U p;
    VERIFY( o == p );
    VERIFY( !(o != p) );
  }

  {
    T o { value_type { 42, "forty-two" } };
    U p;
    VERIFY( !(o == p) );
    VERIFY( o != p );
  }

  {
    T o;
    U p { value_type { 42, "forty-two" } };
    VERIFY( !(o == p) );
    VERIFY( o != p );
  }

  {
    T o { value_type { 11, "eleventy" } };
    U p { value_type { 42, "forty-two" } };
    VERIFY( !(o == p) );
    VERIFY( o != p );
  }

  {
    T o { value_type { 42, "forty-two" } }; U p { value_type { 11, "eleventy" } };
    VERIFY( !(o == p) );
    VERIFY( o != p );
  }

  {
    T o { value_type { 42, "forty-two" } }; U p { value_type { 42, "forty-two" } };
    VERIFY( o == p );
    VERIFY( !(o != p) );
  }

}
int main()
{
  test01<std::pmr::optional<ns::value_type>, std::pmr::optional<ns::value_type>>();
  test01<std::pmr::optional<ns::value_type>, std::optional<ns::value_type>>();


  test01<std::pmr::optional<ns::value_type_aa>, std::pmr::optional<ns::value_type_aa>>();
  test01<std::pmr::optional<ns::value_type_aa>, std::optional<ns::value_type_aa>>();

}
