// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }

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
#include <string>
#include <vector>
#include <unordered_set>
#include <ext/throw_allocator.h>
#include <testsuite_hooks.h>

using namespace std;

struct AlwaysThrow
{
  AlwaysThrow() = default;

  AlwaysThrow(const AlwaysThrow&)
  { throw nullptr; }

  AlwaysThrow(AlwaysThrow&&)
  { throw nullptr; }

  AlwaysThrow& operator=(const AlwaysThrow&)
  {
    throw nullptr;
    return *this;
  }

  AlwaysThrow& operator=(AlwaysThrow&&)
  {
    throw nullptr;
    return *this;
  }

  bool operator<(const AlwaysThrow&) const { VERIFY(false); }
  bool operator<=(const AlwaysThrow&) const { VERIFY(false); }
  bool operator==(const AlwaysThrow&) const { VERIFY(false); }
  bool operator!=(const AlwaysThrow&) const { VERIFY(false); }
  bool operator>=(const AlwaysThrow&) const { VERIFY(false); }
  bool operator>(const AlwaysThrow&) const { VERIFY(false); }
};

struct DeletedMoves
{
  DeletedMoves() = default;
  DeletedMoves(const DeletedMoves&) = default;
  DeletedMoves(DeletedMoves&&) = delete;
  DeletedMoves& operator=(const DeletedMoves&) = default;
  DeletedMoves& operator=(DeletedMoves&&) = delete;
};

void default_ctor()
{
  basic_variant<std::allocator<int>,monostate, string> v;
  VERIFY(holds_alternative<monostate>(v));
}

void copy_ctor()
{
  basic_variant<std::allocator<int>,monostate, string> v("a");
  VERIFY(holds_alternative<string>(v));
  basic_variant<std::allocator<int>,monostate, string> u(v);
  VERIFY(holds_alternative<string>(u));
  VERIFY(get<string>(u) == "a");
}

void move_ctor()
{
  basic_variant<std::allocator<int>,monostate, string> v("a");
  VERIFY(holds_alternative<string>(v));
  basic_variant<std::allocator<int>,monostate, string> u(std::move(v));
  VERIFY(holds_alternative<string>(u));
  VERIFY(get<string>(u) == "a");
  VERIFY(holds_alternative<string>(v));

  basic_variant<std::allocator<int>,vector<int>, DeletedMoves> d{std::in_place_index<0>, {1, 2, 3, 4}};
  // DeletedMoves is not move constructible, so this uses copy ctor:
  basic_variant<std::allocator<int>,vector<int>, DeletedMoves> e(std::move(d));
  VERIFY(std::get<0>(d).size() == 4);
  VERIFY(std::get<0>(e).size() == 4);
}

void arbitrary_ctor()
{
  basic_variant<std::allocator<int>,int, string> v("a");
  VERIFY(holds_alternative<string>(v));
  VERIFY(get<1>(v) == "a");

  {
    // P0608R3
    basic_variant<std::allocator<int>,string, bool> x = "abc";
    VERIFY(x.index() == 0);
  }

  {
    // P0608R3
    struct U {
      U(char16_t c) : c(c) { }
      char16_t c;
    };
    basic_variant<std::allocator<int>,char, U> x = u'\u2043';
    VERIFY(x.index() == 1);
    VERIFY(std::get<1>(x).c == u'\u2043');

    struct Double {
      Double(double& d) : d(d) { }
      double& d;
    };
    double d = 3.14;
    basic_variant<std::allocator<int>,int, Double> y = d;
    VERIFY(y.index() == 1);
    VERIFY(std::get<1>(y).d == d);
  }

  {
    // P0608R3
    basic_variant<std::allocator<int>,float, int> v1 = 'a';
    VERIFY(std::get<1>(v1) == int('a'));
    basic_variant<std::allocator<int>,float, long> v2 = 0;
    VERIFY(std::get<1>(v2) == 0L);
    struct big_int { big_int(int) { } };
    basic_variant<std::allocator<int>,float, big_int> v3 = 0;
    VERIFY(v3.index() == 1);
  }

  {
    // P1957R2 Converting from T* to bool should be considered narrowing
    struct ConvertibleToBool
    {
      operator bool() const { return true; }
    };
    basic_variant<std::allocator<int>,bool> v1 = ConvertibleToBool();
    VERIFY(std::get<0>(v1) == true);
    basic_variant<std::allocator<int>,bool, int> v2 = ConvertibleToBool();
    VERIFY(std::get<0>(v2) == true);
    basic_variant<std::allocator<int>,int, bool> v3 = ConvertibleToBool();
    VERIFY(std::get<1>(v3) == true);
  }
}

struct ThrowingMoveCtorThrowsCopyCtor
{
  ThrowingMoveCtorThrowsCopyCtor() noexcept = default;
  ThrowingMoveCtorThrowsCopyCtor(ThrowingMoveCtorThrowsCopyCtor&&) {}
  ThrowingMoveCtorThrowsCopyCtor(ThrowingMoveCtorThrowsCopyCtor const&)
  {
    throw 0;
  }

  ThrowingMoveCtorThrowsCopyCtor& operator=(ThrowingMoveCtorThrowsCopyCtor&&) noexcept
    = default;
  ThrowingMoveCtorThrowsCopyCtor& operator=(ThrowingMoveCtorThrowsCopyCtor const&) noexcept
    = default;
};

void copy_assign()
{
  basic_variant<std::allocator<int>,monostate, string> v("a");
  VERIFY(holds_alternative<string>(v));
  basic_variant<std::allocator<int>,monostate, string> u;
  u = v;
  VERIFY(holds_alternative<string>(u));
  VERIFY(get<string>(u) == "a");
  {
    std::basic_variant<std::allocator<int>,int, ThrowingMoveCtorThrowsCopyCtor> v1,
      v2 = ThrowingMoveCtorThrowsCopyCtor();
    bool should_throw = false;
    try
      {
	v1 = v2;
      }
    catch(int)
      {
	should_throw = true;
      }
    VERIFY(should_throw);
  }
}

void move_assign()
{
  basic_variant<std::allocator<int>,monostate, string> v("a");
  VERIFY(holds_alternative<string>(v));
  basic_variant<std::allocator<int>,monostate, string> u;
  u = std::move(v);
  VERIFY(holds_alternative<string>(u));
  VERIFY(get<string>(u) == "a");
  VERIFY(holds_alternative<string>(v));

  basic_variant<std::allocator<int>,vector<int>, DeletedMoves> d{std::in_place_index<0>, {1, 2, 3, 4}};
  basic_variant<std::allocator<int>,vector<int>, DeletedMoves> e;
  // DeletedMoves is not move assignable, so this uses copy assignment:
  e = std::move(d);
  VERIFY(std::get<0>(d).size() == 4);
  VERIFY(std::get<0>(e).size() == 4);
}

void arbitrary_assign()
{
  basic_variant<std::allocator<int>,int, string> v;
  v = "a";

  VERIFY(holds_alternative<string>(basic_variant<std::allocator<int>,int, string>("a")));
  VERIFY(get<1>(v) == "a");

  {
    // P0608R3
    using T1 = basic_variant<std::allocator<int>,float, int>;
    T1 v1;
    v1 = 0;
    VERIFY(v1.index() == 1);

    using T2 = basic_variant<std::allocator<int>,float, long>;
    T2 v2;
    v2 = 0;
    VERIFY(v2.index() == 1);

    struct big_int {
      big_int(int) { }
    };
    using T3 = basic_variant<std::allocator<int>,float, big_int>;
    T3 v3;
    v3 = 0;
    VERIFY(v3.index() == 1);
  }

  {
    // P1957R2 Converting from T* to bool should be considered narrowing
    struct ConvertibleToBool
    {
      operator bool() const { return true; }
    };
    basic_variant<std::allocator<int>,bool> v1;
    v1 = ConvertibleToBool();
    VERIFY(std::get<0>(v1) == true);
    basic_variant<std::allocator<int>,bool, int> v2;
    v2 = ConvertibleToBool();
    VERIFY(std::get<0>(v2) == true);
    basic_variant<std::allocator<int>,int, bool> v3;
    v3 = ConvertibleToBool();
    VERIFY(std::get<1>(v3) == true);
  }
}

void dtor()
{
  struct A {
      A(int& called) : called(called) {}
      ~A() {
	  called++;
      }
      int& called;
  };
  {
    int called = 0;
    { basic_variant<std::allocator<int>,string, A> a(in_place_index<1>, called); }
    VERIFY(called == 1);
  }
  {
    int called = 0;
    { basic_variant<std::allocator<int>,string, A> a(in_place_index<0>); }
    VERIFY(called == 0);
  }
}

void in_place_index_ctor()
{
  {
    basic_variant<std::allocator<int>,int, string> v(in_place_index<1>, "a");
    VERIFY(holds_alternative<string>(v));
    VERIFY(get<1>(v) == "a");
  }
  {
    basic_variant<std::allocator<int>,int, string> v(in_place_index<1>, {'a', 'b'});
    VERIFY(holds_alternative<string>(v));
    VERIFY(get<1>(v) == "ab");
  }
}

void in_place_type_ctor()
{
  {
    basic_variant<std::allocator<int>,int, string> v(in_place_type<string>, "a");
    VERIFY(holds_alternative<string>(v));
    VERIFY(get<1>(v) == "a");
  }
  {
    basic_variant<std::allocator<int>,int, string> v(in_place_type<string>, {'a', 'b'});
    VERIFY(holds_alternative<string>(v));
    VERIFY(get<1>(v) == "ab");
  }
}

void emplace()
{
  basic_variant<std::allocator<int>,int, string> v;
  v.emplace<0>(1);
  VERIFY(get<0>(v) == 1);
  v.emplace<string>("a");
  VERIFY(get<string>(v) == "a");
  v.emplace<1>({'a', 'b'});
  VERIFY(get<1>(v) == "ab");
  v.emplace<string>({'a', 'c'});
  VERIFY(get<string>(v) == "ac");
  {
    basic_variant<std::allocator<int>,int, AlwaysThrow> v;
    AlwaysThrow a;
    try { v.emplace<1>(a); } catch (nullptr_t) { }
    VERIFY(v.valueless_by_exception());
    v.emplace<0>(42);
    VERIFY(!v.valueless_by_exception());
  }
  {
    basic_variant<std::allocator<int>,int, AlwaysThrow> v;
    try { v.emplace<1>(AlwaysThrow{}); } catch (nullptr_t) { }
    VERIFY(v.valueless_by_exception());
    v.emplace<0>(42);
    VERIFY(!v.valueless_by_exception());
  }
  VERIFY(&v.emplace<0>(1) == &std::get<0>(v));
  VERIFY(&v.emplace<int>(1) == &std::get<int>(v));
  VERIFY(&v.emplace<1>("a") == &std::get<1>(v));
  VERIFY(&v.emplace<string>("a") == &std::get<string>(v));
  {
    basic_variant<std::allocator<int>,vector<int>> v;
    VERIFY(&v.emplace<0>({1,2,3}) == &std::get<0>(v));
    VERIFY(&v.emplace<vector<int>>({1,2,3}) == &std::get<vector<int>>(v));
  }

  {
    // Ensure no copies of the vector are made, only moves.
    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87431#c21

    // static_assert(__detail::__variant::_Never_valueless_alt<vector<AlwaysThrow>>::value);
    basic_variant<std::allocator<int>,int, DeletedMoves, vector<AlwaysThrow>> v;
    v.emplace<2>(1);
    v.emplace<vector<AlwaysThrow>>(1);
    v.emplace<0>(0);

    // To test the emplace(initializer_list<U>, Args&&...) members we
    // can't use AlwaysThrow because elements in an initialier_list
    // are always copied. Use throw_allocator instead.
    using Vector = vector<int, __gnu_cxx::throw_allocator_limit<int>>;
    // static_assert(__detail::__variant::_Never_valueless_alt<Vector>::value);
    basic_variant<std::allocator<int>,int, DeletedMoves, Vector> vv;
    Vector::allocator_type::set_limit(1);
    vv.emplace<2>(1, 1);
    Vector::allocator_type::set_limit(1);
    vv.emplace<Vector>(1, 1);
    Vector::allocator_type::set_limit(1);
    vv.emplace<0>(0);
    Vector::allocator_type::set_limit(1);
    vv.emplace<2>({1, 2, 3});
    Vector::allocator_type::set_limit(1);
    vv.emplace<Vector>({1, 2, 3, 4});
    try {
      Vector::allocator_type::set_limit(0);
      vv.emplace<2>(1, 1);
      VERIFY(false);
    } catch (const __gnu_cxx::forced_error&) {
    }
    VERIFY(vv.valueless_by_exception());
  }
}

void test_get()
{
  VERIFY(get<1>(basic_variant<std::allocator<int>,int, string>("a")) == "a");
  VERIFY(get<string>(basic_variant<std::allocator<int>,int, string>("a")) == "a");
  {
    bool caught = false;

    try
      {
	get<0>(basic_variant<std::allocator<int>,int, string>("a"));
      }
    catch (const bad_variant_access&)
      {
	caught = true;
      }
    VERIFY(caught);
  }
  {
    bool caught = false;

    try
      {
	get<int>(basic_variant<std::allocator<int>,int, string>("a"));
      }
    catch (const bad_variant_access&)
      {
	caught = true;
      }
    VERIFY(caught);
  }
}

void test_relational()
{
  VERIFY((basic_variant<std::allocator<int>,int, string>(2) < basic_variant<std::allocator<int>,int, string>(3)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(3) == basic_variant<std::allocator<int>,int, string>(3)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(3) > basic_variant<std::allocator<int>,int, string>(2)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(3) <= basic_variant<std::allocator<int>,int, string>(3)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(2) <= basic_variant<std::allocator<int>,int, string>(3)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(3) >= basic_variant<std::allocator<int>,int, string>(3)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(3) >= basic_variant<std::allocator<int>,int, string>(2)));
  VERIFY((basic_variant<std::allocator<int>,int, string>(2) != basic_variant<std::allocator<int>,int, string>(3)));

  VERIFY((basic_variant<std::allocator<int>,int, string>(2) < basic_variant<std::allocator<int>,int, string>("a")));
  VERIFY((basic_variant<std::allocator<int>,string, int>(2) > basic_variant<std::allocator<int>,string, int>("a")));

  {
    basic_variant<std::allocator<int>,int, AlwaysThrow> v, w;
    try
      {
	AlwaysThrow a;
	v = a;
      }
    catch (nullptr_t) { }
    VERIFY(v.valueless_by_exception());
    VERIFY(v < w);
    VERIFY(v <= w);
    VERIFY(!(v == w));
    VERIFY(v == v);
    VERIFY(v != w);
    VERIFY(w > v);
    VERIFY(w >= v);
  }
}

void test_swap()
{
  basic_variant<std::allocator<int>,int, string> a("a"), b("b");
  a.swap(b);
  VERIFY(get<1>(a) == "b");
  VERIFY(get<1>(b) == "a");
  swap(a, b);
  VERIFY(get<1>(a) == "a");
  VERIFY(get<1>(b) == "b");
}

void test_visit()
{
  {
    struct Visitor
    {
      int operator()(int, float) {
	  return 0;
      }
      int operator()(int, double) {
	  return 1;
      }
      int operator()(char, float) {
	  return 2;
      }
      int operator()(char, double) {
	  return 3;
      }
      int operator()(int, float) const {
	  return 5;
      }
      int operator()(int, double) const {
	  return 6;
      }
      int operator()(char, float) const {
	  return 7;
      }
      int operator()(char, double) const {
	  return 8;
      }
    } visitor1;
    VERIFY(visit(visitor1, basic_variant<std::allocator<int>,int, char>(1), basic_variant<std::allocator<int>,float, double>(1.0f)) == 0);
    VERIFY(visit(visitor1, basic_variant<std::allocator<int>,int, char>(1), basic_variant<std::allocator<int>,float, double>(1.0)) == 1);
    VERIFY(visit(visitor1, basic_variant<std::allocator<int>,int, char>('a'), basic_variant<std::allocator<int>,float, double>(1.0f)) == 2);
    VERIFY(visit(visitor1, basic_variant<std::allocator<int>,int, char>('a'), basic_variant<std::allocator<int>,float, double>(1.0)) == 3);

    const auto& visitor2 = visitor1;
    VERIFY(visit(visitor2, basic_variant<std::allocator<int>,int, char>(1), basic_variant<std::allocator<int>,float, double>(1.0f)) == 5);
    VERIFY(visit(visitor2, basic_variant<std::allocator<int>,int, char>(1), basic_variant<std::allocator<int>,float, double>(1.0)) == 6);
    VERIFY(visit(visitor2, basic_variant<std::allocator<int>,int, char>('a'), basic_variant<std::allocator<int>,float, double>(1.0f)) == 7);
    VERIFY(visit(visitor2, basic_variant<std::allocator<int>,int, char>('a'), basic_variant<std::allocator<int>,float, double>(1.0)) == 8);
  }

  {
    struct Visitor
    {
      int operator()(int, float) && {
	  return 0;
      }
      int operator()(int, double) && {
	  return 1;
      }
      int operator()(char, float) && {
	  return 2;
      }
      int operator()(char, double) && {
	  return 3;
      }
    };
    VERIFY(visit(Visitor{}, basic_variant<std::allocator<int>,int, char>(1), basic_variant<std::allocator<int>,float, double>(1.0f)) == 0);
    VERIFY(visit(Visitor{}, basic_variant<std::allocator<int>,int, char>(1), basic_variant<std::allocator<int>,float, double>(1.0)) == 1);
    VERIFY(visit(Visitor{}, basic_variant<std::allocator<int>,int, char>('a'), basic_variant<std::allocator<int>,float, double>(1.0f)) == 2);
    VERIFY(visit(Visitor{}, basic_variant<std::allocator<int>,int, char>('a'), basic_variant<std::allocator<int>,float, double>(1.0)) == 3);
  }
}

struct Hashable
{
  Hashable(const char* s) : s(s) { }
  // Non-trivial special member functions:
  Hashable(const Hashable&) { }
  Hashable(Hashable&&) noexcept { }
  ~Hashable() { }

  string s;

  bool operator==(const Hashable& rhs) const noexcept
  { return s == rhs.s; }
};

namespace std {
  template<> struct hash<Hashable> {
    size_t operator()(const Hashable& h) const noexcept
    { return hash<std::string>()(h.s); }
  };
}

void test_hash()
{
  unordered_set<basic_variant<std::allocator<int>,int, Hashable>> s;
  VERIFY(s.emplace(3).second);
  VERIFY(s.emplace("asdf").second);
  VERIFY(s.emplace().second);
  VERIFY(s.size() == 3);
  VERIFY(!s.emplace(3).second);
  VERIFY(!s.emplace("asdf").second);
  VERIFY(!s.emplace().second);
  VERIFY(s.size() == 3);
  {
    struct A
    {
      operator Hashable()
      {
        throw nullptr;
      }
    };
    basic_variant<std::allocator<int>,int, Hashable> v;
    try
      {
        v.emplace<1>(A{});
      }
    catch (nullptr_t)
      {
      }
    VERIFY(v.valueless_by_exception());
    VERIFY(s.insert(v).second);
    VERIFY(s.size() == 4);
    VERIFY(!s.insert(v).second);
  }
}

void test_valueless_by_exception()
{
  {
    AlwaysThrow a;
    bool caught = false;
    try
      {
	basic_variant<std::allocator<int>,int, AlwaysThrow> v(a);
      }
    catch (nullptr_t)
      {
	caught = true;
      }
    VERIFY(caught);
  }
  {
    AlwaysThrow a;
    bool caught = false;
    try
      {
	basic_variant<std::allocator<int>,int, AlwaysThrow> v(a);
      }
    catch (nullptr_t)
      {
	caught = true;
      }
    VERIFY(caught);
  }
  {
    basic_variant<std::allocator<int>,int, AlwaysThrow> v;
    bool caught = false;
    try
      {
	AlwaysThrow a;
	v = a;
      }
    catch (nullptr_t)
      {
	caught = true;
      }
    VERIFY(caught);
    VERIFY(v.valueless_by_exception());
  }
  {
    basic_variant<std::allocator<int>,int, AlwaysThrow> v;
    bool caught = false;
    try
      {
	v = AlwaysThrow{};
      }
    catch (nullptr_t)
      {
	caught = true;
      }
    VERIFY(caught);
    VERIFY(v.valueless_by_exception());
  }
}

int main()
{
  default_ctor();
  copy_ctor();
  move_ctor();
  arbitrary_ctor();
  in_place_index_ctor();
  in_place_type_ctor();
  copy_assign();
  move_assign();
  arbitrary_assign();
  dtor();
  emplace();
  test_get();
  test_relational();
  test_swap();
  test_visit();
  test_hash();
  test_valueless_by_exception();
}
