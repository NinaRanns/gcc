// { dg-additional-options "-fmodules" }

// Nor to a module-import-declaration: a standard, an unknown and a GNU
// attribute are each ignored, with a warning.

import M [[deprecated]];		// { dg-warning "attribute ignored" }
import M [[foo::bar]];			// { dg-warning "attribute ignored" }
import M __attribute__((unused));	// { dg-warning "attribute ignored" }

int g () { return f (); }
