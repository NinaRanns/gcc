// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

#include "../../../../include/std/basic_variant.h"

struct Alloc{};

static_assert(std::variant_size<
              std::basic_variant<Alloc, int, char, long>>::value == 3);

static_assert(std::variant_size<
              std::basic_variant<Alloc, int, std::string>>::value == 2);
static_assert(std::variant_size<
              std::basic_variant<std::allocator<int>, int, std::string>>::value == 2);


static_assert(std::variant_size<
              const std::basic_variant<Alloc, int, char, long>>::value == 3);
static_assert(std::variant_size<
              const std::basic_variant<Alloc, int, int>>::value == 2);
static_assert(std::variant_size<
              const std::basic_variant<std::allocator<int>, int, const int>>::value == 2);


static_assert(std::variant_size_v<
              std::basic_variant<Alloc, int, char, long>> == 3);

static_assert(std::variant_size_v<
              std::basic_variant<Alloc, int, std::string>> == 2);
static_assert(std::variant_size_v<
              std::basic_variant<std::allocator<int>, int, std::string>> == 2);


static_assert(std::variant_size_v<
              const std::basic_variant<Alloc, int, char, long>> == 3);
static_assert(std::variant_size_v<
              const std::basic_variant<Alloc, int, int>> == 2);
static_assert(std::variant_size_v<
              const std::basic_variant<std::allocator<int>, int, const int>> == 2);

int main()
{}

