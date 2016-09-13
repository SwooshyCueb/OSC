#include "common.h"
#include "storagesystem.h"
#include "user.h"

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <system_error>

using namespace std;

namespace globals {
    StorageSystem local_storage;
    User logged_in;
}

static gchar *username_cs = NULL;
static gchar *password_cs = NULL;
static gboolean initdb = FALSE;

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static GOptionEntry entries[] =
{
    { "user", 'u', 0, G_OPTION_ARG_STRING, &username_cs,
      "Username for logging in (required if not --init)", "username" },
    { "password", 'p', 0, G_OPTION_ARG_STRING, &password_cs,
      "Password for authentication (required if not --init)", "password" },
    { "init", 0, 0, G_OPTION_ARG_NONE, &initdb,
      "Populate the database with sample data", NULL },
    { NULL }
};
#pragma GCC diagnostic warning "-Wmissing-field-initializers"

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GOptionContext *context;
    string cmd;

    // argument parsing
    context = g_option_context_new("- Online Shopping Center command line "
                                   "interface");
    g_option_context_set_description(context,
                                     "Passwords are beyond the scope of this "
                                     "assignment.\n"
                                     "You can put pretty much anything for the "
                                     "password and we'll take it.\n"
                                     "\n"
                                     "Database files are expected to be in the "
                                     "current directory.\n"
                                     "If they are not, they will be created.");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_print("option parsing failed: %s\n", error->message);
        exit(1);
    }

    if (initdb) {
        g_print("WARNING: Populating the database will destroy any database "
                "already present in the current directory.\n\n"
                "Are you SURE you want to do this [y/N]? ");
        getline(cin, cmd);
        if (cmd.front() == 'y' || cmd.front() == 'Y') {
            g_print("Populating database with sample data\n"
                    "This can take some time, depending on your system.\n");
            globals::local_storage.initDB();
        } else {
            g_print("Exiting.\n");
        }
        exit(0);
    }

    if (username_cs == NULL) {
        // user did not provide  username
        g_printerr("Please provide username.\n"
                   "See `%s --help` for more information.\n", argv[0]);
        exit(1);
    }

    if (password_cs == NULL) {
        // user did not provide a password
        g_printerr("Please provide password.\n"
                   "See `%s --help` for more information.\n", argv[0]);
        exit(1);
    }

    try {
        globals::logged_in = globals::local_storage.getUser(username_cs);
    } catch (const system_error &e) {
        g_printerr("Authentication failed...\n");
        exit(1);
    }

    return 0;
}
