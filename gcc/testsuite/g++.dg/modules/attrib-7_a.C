// { dg-additional-options "-fmodules" }
// { dg-module-cmi M }

// No attribute applies to a module-declaration ([dcl.attr.grammar]/7, /8),
// so any there is ignored, with a warning.

export module M [[deprecated]];	// { dg-warning "attribute ignored" }
export int f ();
