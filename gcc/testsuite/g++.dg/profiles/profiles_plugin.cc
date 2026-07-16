/* Profiles attribute plugin.
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
along with GCC.  If not, see <http://www.gnu.org/licenses/>.  */

/* Register the profiles:: attribute namespace via PLUGIN_ATTRIBUTES.
   Argument clauses use max_length == -3 so the frontend caches them as a
   DEFERRED_PARSE (cp_token_cache).  This plugin walks that token range to
   check the profiles argument grammar; it does not drive the C++ parser.  */

#include "gcc-plugin.h"
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "stringpool.h"
#include "tm.h"
#include "cp/cp-tree.h"
#include "cp/parser.h"
#include "attribs.h"
#include "plugin-version.h"

int plugin_is_GPL_compatible;


struct profiles_tokens
{
  cp_token *cur;
  /* Exclusive end of the argument body; points at the closing ')'.  */
  cp_token *end;
  /* The ')' that closes the attribute-argument-clause; use for diagnostics
     when CUR has gone past END.  */
  cp_token *close_paren;
};

/* Check if we have reached the end of tokens.  */
static bool
profiles_tokens_done (const profiles_tokens &toks)
{
  return toks.cur >= toks.close_paren;
}

/* Returns true if the current token has the appropriate type.
 Otherwise, returns false.  */

bool
profiles_next_token (const profiles_tokens &toks, enum cpp_ttype type)
{
  if (profiles_tokens_done(toks))
    return false;

  return (toks.cur->type == type);

}

/* Return true if TOKEN is an operator-or-punctuator token.  */
static bool
cp_token_is_operator_or_punctuator_p (const cp_token *token)
{
  return token->type <= CPP_LAST_PUNCTUATOR;
}

/* Skip one balanced-token starting at START within [START, END).
   Returns a pointer past the token, or NULL on failure.  Mirrors
   cp_parser_skip_balanced_tokens for a bounded cache range.  */
static cp_token *
profiles_skip_balanced_token (cp_token *start, cp_token *end)
{
  if (start >= end)
    return NULL;

  cp_token *p = start;
  int nparens = 0, nbraces = 0, nsquares = 0, nsplices = 0;
  do
    {
      if (p >= end)
	return NULL;
      switch (p->type)
	{
	case CPP_EOF:
	case CPP_PRAGMA_EOL:
	  return NULL;
	case CPP_OPEN_PAREN:
	  ++nparens;
	  break;
	case CPP_OPEN_BRACE:
	  ++nbraces;
	  break;
	case CPP_OPEN_SQUARE:
	  ++nsquares;
	  break;
	case CPP_OPEN_SPLICE:
	  ++nsplices;
	  break;
	case CPP_CLOSE_PAREN:
	  --nparens;
	  break;
	case CPP_CLOSE_BRACE:
	  --nbraces;
	  break;
	case CPP_CLOSE_SQUARE:
	  --nsquares;
	  break;
	case CPP_CLOSE_SPLICE:
	  --nsplices;
	  break;
	default:
	  break;
	}
      p++;
      if (nparens < 0 || nbraces < 0 || nsquares < 0 || nsplices < 0)
	return NULL;
    }
  while (nparens || nbraces || nsquares || nsplices);
  return p;
}

/* Parse a profile-argument from TOKS.

   profile-argument:
     non-operator-non-punctuator-token
     identifier : non-comma-balanced-token  */

static tree
profiles_parse_argument (profiles_tokens &toks)
{
  if (profiles_tokens_done (toks)
      || profiles_next_token (toks, CPP_CLOSE_PAREN))
    {
      error_at (toks.close_paren->location, "expected profile-argument");
      return error_mark_node;
    }

  /* identifier : non-comma-balanced-token  */
  if (profiles_next_token (toks, CPP_NAME)
      && toks.cur + 1 < toks.close_paren
      && toks.cur[1].type == CPP_COLON)
    {
      tree name = toks.cur->u.value;
      toks.cur += 2;

      if (profiles_tokens_done (toks))
	{
	  error_at (toks.close_paren->location,
		    "expected a non-comma-balanced-token");
	  return error_mark_node;
	}
      if (profiles_next_token (toks, CPP_COMMA))
	{
	  error_at (toks.cur->location,
		    "expected a non-comma-balanced-token");
	  return error_mark_node;
	}

      cp_token *after
	= profiles_skip_balanced_token (toks.cur, toks.close_paren);
      if (after == NULL)
	{
	  error_at (toks.cur->location,
		    "expected a non-comma-balanced-token");
	  return error_mark_node;
	}
      toks.cur = after;
      return name;
    }

  /* non-operator-non-punctuator-token  */
  if (cp_token_is_operator_or_punctuator_p (toks.cur))
    {
      error_at (toks.cur->location,
		"expected a non-operator-non-punctuator-token");
      return error_mark_node;
    }

  tree op_or_punct = toks.cur->u.value;
  toks.cur++;
  return op_or_punct;
}

/* Parse a profile-argument-list from TOKS.

   profile-argument-list:
     profile-argument
     profile-argument-list , profile-argument

   On success, returns the tree value of the first argument's leading
   token; arguments are only skipped for now.  */

static tree
profiles_parse_argument_list (profiles_tokens &toks)
{

  tree args = NULL_TREE;

  while (true)
    {
      if (profiles_tokens_done (toks))
	{
	  error_at (toks.close_paren->location, "expected profile-argument");
	  return error_mark_node;
	}

      args = profiles_parse_argument (toks);
      if (args == error_mark_node)
	return error_mark_node;

      if (profiles_next_token (toks, CPP_COMMA))
	toks.cur++;
      else
	break;
    }

  return args;
}

/* Parse a profile-name from TOKS.

   profile-name:
     identifier
     profile-name :: identifier  */

static tree
profiles_parse_name (profiles_tokens &toks)
{

  tree profile_name = NULL_TREE;
  bool saw_name = false;

  while (true)
    {
      if (profiles_tokens_done (toks) || toks.cur->type != CPP_NAME)
	{
	  location_t loc = profiles_tokens_done (toks)
			   ? toks.close_paren->location
			   : toks.cur->location;
	  if (!saw_name)
	    error_at (loc, "expected profile-name");
	  else
	    error_at (loc,
		      "expected identifier after %<::%> in profile-name");
	  return error_mark_node;
	}

      profile_name = toks.cur->u.value;
      saw_name = true;
      toks.cur++;
      if (!profiles_next_token (toks, CPP_SCOPE))
	break;
      toks.cur++;
    }
  return profile_name;
}

/* Parse a profile-designator from TOKS.

   profile-designator:
     profile-name
     profile-name ( profile-argument-list )

   On success, returns the profile-name; optional arguments are only
   skipped for now.  */

static tree
profiles_parse_designator (profiles_tokens &toks)
{
  tree profile_name = profiles_parse_name (toks);
  if (profile_name == error_mark_node)
    return error_mark_node;

  if (!profiles_next_token (toks, CPP_OPEN_PAREN))
    return profile_name;

  toks.cur++;

  if (profiles_parse_argument_list (toks) == error_mark_node)
    return error_mark_node;

  if (!profiles_next_token (toks, CPP_CLOSE_PAREN))
    {
      location_t loc = profiles_tokens_done (toks)
		       ? toks.close_paren->location
		       : toks.cur->location;
      error_at (loc, "expected %<)%>");
      return error_mark_node;
    }
  toks.cur++;
  return profile_name;
}

/* Parse a profile-designator-list from TOKS.

   profile-designator-list:
     profile-designator
     profile-designator-list , profile-designator

   On success, returns the last designator's profile-name; designators
   are only skipped for now.  */

static tree
profiles_parse_designator_seq (profiles_tokens &toks)
{
  tree designator = NULL_TREE;

  while (true)
    {
      if (profiles_tokens_done (toks))
	{
	  error_at (toks.close_paren->location,
		    "expected profile-designator");
	  return error_mark_node;
	}

      designator = profiles_parse_designator (toks);
      if (designator == error_mark_node)
	return error_mark_node;

      if (profiles_next_token (toks, CPP_COMMA))
	toks.cur++;
      else
	break;
    }

  return designator;
}

/* Extract the DEFERRED_PARSE token body from ARGS into *OUT.
   Returns false and diagnoses on failure.  */

static bool
profiles_extract_tokens (tree name, tree args, profiles_tokens *out)
{
  if (args == NULL_TREE || TREE_CODE (args) != TREE_LIST)
    {
      error ("missing arguments for %<profiles::%E%>", name);
      return false;
    }

  tree deferred = TREE_VALUE (args);
  if (deferred == NULL_TREE || TREE_CODE (deferred) != DEFERRED_PARSE)
    {
      error ("invalid arguments for %<profiles::%E%>", name);
      return false;
    }

  cp_token_cache *cache = DEFPARSE_TOKENS (deferred);
  /* cache->first points to the first token after the attribute name,
     cache->last points one past the attribute.  */
  cp_token *first = cache->first;
  cp_token *last = cache->last-1;
  if (first == NULL || last == NULL || first >= last
      || first->type != CPP_OPEN_PAREN
      || last->type != CPP_CLOSE_PAREN)
    {
      error_at (first ? first->location : input_location,
		"expected %<(%> in %<profiles::%E%> arguments", name);
      return false;
    }

  out->cur = first + 1;
  out->end = last - 1;
  out->close_paren = last;
  return true;
}

/* True if TOKS was fully consumed; otherwise diagnose leftover tokens.  */

static bool
profiles_finish_args (tree name, profiles_tokens &toks)
{
  if (profiles_tokens_done (toks))
    return true;

  error_at (toks.cur->location,
	    "unexpected token in %<profiles::%E%> arguments", name);
  return false;
}

/*
  Handle profiles::enforce
  [[profiles::enforce(profile-designator-list)]]
*/

static tree
profiles_plugin_enforce_attribute (tree *node, tree name, tree args,
				   int flags, bool *no_add_attrs)
{
  profiles_tokens toks;

  if (!profiles_extract_tokens (name, args, &toks))
    return error_mark_node;

  tree designators = profiles_parse_designator_seq (toks);

  if (designators == error_mark_node
      || !profiles_finish_args (name, toks))
    return error_mark_node;
  return NULL_TREE;
}

/*
  Handle profiles::require
  [[profiles::require(profile-designator-list)]]
*/

static tree
profiles_plugin_require_attribute (tree *node, tree name, tree args,
				   int flags, bool *no_add_attrs)
{
  profiles_tokens toks;

  if (!profiles_extract_tokens (name, args, &toks))
    return error_mark_node;

  tree designators = profiles_parse_designator_seq (toks);

  if (designators == error_mark_node
      || !profiles_finish_args (name, toks))
    return error_mark_node;
  return NULL_TREE;
}

/*
  Handle profiles::suppress attribute.

  [[profiles::suppress(profile-name, profile-argument-list)]]

 */

static tree
profiles_plugin_suppress_attribute (tree *node, tree name, tree args,
				    int flags, bool *no_add_attrs)
{
  profiles_tokens toks;

  if (!profiles_extract_tokens(name, args, &toks))
    return error_mark_node;

  tree profile_name = profiles_parse_name (toks);

  if (profile_name == error_mark_node)
    return error_mark_node;

  if (profiles_tokens_done(toks))
    return NULL_TREE;

  if (!profiles_next_token(toks, CPP_COMMA))
    {
      error_at (toks.cur->location, "expected %<,%>");
      return error_mark_node;
    }

  toks.cur++;

  tree profile_args = profiles_parse_argument_list (toks);

  if (profile_args == error_mark_node || !profiles_finish_args(name, toks))
    return error_mark_node;
  return NULL_TREE;
}


/* max_length -3: balanced-token-seq attribute-argument-clause; min_length
   is unused for arity checks in that mode.

   TODO : add exempt when we know the grammar.  */
static const attribute_spec profiles_attributes[] =
{
  { "enforce",  0, -3, false, false, false, false,
    profiles_plugin_enforce_attribute, NULL },
  { "require",  0, -3, false, false, false, false,
    profiles_plugin_require_attribute, NULL },
  { "suppress", 0, -3, false, false, false, false,
    profiles_plugin_suppress_attribute, NULL },

};

static const scoped_attribute_specs profiles_attribute_table =
{
  "profiles", { profiles_attributes }
};

static void
register_profiles_attributes (void * /*event_data*/, void * /*data*/)
{
  register_scoped_attributes (profiles_attribute_table);
}

int
plugin_init (struct plugin_name_args *plugin_info,
	     struct plugin_gcc_version *version)
{
  if (!plugin_default_version_check (version, &gcc_version))
    return 1;

  register_callback (plugin_info->base_name, PLUGIN_ATTRIBUTES,
		     register_profiles_attributes, NULL);
  return 0;
}
