// { dg-do compile { target c++20 } }
// { dg-additional-options "-fmodules-ts -fprofiles" }

// Review item [markup.enforce.import], part of [markup.module.gmf]: an
// import is a declaration, so [decl.profile.enforce]/1 puts every
// free-standing enforce before it, even in a translation unit that is not a
// module unit, although the enforce does not apply to the imported module.

import Bob;
[[profiles::enforce(std::init)]];	// { dg-error "may only appear on a module-declaration" }
