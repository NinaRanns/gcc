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

#include "../../../../include/std/basic_optional"
#include <testsuite_hooks.h>

#include <tuple>
#include <string>

namespace ns
{
  struct value_type
  {
    int i;
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

} // namespace ns

int main()
{
  using ns::value_type;
  using SO = std::optional<value_type>;
  using BO = std::basic_optional<value_type>;

  {
    SO o;
    BO p;
    VERIFY( !(o < p) );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( o >= p );
  }
  {
    BO o;
    SO p;
    VERIFY( !(o < p) );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( o >= p );
  }


  {
    SO o { value_type { 42, "forty-two" } };
    BO p;
    VERIFY( !(o < p) );
    VERIFY( o > p );
    VERIFY( !(o <= p) );
    VERIFY( o >= p );
  }
  {
    BO o { value_type { 42, "forty-two" } };
    SO p;
    VERIFY( !(o < p) );
    VERIFY( o > p );
    VERIFY( !(o <= p) );
    VERIFY( o >= p );
  }

  {
    SO o;
    BO p { value_type { 42, "forty-two" } };
    VERIFY( o < p );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( !(o >= p) );
  }
  {
    BO o;
    SO p { value_type { 42, "forty-two" } };
    VERIFY( o < p );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( !(o >= p) );
  }

  {
    SO o { value_type { 11, "eleventy" } };
    BO p { value_type { 42, "forty-two" } };
    VERIFY( o < p );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( !(o >= p) );
  }
  {
    BO o { value_type { 11, "eleventy" } };
    SO p { value_type { 42, "forty-two" } };
    VERIFY( o < p );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( !(o >= p) );
  }

  {
    SO o { value_type { 42, "forty-two" } };
    BO p { value_type { 11, "eleventy" } };
    VERIFY( !(o < p) );
    VERIFY( o > p );
    VERIFY( !(o <= p) );
    VERIFY( o >= p );
  }
  {
    BO o { value_type { 42, "forty-two" } };
    SO p { value_type { 11, "eleventy" } };
    VERIFY( !(o < p) );
    VERIFY( o > p );
    VERIFY( !(o <= p) );
    VERIFY( o >= p );
  }

  {
    SO o { value_type { 42, "forty-two" } };
    BO p { value_type { 42, "forty-two" } };
    VERIFY( !(o < p) );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( o >= p );
  }
  {
    BO o { value_type { 42, "forty-two" } };
    SO p { value_type { 42, "forty-two" } };
    VERIFY( !(o < p) );
    VERIFY( !(o > p) );
    VERIFY( o <= p );
    VERIFY( o >= p );
  }
}
