// { dg-do compile { target c++11 } }
// { dg-options "-fprofiles" }

// Review item [markup.enforce.list]: [decl.profile.enforce]/2 and
// [decl.profile.require]/1 allow a single ( profile-designator ), but the
// examples of [decl.profile.require]/2 rely on a list, as in
// enforce(lib::hardened, acme::type).  The prototype follows the examples:
// a comma-separated list is accepted, and each designator in it is recorded
// as if it had been enforced on its own.

[[profiles::enforce(lib::hardened, acme::type)]];
[[profiles::enforce(lib::hardened)]];		// the same tokens again: OK
[[profiles::enforce(other, more::names(fortify: 3), last)]];

// Each designator of a list is subject to the repetition rule of
// [decl.profile.enforce]/3.
[[profiles::enforce(acme::type(fortify: 3))]];	// { dg-error "already loaded with different arguments" }
[[profiles::enforce(more::names)]];		// { dg-error "already loaded with different arguments" }
