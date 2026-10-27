// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Review item [markup.exempt.position]: P3589R3 does not say where a
// profiles::exempt attribute may appear.  An exemption targets #include
// directives, which may appear anywhere, and there is no attribute position
// on an #include line.  The behaviour chosen here: an exemption is an
// attribute-declaration, or appertains to an otherwise empty statement.

// Accepted: attribute-declarations.
[[profiles::exempt(acme::type, angle_header: "unistd.h")]];
namespace N {
  [[profiles::exempt(acme::type, quote_header: "n.h")]];
}
extern "C++" {
  [[profiles::exempt(acme::type, quote_header: "l.h")]];
}

// Accepted: otherwise empty statements.
void
f ()
{
  [[profiles::exempt(acme::type, quote_header: "f.h")]];
 lab:
  [[profiles::exempt(acme::type, quote_header: "g.h")]];
  {
    [[profiles::exempt(acme::type, quote_header: "h.h")]];
  }
}

// Rejected: on a declaration, its declarator or its type.
[[profiles::exempt(acme::type, quote_header: "a.h")]] void g (); // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
void h [[profiles::exempt(acme::type, quote_header: "b.h")]] (); // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
int [[profiles::exempt(acme::type, quote_header: "c.h")]] i; // { dg-error "may only appear in an attribute-declaration or on an empty statement" }

// Rejected: in a class.
struct S
{
  [[profiles::exempt(acme::type, quote_header: "d.h")]] int m; // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
};

// Rejected: on a statement that is not empty, or a block-scope declaration.
int
k (int x)
{
  [[profiles::exempt(acme::type, quote_header: "e.h")]] int y = x; // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
  [[profiles::exempt(acme::type, quote_header: "e.h")]] { } // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
  [[profiles::exempt(acme::type, quote_header: "e.h")]] return y; // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
}

// Rejected: on a parameter.
void p ([[profiles::exempt(acme::type, quote_header: "q.h")]] int); // { dg-error "may only appear in an attribute-declaration or on an empty statement" }
