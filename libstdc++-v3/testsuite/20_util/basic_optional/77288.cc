// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 }  }

// Copyright (C) 2016-2020 Free Software Foundation, Inc.
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

#include "../../../include/std/basic_optional"
#include <any>

using std::basic_optional;

#include <testsuite_hooks.h>

void test01()
{
    basic_optional<basic_optional<int>> nested_element;
    basic_optional<int> element = {};
    nested_element = element;
    VERIFY(nested_element);
}

template <class T>
struct service_result
{
  static basic_optional<T> get_result()
  {
    T sr;
    return sr;
  }

  static basic_optional<T> get_result_with_cond(bool cond)
  {
    if (cond)
      return T{};
    return {};
  }
};

void test02()
{
  VERIFY(service_result<int>::get_result());
  VERIFY(service_result<basic_optional<int>>::get_result());
  VERIFY(service_result<int>::get_result_with_cond(true));
  VERIFY(service_result<basic_optional<int>>::get_result_with_cond(true));
  VERIFY(!service_result<int>::get_result_with_cond(false));
  VERIFY(!service_result<basic_optional<int>>::get_result_with_cond(false));
}

struct Widget
{
  Widget(int) {}
  Widget(basic_optional<int>) {}
};

void foo( basic_optional<int>)
{}


void test03()
{
  basic_optional<Widget> w;
  w = basic_optional<int>();
  VERIFY(w);
  static_assert(!std::is_assignable_v<basic_optional<Widget>&,
		basic_optional<short>>);;
  w = basic_optional<basic_optional<int>>();
  VERIFY(!w);
  static_assert(!std::is_assignable_v<basic_optional<Widget>&,
		basic_optional<basic_optional<short>>>);;

  basic_optional<Widget> w2{basic_optional<int>()};
  VERIFY(w2);
  basic_optional<Widget> w3  = basic_optional<int>();
  VERIFY(w3);
  basic_optional<Widget> w4{basic_optional<short>()};
  VERIFY(w4);
  static_assert(!std::is_convertible_v<basic_optional<short>&&, basic_optional<Widget>>);

  basic_optional<Widget> w6{basic_optional<basic_optional<int>>()};
  VERIFY(!w6);
  basic_optional<Widget> w7  = basic_optional<basic_optional<int>>();
  VERIFY(!w7);
  basic_optional<int> hjh = basic_optional<short>();
  foo(basic_optional<short>());
  basic_optional<Widget> w8{basic_optional<basic_optional<short>>()};
  VERIFY(!w8);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<short>>&&,
		basic_optional<Widget>>);
  basic_optional<Widget> w10{basic_optional<basic_optional<short>>(10)};
  VERIFY(w10);
  basic_optional<Widget> w11 = std::nullopt;
  VERIFY(!w11);
  basic_optional<Widget> w12 = {};
  VERIFY(!w12);
  basic_optional<Widget> w13{std::nullopt};
  VERIFY(!w13);
  basic_optional<Widget> w14;
  w14 = {};
  VERIFY(!w14);
}

struct Widget2
{
  Widget2(int) {}
  Widget2(basic_optional<int>) {}
  Widget2& operator=(int) {return *this;}
  Widget2& operator=(basic_optional<int>) {return *this;}
};

void test04()
{
  basic_optional<Widget2> w;
  w = basic_optional<int>();
  VERIFY(w);
  w = basic_optional<short>();
  VERIFY(w);
  w = basic_optional<basic_optional<int>>();
  VERIFY(!w);
  w = basic_optional<basic_optional<short>>();
  VERIFY(!w);
  w = basic_optional<basic_optional<short>>(10);
  basic_optional<Widget2> w2 = std::nullopt;
  VERIFY(!w2);
  basic_optional<Widget2> w3 = {};
  VERIFY(!w3);
  basic_optional<Widget2> w4{std::nullopt};
  VERIFY(!w4);
  basic_optional<Widget2> w5;
  w5 = {};
  VERIFY(!w5);
}

struct Thingy
{
  Thingy(int) {}
  Thingy(Widget) {}
};

void test05()
{
  basic_optional<Thingy> ot;

  static_assert(!std::is_assignable_v<basic_optional<Thingy>&,
		basic_optional<int>>);
  static_assert(std::is_assignable_v<basic_optional<Thingy>&,
		basic_optional<short>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy>&,
		basic_optional<basic_optional<int>>>);
  ot = basic_optional<Widget>();
  VERIFY(!ot);
  basic_optional<Thingy> ot2{basic_optional<int>()};
  VERIFY(ot2);
  static_assert(!std::is_convertible_v<basic_optional<int>&&,
		basic_optional<Thingy>>);
  basic_optional<Thingy> ot3{basic_optional<short>()};
  VERIFY(!ot3);
  basic_optional<Thingy> ot4 = basic_optional<short>();
  VERIFY(!ot4);

  basic_optional<Thingy> ot5{basic_optional<basic_optional<int>>()};
  VERIFY(!ot5);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<int>>&&,
		basic_optional<Thingy>>);

  basic_optional<Thingy> ot7{basic_optional<Widget>()};
  VERIFY(!ot7);
  basic_optional<Thingy> ot8 = basic_optional<Widget>();
  VERIFY(!ot8);
  static_assert(!std::is_constructible_v<basic_optional<Thingy>,
		basic_optional<basic_optional<short>>>);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<short>>,
		basic_optional<Thingy>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy>&,
		basic_optional<basic_optional<short>>>);
  basic_optional<Thingy> ot9 = std::nullopt;
  VERIFY(!ot9);
  basic_optional<Thingy> ot10 = {};
  VERIFY(!ot10);
  basic_optional<Thingy> ot11{std::nullopt};
  VERIFY(!ot11);
  basic_optional<Thingy> ot12;
  ot12 = {};
  VERIFY(!ot12);
}

struct RvalueConstructible
{
  RvalueConstructible(int) {}
  RvalueConstructible(basic_optional<int>&&) {}
};

void test06()
{
  basic_optional<int> oi;
  basic_optional<RvalueConstructible> ori;
  static_assert(!std::is_assignable_v<basic_optional<RvalueConstructible>&,
		basic_optional<int>&>);
  ori = std::move(oi);
  VERIFY(ori);

  basic_optional<basic_optional<int>> ooi;
  static_assert(!std::is_assignable_v<basic_optional<RvalueConstructible>&,
		basic_optional<basic_optional<int>>&>);
  ori = std::move(ooi);
  VERIFY(!ori);

  static_assert(!std::is_constructible_v<basic_optional<RvalueConstructible>,
		basic_optional<int>&>);
  static_assert(!std::is_convertible_v<basic_optional<int>&,
		basic_optional<RvalueConstructible>>);

  basic_optional<RvalueConstructible> ori2(std::move(oi));
  VERIFY(ori2);
  basic_optional<RvalueConstructible> ori3 = std::move(oi);
  VERIFY(ori3);

  static_assert(!std::is_constructible_v<basic_optional<RvalueConstructible>,
		basic_optional<basic_optional<int>>&>);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<int>>&,
		basic_optional<RvalueConstructible>>);
  basic_optional<RvalueConstructible> ori6(std::move(ooi));
  VERIFY(!ori6);
  basic_optional<RvalueConstructible> ori7 = std::move(ooi);
  VERIFY(!ori7);
  basic_optional<RvalueConstructible> ori8 = std::nullopt;
  VERIFY(!ori8);
  basic_optional<RvalueConstructible> ori9 = {};
  VERIFY(!ori9);
  basic_optional<RvalueConstructible> ori10{std::nullopt};
  VERIFY(!ori10);
  basic_optional<RvalueConstructible> ori11;
  ori11 = {};
  VERIFY(!ori11);
}

struct Thingy2
{
  Thingy2(int) {}
  explicit Thingy2(basic_optional<int>) {}
  Thingy2(Widget) {}
};

void test07()
{
  basic_optional<Thingy2> ot{basic_optional<int>{}};
  VERIFY(ot);
  static_assert(!std::is_convertible_v<basic_optional<int>,
		basic_optional<Thingy2>>);
  basic_optional<Thingy2> ot2{basic_optional<short>{}};
  VERIFY(ot2);
  static_assert(!std::is_convertible_v<basic_optional<short>,
		basic_optional<Thingy2>>);
  basic_optional<Thingy2> ot3{basic_optional<basic_optional<int>>{}};
  VERIFY(!ot3);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<int>>,
		basic_optional<Thingy2>>);
  basic_optional<Thingy2> ot4{basic_optional<basic_optional<short>>{}};
  VERIFY(!ot4);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<short>>,
		basic_optional<Thingy2>>);

  basic_optional<Thingy2> ot5{basic_optional<Widget>{}};
  VERIFY(!ot5);
  basic_optional<Thingy2> ot6 = basic_optional<Widget>();
  VERIFY(!ot6);

  static_assert(!std::is_assignable_v<basic_optional<Thingy2>&,
		basic_optional<int>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy2>&,
		basic_optional<short>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy2>&,
		basic_optional<basic_optional<int>>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy2>&,
		basic_optional<basic_optional<short>>>);
  basic_optional<Thingy2> ot7;
  ot = basic_optional<Widget>();
  VERIFY(!ot7);
  basic_optional<Thingy2> ot8 = std::nullopt;
  VERIFY(!ot8);
  basic_optional<Thingy2> ot9 = {};
  VERIFY(!ot9);
  basic_optional<Thingy2> ot10{std::nullopt};
  VERIFY(!ot10);
  basic_optional<Thingy2> ot11;
  ot11 = {};
  VERIFY(!ot11);
}

struct Thingy3
{
  Thingy3(int) {}
  template<class... Args,
	   std::enable_if_t<std::is_constructible_v<Widget, Args&&...>,
			    bool> = true>
  explicit Thingy3(Args&&... args) {}
  Thingy3(Widget) {}
};

void test08()
{
  basic_optional<Thingy3> ot{basic_optional<int>{}};
  VERIFY(ot);
  static_assert(!std::is_convertible_v<basic_optional<int>,
		basic_optional<Thingy3>>);
  basic_optional<Thingy3> ot2{basic_optional<short>{}};
  VERIFY(ot2);
  static_assert(!std::is_convertible_v<basic_optional<short>,
		basic_optional<Thingy3>>);
  basic_optional<Thingy3> ot3{basic_optional<basic_optional<int>>{}};
  VERIFY(!ot3);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<int>>,
		basic_optional<Thingy3>>);
  basic_optional<Thingy3> ot4{basic_optional<basic_optional<short>>{}};
  VERIFY(!ot4);
  static_assert(!std::is_convertible_v<basic_optional<basic_optional<short>>,
		basic_optional<Thingy3>>);

  basic_optional<Thingy3> ot5{basic_optional<Widget>{}};
  VERIFY(!ot5);
  basic_optional<Thingy3> ot6 = basic_optional<Widget>();
  VERIFY(!ot6);

  static_assert(!std::is_assignable_v<basic_optional<Thingy3>&,
		basic_optional<int>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy3>&,
		basic_optional<short>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy3>&,
		basic_optional<basic_optional<int>>>);
  static_assert(!std::is_assignable_v<basic_optional<Thingy3>&,
		basic_optional<basic_optional<short>>>);
  basic_optional<Thingy3> ot7;
  ot = basic_optional<Widget>();
  VERIFY(!ot7);
  basic_optional<Thingy3> ot8 = std::nullopt;
  VERIFY(!ot8);
  basic_optional<Thingy3> ot9 = {};
  VERIFY(!ot9);
  basic_optional<Thingy3> ot10{std::nullopt};
  VERIFY(!ot10);
  basic_optional<Thingy3> ot11;
  ot11 = {};
  VERIFY(!ot11);
}

void test09()
{
  std::any a = 42;
  basic_optional<std::any> oa2 = a;
  VERIFY(oa2);
  VERIFY(std::any_cast<int>(*oa2) == 42);
  basic_optional<std::any> oa3 = oa2;
  VERIFY(oa3);
  VERIFY(std::any_cast<int>(*oa3) == 42);
  basic_optional<std::any> oa4{oa2};
  VERIFY(oa4);
  VERIFY(std::any_cast<int>(*oa4) == 42);
  basic_optional<std::any> oa5(oa2);
  VERIFY(oa5);
  VERIFY(std::any_cast<int>(*oa5) == 42);
  basic_optional<std::any> oa6;
  VERIFY(!oa6);
  basic_optional<std::any> oa7 = oa6;
  VERIFY(!oa7);
  basic_optional<std::any> oa8{oa6};
  VERIFY(!oa8);
  basic_optional<std::any> oa9(oa6);
  VERIFY(!oa9);
}

void test10()
{
  struct X {};
  basic_optional<int> oi(std::in_place);
  oi = {};
  VERIFY(oi.has_value() == false);
  basic_optional<X> ot(std::in_place);
  ot = {};
  VERIFY(ot.has_value() == false);
  basic_optional<int> oi2(std::in_place);
  short int si = 6;
  oi2 = si;
}

int main()
{
  test01();
  test02();
  test03();
  test04();
  test05();
  test06();
  test07();
  test08();
  test09();
  test10();
}
