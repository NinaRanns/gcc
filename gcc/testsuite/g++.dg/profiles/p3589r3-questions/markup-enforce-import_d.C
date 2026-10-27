// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }
// { dg-module-cmi !Imp2 }

// And before the import: the enforce itself is allowed right after the
// module-declaration ([decl.profile.enforce]/1), but [module.import]/1 puts
// the imports before every other declaration, so the enforce ends the import
// region and the import is rejected.  In a module unit with imports, no
// free-standing enforce can be written in the purview.
export module Imp2;
[[profiles::enforce(std::init)]];
import Bob;				// { dg-error "post-module-declaration imports must be contiguous" }
// { dg-message "perhaps insert a line break" "" { target *-*-* } .-1 }
