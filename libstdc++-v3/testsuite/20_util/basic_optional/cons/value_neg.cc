// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 }  }

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
#include <string>
#include <memory>

int main()
{
  {
    struct X
    {
      explicit X(int) {}
    };
    std::basic_optional<X> ox{42};
    std::basic_optional<X> ox2 = 42; // { dg-error "conversion" }
    std::basic_optional<std::unique_ptr<int>> oup{new int};
    std::basic_optional<std::unique_ptr<int>> oup2 = new int;  // { dg-error "conversion" }
    struct U { explicit U(std::in_place_t); };
    std::basic_optional<U> ou(std::in_place); // { dg-error "no matching" }
  }
}
// { dg-prune-output "no type .*enable_if" }
