// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 } }

// Copyright (C) 2016-2022 Free Software Foundation, Inc.
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

#include "../../../../include/std/basic_variant"
#include <testsuite_hooks.h>

class S{}; // No hash specialization

template<class T>
auto f(int) -> decltype(std::hash<std::basic_variant<std::allocator<int>,T>>(), std::true_type());

template<class T>
auto f(...) -> decltype(std::false_type());

static_assert(!decltype(f<S>(0))::value, "");
static_assert(!decltype(f<std::basic_variant<std::allocator<int>,S>>(0))::value, "");
static_assert(!decltype(f<std::basic_variant<std::allocator<int>,S, S>>(0))::value, "");
static_assert(decltype(f<std::basic_variant<std::allocator<int>,int>>(0))::value, "");
static_assert(decltype(f<std::basic_variant<std::allocator<int>,int, int>>(0))::value, "");
static_assert(!std::is_invocable_v<
    std::hash<std::basic_variant<std::allocator<int>,S>>&, std::basic_variant<std::allocator<int>,S> const&> );
static_assert(!std::is_invocable_v<
    std::hash<std::basic_variant<std::allocator<int>,S, int>>&, std::basic_variant<std::allocator<int>,S, int> const&> );
static_assert(std::is_invocable_v<
    std::hash<std::basic_variant<std::allocator<int>,int>>&, std::basic_variant<std::allocator<int>,int> const&> );
static_assert(std::is_invocable_v<
    std::hash<std::basic_variant<std::allocator<int>,int, int>>&, std::basic_variant<std::allocator<int>,int, int> const&> );

int main()
{
  int x = 42;
  std::basic_variant<std::allocator<int>,int> x2 = 42;
  VERIFY(std::hash<int>()(x) == std::hash<std::basic_variant<std::allocator<int>,int>>()(x2));
}
