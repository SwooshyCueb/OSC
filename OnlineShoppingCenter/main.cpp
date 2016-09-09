#include <glib.h>
/* Relevant documentation:
 * https://developer.gnome.org/glib/stable/glib-Shell-related-Utilities.html
 * https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html
 */

#include <iostream>
#include <cstdlib>

using namespace std;

static gchar *username = NULL;
static gchar *password = NULL;

static GOptionEntry entries[] =
{
  { "user", 'u', 0, G_OPTION_ARG_STRING, &username, "Username for logging in (required)", "username" },
  { "password", 'p', 0, G_OPTION_ARG_STRING, &password, "Password for authentication (required)", "password" },
  { NULL }
};

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GOptionContext *context;

    // argument parsing
    context = g_option_context_new("- Online Shopping Center command line interface");
    g_option_context_set_description(context, "Passwords are beyond the scope of this assignment.\n"
                                              "You can put pretty much anything for the password and we'll take it.\n");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_print("option parsing failed: %s\n", error->message);
        exit(1);
    }

    if (username == NULL) {
        // user did not provide  username
        g_printerr("Please provide username. See `%s --help` for more information.\n", argv[0]);
        exit(1);
    }

    if (password == NULL) {
        // user did not provide a password
        g_printerr("Please provide password. See `%s --help` for more information.\n", argv[0]);
        exit(1);
    }

    return 0;
}
