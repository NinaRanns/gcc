// Copyright (C) 2017-2020 Free Software Foundation, Inc.
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

// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 }  }

#include "../../../../include/std/basic_optional"
#include <type_traits>

struct MoveOnly
{
  MoveOnly() = default;
  MoveOnly(MoveOnly&&);
  MoveOnly& operator=(MoveOnly&&);
};

int main()
{
  std::basic_optional x = 5;
  static_assert(std::is_same_v<decltype(x), std::basic_optional<int>>);
  int y = 42;
  std::basic_optional x2 = y;
  static_assert(std::is_same_v<decltype(x2), std::basic_optional<int>>);
  const int z = 666;
  std::basic_optional x3 = z;
  static_assert(std::is_same_v<decltype(x3), std::basic_optional<int>>);
  std::basic_optional mo = MoveOnly();
  static_assert(std::is_same_v<decltype(mo), std::basic_optional<MoveOnly>>);
  mo = MoveOnly();

  std::basic_optional copy = x;
  static_assert(std::is_same_v<decltype(copy), std::basic_optional<int>>);
  std::basic_optional move = std::move(mo);
  static_assert(std::is_same_v<decltype(move), std::basic_optional<MoveOnly>>);
}
