/* Demonstrates how to register a concrete C++ profile-name.  */

#include "gcc-plugin.h"
#include <stdlib.h>
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "stringpool.h"
#include "toplev.h"
#include "intl.h"
#include "plugin.h"
#include "diagnostic.h"
#include "cp/profiles.h"

int plugin_is_GPL_compatible;

/* Called when profiles::enforce(test::plugin) is seen.  */

static bool
handle_test_plugin (tree name, tree args, location_t loc)
{
  warning_at (loc, 0, G_("profile %qE enforced by plugin"), name);
  (void) args;
  return true;
}

static const profile_spec test_plugin_profile =
  { "test::plugin", handle_test_plugin };

static void
register_profiles (void * /* event_data */, void * /* data */)
{
  warning (0, G_("Callback to register profiles"));
  register_profile (&test_plugin_profile);
}

int
plugin_init (struct plugin_name_args *plugin_info,
	     struct plugin_gcc_version * /* version */)
{
  const char *plugin_name = plugin_info->base_name;
  register_callback (plugin_name, PLUGIN_PROFILES, register_profiles, NULL);
  return 0;
}
