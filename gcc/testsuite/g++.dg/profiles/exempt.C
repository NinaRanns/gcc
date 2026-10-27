// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Grammar of profiles::exempt ([decl.profile.exempt]/1):
//   ( profile-name , header-search : string-literal )
//   ( profile-name , header-search : string-literal ,
//     justification : string-literal )
//   header-search: angle_header | quote_header

[[profiles::exempt(acme::type, angle_header: "unistd.h")]];
[[profiles::exempt(acme::type, quote_header: "lead-ffi.h")]];
[[profiles::exempt(std::type, angle_header: "x/y.h", justification: "legacy FFI")]];
[[profiles::exempt(acme, quote_header: u8"wide.h")]];

[[profiles::exempt]];					// { dg-error "requires argument" }
[[profiles::exempt()]];					// { dg-error "expected" }
[[profiles::exempt(acme::type)]];			// { dg-error "expected" }
[[profiles::exempt(acme::type, "unistd.h")]];		// { dg-error "expected 'angle_header' or 'quote_header'" }
[[profiles::exempt(acme::type, system_header: "unistd.h")]];	// { dg-error "expected 'angle_header' or 'quote_header'" }
[[profiles::exempt(acme::type, angle_header "unistd.h")]];	// { dg-error "expected" }
[[profiles::exempt(acme::type, angle_header: unistd)]];	// { dg-error "expected string-literal" }
[[profiles::exempt(acme::type, angle_header: "x.h", why: "y")]];	// { dg-error "expected 'justification'" }
[[profiles::exempt(acme::type, angle_header: "x.h", justification: 3)]];	// { dg-error "expected string-literal" }
[[profiles::exempt(acme::type, angle_header: "x.h", justification: "a", rule: "b")]];	// { dg-error "expected" }
[[profiles::exempt(::type, angle_header: "x.h")]];	// { dg-error "expected" }
