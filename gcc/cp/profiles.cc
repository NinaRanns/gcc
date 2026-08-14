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
#include "attribs.h"
#include "cpplib.h"
#include "parser.h"
#include "tree-pretty-print.h"
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
      if (!profile_token_values_equal (va, vb))
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
      if (profile_enforcement_args_equal (TREE_VALUE (existing->enforcement),
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

  profile_enforcement *pe = ggc_alloc<profile_enforcement> ();
  pe->enforcement = enforcement;
  pe->location = location;
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

/* From ATTRS, extract designators of profiles::<kind> into a fresh
   TREE_LIST (TREE_PURPOSE = profile-name, TREE_VALUE = argument clause).  */

static tree
extract_profile_designators (tree attrs, enum profiles_attr_kind kind)
{
  const char *attr_name = profiles_attr_kind_name (kind);
  tree result = NULL_TREE;

  for (tree a = attrs; a; a = TREE_CHAIN (a))
    {
      if (a == error_mark_node || a == void_list_node)
	continue;
      if (!is_attribute_namespace_p ("profiles", a)
	  || !is_attribute_p (attr_name, get_attribute_name (a)))
	continue;

      /* Build a fresh TREE_LIST rather than chainon'ing the attribute's own
	 list, which would mutate TREE_CHAIN on those nodes.  */
      for (tree e = TREE_VALUE (a); e; e = TREE_CHAIN (e))
	{
	  if (e == error_mark_node)
	    continue;
	  gcc_checking_assert (TREE_CODE (e) == TREE_LIST);
	  result = chainon (result,
			    build_tree_list (TREE_PURPOSE (e), TREE_VALUE (e)));
	}
    }

  return result;
}

tree
extract_profile_enforcements (tree attrs)
{
  return extract_profile_designators (attrs, PAK_ENFORCE);
}

tree
extract_profile_requirements (tree attrs)
{
  return extract_profile_designators (attrs, PAK_REQUIRE);
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
      print_generic_expr (stream, val);
      break;

    case CPP_STRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_WSTRING:
    case CPP_UTF8STRING:
      fprintf (stream, "\"%s\"", TREE_STRING_POINTER (val));
      break;

    case CPP_CHAR:
    case CPP_WCHAR:
    case CPP_CHAR16:
    case CPP_CHAR32:
    case CPP_UTF8CHAR:
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
   until CMI write-out, so exported profiles must not retain them.  */

tree
stabilize_profile_enforcements (tree enforcements)
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

/* Write a one-line description of ENFORCEMENT to STREAM.  */

void
dump_profile_enforcement (FILE *stream, tree enforcement)
{
  gcc_checking_assert (TREE_CODE (enforcement) == TREE_LIST);

  fputs ("enforce ", stream);
  fputs (IDENTIFIER_POINTER (TREE_PURPOSE (enforcement)), stream);
  dump_profile_args (stream, TREE_VALUE (enforcement));
}

#include "gt-cp-profiles.h"
