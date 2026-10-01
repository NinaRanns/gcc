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
  PAK_EXEMPT = 4,
};

/* Set up the framework's own per-TU state under -fprofiles: what the
   framework records from the source.  How profiles are told about any of it
   is a separate question, not settled.  */
extern void init_profiles_framework (void);

/* An #include, #include_next or #import directive of this translation unit,
   as written: [decl.profile.exempt]/1 matches an exemption against it.  */

struct GTY(()) profile_include {
  /* The directive.  */
  location_t location;
  /* The header-name as written, without its delimiters.  */
  const char *header;
  /* True for <>, false for "".  */
  bool angle_p;
};

/* A profile-exemption of this translation unit ([decl.profile.exempt]).  */

struct GTY(()) profile_exemption {
  tree profile_name;
  /* The STRING_CST naming the header.  */
  tree header;
  /* True for angle_header, false for quote_header.  */
  bool angle_p;
  location_t location;
};

/* Record the exemption of the header HEADER (a STRING_CST; ANGLE_P for
   angle_header) from the profile NAME, appearing at LOCATION.  It covers the
   #include directives after it that name the header as written, with the
   same delimiters; one that precedes it is diagnosed.  */
extern void add_profile_exemption (tree name, tree header, bool angle_p,
				   location_t location);

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

/* TREE_LIST of all profile-enforcements recorded in this translation unit
   (TREE_PURPOSE = profile-name, TREE_VALUE = argument clause), in order, or
   NULL_TREE if none.  Fresh list nodes; argument clauses are shared.  */
extern tree profile_enforcements_as_list (void);

/* From ATTRS, extract the designators of any profiles::enforce attributes
   into a single TREE_LIST of enforcements (TREE_PURPOSE = profile-name,
   TREE_VALUE = argument clause).  If REMOVE_P, also unlink those attributes
   from *ATTRS.  */
extern tree extract_profile_enforcements (tree *attrs, bool remove_p);

/* From ATTRS, extract the designators of any profiles::require attributes
   into a single TREE_LIST of requirements (TREE_PURPOSE = profile-name,
   TREE_VALUE = argument clause).  If REMOVE_P, also unlink those attributes
   from *ATTRS.  */
extern tree extract_profile_requirements (tree *attrs, bool remove_p);

/* Return a malloc'd spelling of ARGS (a DEFERRED_PARSE argument clause,
   a STRING_CST of a previously serialized spelling, or NULL_TREE).
   Caller must free.  */
extern char *profile_enforcement_args_spelling (tree args);

/* Convert DEFERRED_PARSE argument clauses in ENFORCEMENTS to STRING_CST
   spellings so they remain valid after the parser's token caches are gone.
   No-op if already STRING_CST or NULL.  */
extern tree materialize_profile_enforcement_args (tree enforcements);

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
