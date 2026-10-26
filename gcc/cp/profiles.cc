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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "cp-tree.h"
#include "stringpool.h"
#include "diagnostic.h"
#include "attribs.h"
#include "cpplib.h"
#include "parser.h"
#include "tree-pretty-print.h"
#include "dumpfile.h"
#include "langhooks.h"
#include "plugin.h"
#include "profiles.h"

/* Registered profile specs (plugin-owned storage).  Not GC roots: plugins
   keep the profile_spec objects live for the compilation.  */

static vec<const profile_spec *> profile_registry;

/* All profile-enforcements found in the current translation unit, in the
   order they were seen.  Each element is GC-allocated so pointers remain
   valid after further pushes.  */

static GTY(()) vec<profile_enforcement *, va_gc> *profile_enforcements;

/* Register PROFILE under its full profile-name.  */

void
register_profile (const profile_spec *profile)
{
  gcc_assert (profile != NULL && profile->name != NULL);

  for (unsigned i = 0; i < profile_registry.length (); ++i)
    if (!strcmp (profile_registry[i]->name, profile->name))
      {
	error ("duplicate profile %qs", profile->name);
	return;
      }

  profile_registry.safe_push (profile);
}

/* Return the registered spec for NAME, or NULL.  */

const profile_spec *
lookup_profile_spec (tree name)
{
  if (name == NULL_TREE || !identifier_p (name))
    return NULL;

  const char *n = IDENTIFIER_POINTER (name);
  for (unsigned i = 0; i < profile_registry.length (); ++i)
    if (!strcmp (profile_registry[i]->name, n))
      return profile_registry[i];
  return NULL;
}

/* Give plugins a chance to call register_profile.  */

void
init_profiles (void)
{
  invoke_plugin_callbacks (PLUGIN_PROFILES, NULL);
}

/* The #include directives of this translation unit, in order, and its
   exemptions.  */
static GTY(()) vec<profile_include, va_gc> *profile_includes;
static GTY(()) vec<profile_exemption, va_gc> *profile_exemptions;

/* -fdump-lang-profiles.  */
int profiles_dump_id;

/* cpplib's include callback: note each directive as written.  */

static void
profiles_cb_include (cpp_reader *, location_t location,
		     const unsigned char *, const char *header,
		     int angle_brackets, const cpp_token **)
{
  /* LOCATION is the start of the directive's line; point at its `#'.  */
  location = linemap_position_for_loc_and_offset (line_table, location, 1);
  profile_include inc = { location, ggc_strdup (header),
			  angle_brackets != 0 };
  vec_safe_push (profile_includes, inc);
}

/* The handler for profiles:: attributes that reach decl_attributes.  Only
   suppress does: the others are stripped or diagnosed where they are
   parsed.  A suppress on a declaration, a label included, is kept with the
   entity declared: in DECL_ATTRIBUTES, or for a class or enumeration in its
   TYPE_ATTRIBUTES.  On any other type it is ignored, since there it does
   not appertain to a declaration ([decl.profile.suppress]/3).  */

static tree
handle_profiles_attribute (tree *node, tree name, tree args, int,
			   bool *no_add_attrs)
{
  if (!is_attribute_p ("suppress", name))
    {
      *no_add_attrs = true;
      return NULL_TREE;
    }
  if (!DECL_P (*node) && !OVERLOAD_TYPE_P (*node))
    {
      warning (OPT_Wattributes, "%<profiles::suppress%> on a type is ignored");
      *no_add_attrs = true;
      return NULL_TREE;
    }
  if (FILE *dump = dump_begin (profiles_dump_id, NULL))
    {
      tree decl = TYPE_P (*node) ? TYPE_NAME (*node) : *node;
      fprintf (dump, "suppress %s on %s %s at line %d\n",
	       args ? IDENTIFIER_POINTER (TREE_PURPOSE (args)) : "?",
	       get_tree_code_name (TREE_CODE (*node)),
	       decl && DECL_NAME (decl)
	       ? lang_hooks.decl_printable_name (decl, 0) : "<anonymous>",
	       decl ? LOCATION_LINE (DECL_SOURCE_LOCATION (decl)) : 0);
      dump_end (profiles_dump_id, dump);
    }
  return NULL_TREE;
}

static const attribute_spec profiles_attributes[] =
{
  { "enforce", 0, -1, false, false, false, false,
    handle_profiles_attribute, NULL },
  { "require", 0, -1, false, false, false, false,
    handle_profiles_attribute, NULL },
  { "suppress", 0, -1, false, false, false, false,
    handle_profiles_attribute, NULL },
  { "exempt", 0, -1, false, false, false, false,
    handle_profiles_attribute, NULL },
};

static const scoped_attribute_specs profiles_attribute_table =
{
  "profiles", { profiles_attributes }
};

/* See profiles.h.  GCC lexes the whole translation unit before parsing it,
   so every directive is known by the time an exemption is parsed.  The
   profiles namespace is registered only under -fprofiles, so that without
   it profiles:: is an unknown namespace, as before.  */

void
init_profiles_framework (void)
{
  cpp_callbacks *cb = cpp_get_callbacks (parse_in);
  if (!cb->include)
    cb->include = profiles_cb_include;
  register_scoped_attributes (profiles_attribute_table);
}

/* See profiles.h.  */

void
add_profile_exemption (tree name, tree header, bool angle_p,
		       location_t location)
{
  location = linemap_resolve_location (line_table, location,
				       LRK_MACRO_EXPANSION_POINT, NULL);
  for (const profile_exemption &e : profile_exemptions)
    if (e.location == location)
      return;
  profile_exemption ex = { name, header, angle_p, location };
  vec_safe_push (profile_exemptions, ex);

  const char *h = TREE_STRING_POINTER (header);
  char open = angle_p ? '<' : '"', close = angle_p ? '>' : '"';
  FILE *dump = dump_begin (profiles_dump_id, NULL);
  if (dump)
    fprintf (dump, "exempt %s from %c%s%c at line %d\n",
	     IDENTIFIER_POINTER (name), open, h, close,
	     LOCATION_LINE (location));

  bool warned = false;
  for (const profile_include &inc : profile_includes)
    {
      if (inc.angle_p != angle_p || strcmp (inc.header, h))
	continue;
      if (inc.location < location)
	{
	  /* Only the directives after it ([markup.exempt.position]).  */
	  if (!warned)
	    {
	      auto_diagnostic_group d;
	      if (warning_at (location, OPT_Wattributes,
			      "exemption of %<%c%s%c%> follows an %<#include%>"
			      " of it, which it does not cover", open, h, close))
		inform (inc.location, "included here");
	      warned = true;
	    }
	}
      else if (dump)
	{
	  expanded_location xl = expand_location (inc.location);
	  fprintf (dump, "  covers the #include at %s:%d\n",
		   lbasename (xl.file), xl.line);
	}
    }
  if (dump)
    dump_end (profiles_dump_id, dump);
}

/* Suppress the profile named NAME.  */

void
suppress_profile (tree name, location_t location)
{
  const profile_spec *spec = lookup_profile_spec (name);

  if (!spec)
    {
      error_at (location,
		"profile %qE cannot be suppressed because it is not loaded", name);
      return;
    }

  // profile spec needs to hold functions registered for the supression
}

/* Return true if the tree values associated with two tokens are the same.
   Token values are not general C++ expressions: in particular, string
   tokens produced under C_LEX_STRING_NO_JOIN are STRING_CSTs with a NULL
   TREE_TYPE, so cp_tree_equal (which type-checks constants via comptypes)
   must not be used on them.  */

static bool
profile_token_values_equal (tree a, tree b)
{
  if (a == b)
    return true;
  if (a == NULL_TREE || b == NULL_TREE)
    return false;
  if (TREE_CODE (a) != TREE_CODE (b))
    return false;

  switch (TREE_CODE (a))
    {
    case STRING_CST:
      return (TREE_STRING_LENGTH (a) == TREE_STRING_LENGTH (b)
	      && !memcmp (TREE_STRING_POINTER (a), TREE_STRING_POINTER (b),
			  TREE_STRING_LENGTH (a)));

    case INTEGER_CST:
      return tree_int_cst_equal (a, b);

    case REAL_CST:
      return real_identical (&TREE_REAL_CST (a), &TREE_REAL_CST (b));

    default:
      /* Identifiers and other token values are pointer-canonical.  */
      return false;
    }
}

/* True if TOKEN is a literal: a number, or a character or string literal.
   The identity of a literal in a profile-argument is its spelling, not
   its value ([decl.profile.enforce]/3 compares token sequences), so 3 and
   0x3 are different arguments.  */

static bool
profile_literal_token_p (const cp_token *token)
{
  switch (token->type)
    {
    case CPP_NUMBER:
    case CPP_CHAR:
    case CPP_WCHAR:
    case CPP_CHAR16:
    case CPP_CHAR32:
    case CPP_UTF8CHAR:
    case CPP_STRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_WSTRING:
    case CPP_UTF8STRING:
      return true;
    default:
      return false;
    }
}

/* Return the source spelling of TOKEN, which the caller must free, or NULL
   if the source text is not available.  */

static char *
profile_token_source_spelling (const cp_token *token)
{
  return get_source_text_between (global_dc->get_file_cache (),
				  get_start (token->location),
				  get_finish (token->location));
}

/* Return true if the deferred profile-argument clauses A and B consist of the
   same token sequence.  A and B are each the ARGS of a profile-enforcement: a
   DEFERRED_PARSE holding the cached argument tokens, or NULL_TREE when the
   designator had no argument clause.  Token locations are ignored; only the
   token kind, keyword and associated value are compared.  */

static bool
profile_enforcement_args_equal (tree a, tree b)
{
  if (a == b)
    return true;
  if (a == NULL_TREE || b == NULL_TREE)
    return false;

  cp_token_cache *ca = DEFPARSE_TOKENS (a);
  cp_token_cache *cb = DEFPARSE_TOKENS (b);

  if (ca->last - ca->first != cb->last - cb->first)
    return false;

  for (cp_token *ta = ca->first, *tb = cb->first; ta < ca->last; ++ta, ++tb)
    {
      if (ta->type != tb->type
	  || ta->keyword != tb->keyword
	  || ta->tree_check_p != tb->tree_check_p)
	return false;

      tree va = ta->tree_check_p ? ta->u.tree_check_value->value : ta->u.value;
      tree vb = tb->tree_check_p ? tb->u.tree_check_value->value : tb->u.value;
      if (profile_literal_token_p (ta))
	{
	  /* A literal is the same token only if it is spelled the same.  */
	  char *sa = profile_token_source_spelling (ta);
	  char *sb = profile_token_source_spelling (tb);
	  bool same = (sa && sb) ? strcmp (sa, sb) == 0
				 : profile_token_values_equal (va, vb);
	  free (sa);
	  free (sb);
	  if (!same)
	    return false;
	}
      else if (!profile_token_values_equal (va, vb))
	return false;
    }

  return true;
}

/* Record ENFORCEMENT (a TREE_LIST whose TREE_PURPOSE is the profile-name and
   whose TREE_VALUE is the deferred argument clause), appearing at LOCATION, as
   seen in this translation unit.

   If a profile-enforcement with the same profile-name has already been
   recorded, its argument clause is compared against ENFORCEMENT's: if they
   consist of the same tokens the previously stored enforcement is returned,
   otherwise an error is issued and NULL is returned.  Otherwise, if a matching
   profile_spec is registered and its enforce hook rejects the designator,
   NULL is returned.  Otherwise ENFORCEMENT is stored and returned.  */

profile_enforcement *
add_profile_enforcement (tree enforcement, location_t location)
{
  tree name = TREE_PURPOSE (enforcement);
  profile_enforcement *existing = profile_enforcement_exists (name);
  if (existing != NULL)
    {
      /* Existing args may already be STRING_CST spellings; the new clause
	 may still be a live DEFERRED_PARSE.  */
      if (profile_designator_args_match (TREE_VALUE (existing->enforcement),
					 TREE_VALUE (enforcement)))
	return existing;

      error_at (location,
		"profile %qE already loaded with different arguments", name);
      inform (existing->location, "previously loaded here");
      return NULL;
    }

  const profile_spec *spec = lookup_profile_spec (name);
  if (spec && spec->enforce
      && !spec->enforce (name, TREE_VALUE (enforcement), location))
    return NULL;

  /* Materialize argument spellings while token caches are still live
     (see materialize_profile_enforcement_args).  */
  materialize_profile_enforcement_args (enforcement);

  profile_enforcement *pe = ggc_alloc<profile_enforcement> ();
  pe->enforcement = enforcement;
  pe->location = location;
  // somehow figure out what the relevant profile hooks are and add it to the profile enforcement

  vec_safe_push (profile_enforcements, pe);
  return pe;
}

/* If a profile-enforcement with the given NAME has been recorded in this
   translation unit, return it; otherwise return NULL.  */

profile_enforcement *
profile_enforcement_exists (tree name)
{
  unsigned int i;
  for (i = 0; i < vec_safe_length (profile_enforcements); i++)
    {
      profile_enforcement *pe = (*profile_enforcements)[i];
      tree existing_name = TREE_PURPOSE (pe->enforcement);
      /* Profile-names are get_identifier results, so pointer equality is
	 enough; do not use cp_tree_equal on them.  */
      if (existing_name == name)
	return pe;
    }
  return NULL;
}

/* See profiles.h.  */

tree
profile_enforcements_as_list (void)
{
  tree list = NULL_TREE;
  unsigned int i;
  for (i = 0; i < vec_safe_length (profile_enforcements); i++)
    {
      profile_enforcement *pe = (*profile_enforcements)[i];
      tree e = pe->enforcement;
      list = tree_cons (TREE_PURPOSE (e), TREE_VALUE (e), list);
    }
  return nreverse (list);
}

/* Attribute-name spelling for KIND.  Only enforce and require carry
   designator lists today.  */

static const char *
profiles_attr_kind_name (enum profiles_attr_kind kind)
{
  switch (kind)
    {
    case PAK_ENFORCE:
      return "enforce";
    case PAK_REQUIRE:
      return "require";
    case PAK_SUPPRESS:
      return "suppress";
    default:
      gcc_unreachable ();
    }
}

/* From *ATTRS, extract designators of profiles::<kind> into a fresh
   TREE_LIST (TREE_PURPOSE = profile-name, TREE_VALUE = argument clause).
   If REMOVE_P, also unlink matching attributes from *ATTRS.  */

static tree
extract_profile_designators (tree *attrs, enum profiles_attr_kind kind,
			     bool remove_p)
{
  const char *attr_name = profiles_attr_kind_name (kind);
  tree result = NULL_TREE;

  for (tree *pa = attrs; *pa; )
    {
      tree a = *pa;
      if (a == error_mark_node || a == void_list_node)
	{
	  pa = &TREE_CHAIN (*pa);
	  continue;
	}
      if (!is_attribute_namespace_p ("profiles", a)
	  || !is_attribute_p (attr_name, get_attribute_name (a)))
	{
	  pa = &TREE_CHAIN (*pa);
	  continue;
	}

      for (tree e = TREE_VALUE (a); e; e = TREE_CHAIN (e))
	{
	  if (e == error_mark_node)
	    continue;
	  gcc_checking_assert (TREE_CODE (e) == TREE_LIST);
	  /* A repeated enforcement has no effect ([decl.profile.enforce]/3):
	     keep each profile once, as the TU record does.  Requirements are
	     each checked, so keep them all.  */
	  if (kind == PAK_ENFORCE && purpose_member (TREE_PURPOSE (e), result))
	    continue;
	  result = chainon (result,
			    build_tree_list (TREE_PURPOSE (e), TREE_VALUE (e)));
	}

      if (remove_p)
	*pa = TREE_CHAIN (*pa);
      else
	pa = &TREE_CHAIN (*pa);
    }

  return result;
}

tree
extract_profile_enforcements (tree *attrs, bool remove_p)
{
  return extract_profile_designators (attrs, PAK_ENFORCE, remove_p);
}

tree
extract_profile_requirements (tree *attrs, bool remove_p)
{
  return extract_profile_designators (attrs, PAK_REQUIRE, remove_p);
}

/* Print TOKEN to STREAM using source-like spellings where possible.  */

static void
dump_profile_token (FILE *stream, const cp_token *token)
{
  tree val = token->tree_check_p ? token->u.tree_check_value->value
				 : token->u.value;

  switch (token->type)
    {
    case CPP_KEYWORD:
      if (!val || !identifier_p (val))
	{
	  if (token->keyword != RID_MAX && ridpointers[token->keyword])
	    fputs (IDENTIFIER_POINTER (ridpointers[token->keyword]), stream);
	  else
	    fputs (cpp_type2name (token->type, token->flags), stream);
	  break;
	}
      /* FALLTHRU */
    case CPP_NAME:
      fputs (IDENTIFIER_POINTER (val), stream);
      break;

    case CPP_NUMBER:
    case CPP_STRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_WSTRING:
    case CPP_UTF8STRING:
    case CPP_CHAR:
    case CPP_WCHAR:
    case CPP_CHAR16:
    case CPP_CHAR32:
    case CPP_UTF8CHAR:
      /* A literal is spelled as written, so that its identity is its
	 token, not its value; otherwise fall back to the value.  The
	 strings here are unjoined, so their value already holds the
	 spelling, quotes and prefix included.  */
      if (char *spelling = profile_token_source_spelling (token))
	{
	  fputs (spelling, stream);
	  free (spelling);
	}
      else if (TREE_CODE (val) == STRING_CST)
	fputs (TREE_STRING_POINTER (val), stream);
      else
	print_generic_expr (stream, val);
      break;

    default:
      {
	/* Prefer "(" / ":" spellings over "CPP_OPEN_PAREN".  */
	static const char *const spellings[] = {
#define OP(e, s) s,
#define TK(e, s) NULL,
	  TTYPE_TABLE
#undef OP
#undef TK
	};
	if (token->type < ARRAY_SIZE (spellings) && spellings[token->type])
	  fputs (spellings[token->type], stream);
	else
	  fputs (cpp_type2name (token->type, token->flags), stream);
      }
      break;
    }
}

/* True if TOKEN is spelled as a word or literal that usually wants a
   separating space after ':' or ',' or another word.  */

static bool
profile_token_word_p (const cp_token *token)
{
  switch (token->type)
    {
    case CPP_NAME:
    case CPP_KEYWORD:
    case CPP_NUMBER:
    case CPP_STRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_WSTRING:
    case CPP_UTF8STRING:
    case CPP_CHAR:
    case CPP_WCHAR:
    case CPP_CHAR16:
    case CPP_CHAR32:
    case CPP_UTF8CHAR:
      return true;
    default:
      return false;
    }
}

/* Dump the argument clause of a profile designator to STREAM.  ARGS is a
   DEFERRED_PARSE of the balanced token sequence, a STRING_CST spelling from
   a CMI, or NULL_TREE.  */

static void
dump_profile_args (FILE *stream, tree args)
{
  if (!args)
    return;

  if (TREE_CODE (args) == STRING_CST)
    {
      fputs (TREE_STRING_POINTER (args), stream);
      return;
    }

  gcc_checking_assert (TREE_CODE (args) == DEFERRED_PARSE);
  cp_token_cache *cache = DEFPARSE_TOKENS (args);
  bool space_before_word = false;
  for (cp_token *t = cache->first; t < cache->last; ++t)
    {
      if (space_before_word && profile_token_word_p (t))
	fputc (' ', stream);
      dump_profile_token (stream, t);
      space_before_word
	= (profile_token_word_p (t)
	   || t->type == CPP_COLON
	   || t->type == CPP_COMMA);
    }
}

/* Return a malloc'd spelling of ARGS.  Caller must free.  ARGS must still
   refer to live token data when it is a DEFERRED_PARSE.  */

char *
profile_enforcement_args_spelling (tree args)
{
  if (!args)
    return xstrdup ("");
  if (TREE_CODE (args) == STRING_CST)
    return xstrdup (TREE_STRING_POINTER (args));

  char *buf = NULL;
  size_t size = 0;
  FILE *stream = open_memstream (&buf, &size);
  gcc_assert (stream);
  dump_profile_args (stream, args);
  fclose (stream);
  return buf;
}

/* Replace any DEFERRED_PARSE argument clauses in ENFORCEMENTS with
   STRING_CST spellings.  Parser token caches are not guaranteed to live
   until CMI write-out, so recorded/exported profiles must not retain them.
   Called when recording a TU enforcement and again when installing a
   module's exported set (no-op if already STRING_CST / NULL).  */

tree
materialize_profile_enforcement_args (tree enforcements)
{
  for (tree e = enforcements; e; e = TREE_CHAIN (e))
    {
      tree args = TREE_VALUE (e);
      if (!args || TREE_CODE (args) != DEFERRED_PARSE)
	continue;

      char *sp = profile_enforcement_args_spelling (args);
      TREE_VALUE (e) = *sp ? build_string (strlen (sp), sp) : NULL_TREE;
      free (sp);
    }
  return enforcements;
}

/* True if A and B name the same argument clause for require/enforce matching.
   Token-sequence equality is used when both are live DEFERRED_PARSE nodes;
   otherwise spellings are compared (e.g. against a CMI-loaded STRING_CST).  */

bool
profile_designator_args_match (tree a, tree b)
{
  if (a == b)
    return true;
  if (a && b
      && TREE_CODE (a) == DEFERRED_PARSE
      && TREE_CODE (b) == DEFERRED_PARSE)
    return profile_enforcement_args_equal (a, b);

  char *sa = profile_enforcement_args_spelling (a);
  char *sb = profile_enforcement_args_spelling (b);
  bool eq = !strcmp (sa, sb);
  free (sa);
  free (sb);
  return eq;
}

/* Ensure every designator in REQUIRED is matched by an exported enforce.  */

void
check_profile_requirements (location_t loc, const char *module_name,
			    tree required, tree exported)
{
  for (tree r = required; r; r = TREE_CHAIN (r))
    {
      tree rname = TREE_PURPOSE (r);
      tree rargs = TREE_VALUE (r);
      bool name_seen = false;
      bool matched = false;

      for (tree e = exported; e; e = TREE_CHAIN (e))
	{
	  if (TREE_PURPOSE (e) != rname)
	    continue;
	  name_seen = true;
	  if (profile_designator_args_match (rargs, TREE_VALUE (e)))
	    {
	      matched = true;
	      break;
	    }
	}

      if (matched)
	continue;

      if (name_seen)
	error_at (loc,
		  "module %qs does not enforce profile %qE with the "
		  "required arguments",
		  module_name, rname);
      else
	error_at (loc, "module %qs does not enforce required profile %qE",
		  module_name, rname);
    }
}

/* Write a one-line description of ENFORCEMENT to STREAM. This
   is used for module purposes.  If enforcement arguments were
   profiles responsibility, we could have the profile decide how
   it wants to be represented in the module, and whether a
   requirement matches the enforcement. */

void
dump_profile_enforcement (FILE *stream, tree enforcement)
{
  gcc_checking_assert (TREE_CODE (enforcement) == TREE_LIST);

  fputs ("enforce ", stream);
  fputs (IDENTIFIER_POINTER (TREE_PURPOSE (enforcement)), stream);
  dump_profile_args (stream, TREE_VALUE (enforcement));
}

#include "gt-cp-profiles.h"
