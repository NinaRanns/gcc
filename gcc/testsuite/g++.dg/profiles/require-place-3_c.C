// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Valid placement (import-declaration) with ill-formed require syntax.

import PlaceOk [[profiles::require()]]; // { dg-error "" }
import PlaceOk [[profiles::require(std::init,)]]; // { dg-error "" }
import PlaceOk [[profiles::require(std::init,, acme::type)]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme())]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(fortify: 3,))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(a,, b))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(+))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(::))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(*))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme((a)))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(name:))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(name: ))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(name: ,))]]; // { dg-error "" }
import PlaceOk [[profiles::require(::type)]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme::)]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme::type::)]]; // { dg-error "" }
import PlaceOk [[profiles::require(int)]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(std::rule: "x"))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(rule: std::x))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(a::b))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(x: [))]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(class: x))]]; // { dg-error "" }
import PlaceOk [[profiles::require(std::type +)]]; // { dg-error "" }
import PlaceOk [[profiles::require(acme(x: ([({ })]))"y"))]]; // { dg-error "" }
import PlaceOk [[profiles::require(std::init]]; // { dg-error "" }
