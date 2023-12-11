// { dg-options "-std=gnu++20" }
// { dg-do compile { target c++20 } }

#include "../../../include/std/basic_variant.h"

int visit(int*, std::true_type) { return 0; }

const std::true_type dat;

int i = visit(nullptr, dat);
