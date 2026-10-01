// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// An empty-declaration ends the import region of a named module, with
// -fprofiles as without ([module.import]/1): there is no profiles content.

export module PB;
import PA;
;
import PA;	// { dg-error "post-module-declaration imports must be contiguous" }
// { dg-message "perhaps insert a line break" "" { target *-*-* } .-1 }
