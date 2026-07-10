/* Support for the C++ profiles framework.
   Copyright (C) 2026 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_CP_PROFILES_H
#define GCC_CP_PROFILES_H

#include <cstdint>

/* Profiles attribute kind.  */

enum profiles_attr_kind : uint16_t {
  PAK_INVALID = 0,
  PAK_ENFORCE = 1,
  PAK_REQUIRE = 2,
  PAK_SUPPRESS = 3,
  PAK_EXEMPT = 4,
};

#endif /* GCC_CP_PROFILES_H */
