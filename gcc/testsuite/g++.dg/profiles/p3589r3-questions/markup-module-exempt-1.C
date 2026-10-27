// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi ExGmf }

// Review item [markup.module.exempt], part of [markup.module.gmf]: an
// exemption must precede the #include it exempts, so in a module unit it
// belongs in the global module fragment, where [cpp.pre] allows only
// preprocessing directives.  Legacy headers, the stated use for exempt, are
// what the fragment is for.  The prototype accepts a profiles-only
// attribute-declaration there ([disp.module.gmf] (d)).

module;
[[profiles::exempt(acme::type, angle_header: "legacy.h")]];
export module ExGmf;
export int f ();
