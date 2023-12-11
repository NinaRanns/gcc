// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

#include "../../../../include/std/basic_variant.h"

struct Alloc{};

static_assert(std::is_same_v<std::variant_alternative< 0,
              std::basic_variant<Alloc, int, char, long>>::type,
              int>);

static_assert(std::is_same_v<std::variant_alternative< 1,
              std::basic_variant<Alloc, int, const char, long>>::type,
              const char>);

static_assert(std::is_same_v<std::variant_alternative< 2,
              std::basic_variant<Alloc, int, char, int>>::type,
              int>);



static_assert(std::is_same_v<std::variant_alternative< 0,
              const std::basic_variant<Alloc, int, char, long>>::type,
              const int>);

static_assert(std::is_same_v<std::variant_alternative< 1,
              const std::basic_variant<Alloc, int, const char, long>>::type,
              const char>);

static_assert(std::is_same_v<std::variant_alternative< 2,
              const std::basic_variant<Alloc, int, char, int>>::type,
              const int>);

static_assert(std::is_same_v<std::variant_alternative< 0,
              volatile std::basic_variant<Alloc, int, char, long>>::type,
              volatile int>);

static_assert(std::is_same_v<std::variant_alternative< 1,
    volatile std::basic_variant<Alloc, int, const char, long>>::type,
    volatile const char>);

static_assert(std::is_same_v<std::variant_alternative< 2,
    volatile std::basic_variant<Alloc, int, char, int>>::type,
    volatile int>);



static_assert(std::is_same_v<std::variant_alternative< 0,
    volatile const std::basic_variant<Alloc, int, char, long>>::type,
    volatile const int>);

static_assert(std::is_same_v<std::variant_alternative< 1,
    volatile const std::basic_variant<Alloc, int, const char, long>>::type,
    volatile const char>);

static_assert(std::is_same_v<std::variant_alternative< 2,
    volatile const std::basic_variant<Alloc, int, char, int>>::type,
    volatile const int>);


int main()
{}

