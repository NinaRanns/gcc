// Copyright (C) 2020 Free Software Foundation, Inc.
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

// { dg-options "-std=gnu++2a" }
// { dg-do compile { target c++2a } }

#include "../../../../include/std/basic_optional"
#include <testsuite_hooks.h>

void
test01()
{
  using O = std::basic_optional<int>;

  VERIFY( std::is_eq(O{} <=> O{}) );
  VERIFY( std::is_lt(O{} <=> O{1}) );
  VERIFY( std::is_gt(O{1} <=> O{}) );
  VERIFY( std::is_eq(O{1} <=> O{1}) );
  VERIFY( std::is_lt(O{1} <=> O{2}) );

  VERIFY( O{} == O{} );
  VERIFY( O{} < O{1} );
  VERIFY( O{1} > O{} );
  VERIFY( O{1} == O{1} );
  VERIFY( O{1} != O{2} );
  VERIFY( O{1} < O{2} );

  using Os = std::basic_optional<short>;
  VERIFY( std::is_eq(O{} <=> Os{}) );
  VERIFY( std::is_lt(O{} <=> Os{1}) );
  VERIFY( std::is_gt(O{1} <=> Os{}) );
  VERIFY( std::is_eq(O{1} <=> Os{1}) );
  VERIFY( std::is_lt(O{1} <=> Os{2}) );

  VERIFY( O{} == Os{} );
  VERIFY( O{} < Os{1} );
  VERIFY( O{1} > Os{} );
  VERIFY( O{1} == Os{1} );
  VERIFY( O{1} != Os{2} );
  VERIFY( O{1} < Os{2} );

  // Would requires narrowing conversion to make operands the same type:
  static_assert( !std::three_way_comparable_with<O, std::basic_optional<unsigned>> );
}

void
test02()
{
  using O = std::basic_optional<int>;
  using std::nullopt;

  VERIFY( std::is_eq(O{} <=> nullopt) );
  VERIFY( std::is_gt(O{1} <=> nullopt) );
  VERIFY( std::is_lt(nullopt <=> O{1}) );

  VERIFY( O{} == nullopt );
  VERIFY( O{1} != nullopt );
  VERIFY( nullopt != O{1} );
  VERIFY( O{1} > nullopt );
  VERIFY( nullopt < O{1} );
  VERIFY( nullopt <= O{} );
  VERIFY( nullopt <= O{1} );
}
