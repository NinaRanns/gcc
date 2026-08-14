/* Support for the C++ profiles framework.
   Copyright (C) 2026 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_CP_PROFILES_H
#define GCC_CP_PROFILES_H

#include <cstdint>

/* Profiles attribute kind.  */

enum profiles_attr_kind : uint16_t {
  PAK_INVALID = 0,
  PAK_ENFORCE = 1,
  PAK_REQUIRE = 2,
  PAK_SUPPRESS = 3,
};

/* Spec for a concrete profile-name, analogous to attribute_spec for a
   concrete attribute name.  NAME is the full profile-name spelling as it
   appears in a designator (e.g. "acme::bob" or "std::init"), not a bare
   root with catch-all dispatch.

   ENFORCE is invoked when profiles::enforce mentions this name.  NAME is
   the profile-name identifier, ARGS is the designator's argument clause
   (a DEFERRED_PARSE, or NULL_TREE when there was none), and LOC is where
   the enforcement appeared.  Return true to accept the enforcement (the
   framework records it); return false to reject it (it is not recorded).
   ENFORCE may be NULL when no special handling is needed beyond claiming
   the name.  The plugin otherwise uses ordinary GCC plugin callbacks for
   checking; it owns whatever "enabled" state it derives from ENFORCE.  */

struct profile_spec
{
  const char *name;
  bool (*enforce) (tree name, tree args, location_t loc);
};

/* Register PROFILE with the C++ profiles framework.  Typically called from
   a PLUGIN_PROFILES callback.  PROFILE must remain live for the rest of the
   compilation.  Duplicate names are diagnosed.  */
extern void register_profile (const profile_spec *profile);

/* If a profile with the given NAME has been registered, return its spec;
   otherwise return NULL.  */
extern const profile_spec *lookup_profile_spec (tree name);

/* Invoke PLUGIN_PROFILES so plugins can call register_profile.  */
extern void init_profiles (void);

/* A profile-enforcement seen in the current translation unit.  More
   information will be added here as the profiles framework grows.  */

struct GTY(()) profile_enforcement {
  /* The profile-enforcement designator as a single tree: TREE_PURPOSE is the
     profile-name and TREE_VALUE is the deferred argument clause
     (a DEFERRED_PARSE), or NULL_TREE when there was no argument clause.  */
  tree enforcement;
  /* Where the enforcement appeared, for diagnostics.  */
  location_t location;
};

/* Record ENFORCEMENT (a TREE_LIST whose TREE_PURPOSE is the profile-name and
   whose TREE_VALUE is the deferred argument clause), appearing at LOCATION, as
   seen in the current translation unit.  If a profile-enforcement with the same
   name was already recorded, return it when the arguments match, or report an
   error and return NULL when they differ; otherwise store and return the new
   (or existing matching) profile_enforcement.  */
extern profile_enforcement *add_profile_enforcement (tree enforcement,
						    location_t location);

/* If a profile-enforcement with the given NAME has been recorded in the
   current translation unit, return it; otherwise return NULL.  */
extern profile_enforcement *profile_enforcement_exists (tree name);

/* From ATTRS, extract the designators of any profiles::enforce attributes
   into a single TREE_LIST of enforcements (TREE_PURPOSE = profile-name,
   TREE_VALUE = argument clause).  */
extern tree extract_profile_enforcements (tree attrs);

/* From ATTRS, extract the designators of any profiles::require attributes
   into a single TREE_LIST of requirements (TREE_PURPOSE = profile-name,
   TREE_VALUE = argument clause).  */
extern tree extract_profile_requirements (tree attrs);

/* Return a malloc'd spelling of ARGS (a DEFERRED_PARSE argument clause,
   a STRING_CST of a previously serialized spelling, or NULL_TREE).
   Caller must free.  */
extern char *profile_enforcement_args_spelling (tree args);

/* Convert DEFERRED_PARSE argument clauses in ENFORCEMENTS to STRING_CST
   spellings so they remain valid after the parser's token caches are gone.  */
extern tree stabilize_profile_enforcements (tree enforcements);

/* True if the two profile-designator argument clauses match for the purpose
   of matching a profiles::require against an exported profiles::enforce.  */
extern bool profile_designator_args_match (tree a, tree b);

/* For each designator in REQUIRED, ensure EXPORTED contains a matching
   enforce designator (same profile-name and matching argument clause).
   MODULE_NAME is used in diagnostics; LOC is the import location.  */
extern void check_profile_requirements (location_t loc,
					const char *module_name,
					tree required, tree exported);

/* Write a one-line description of ENFORCEMENT (a TREE_LIST of profile-name
   and optional deferred argument clause) to STREAM.  */
extern void dump_profile_enforcement (FILE *stream, tree enforcement);

#endif /* GCC_CP_PROFILES_H */
