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
    vector<string> cmd_parts;

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

    g_print("Authenticating...\n");
    try {
        globals::logged_in = globals::local_storage.getUser(username_cs);
    } catch (const system_error &e) {
        g_printerr("Authentication failed...\n");
        exit(1);
    }

    g_print("Logged in as %s.\n", globals::logged_in.user_name.c_str());

    while (true) {
        g_print("\n\033[1;4mCurrent inventory:\033[0m\n");
        vector<SKU> prodlist = globals::local_storage.getProducts();
        for (auto upc : prodlist) {
            Product prod = globals::local_storage.getProduct(upc);
            g_print("%lu:\t\033[1m%s\033[0m\n  ", upc, prod.getName().c_str());
            g_print("Price: $%04.2f\t", prod.getPrice());
            g_print("On hand: %u\t", prod.getQuantity());
            g_print("Category: %s\n", prod.getCategory().c_str());
        }
        mainhelp:
        g_print("\n\033[1;4mAvailable commands:\033[0m\n");
        g_print("  addtocart <UPC> [qty]     Add product to cart\n");
        g_print("  cart                      Manage shopping cart\n");
        g_print("  profile                   Manage personal information\n");
        g_print("  exit                      Log out and close OSC\n");
        g_print("\n");
        mainshell:
        g_print("\033[1m>>>\033[0m ");
        getline(cin, cmd);
        cmd_parts = split(cmd, ' ');
        if (cmd_parts.empty()) {
            goto mainshell;
        } else if (cmd_parts[0] == "exit") {
            g_print("Exiting.\n");
            exit(0);
        } else if (cmd_parts[0] == "addtocart") {
            if (cmd_parts.size() < 2) {
                g_printerr("Invalid use of addtocart.\n");
                goto mainhelp;
            }
            Product prod;
            unsigned int qty;
            try {
                prod = globals::local_storage.getProduct(stoul(cmd_parts[1]));
            } catch (const system_error &e) {
                g_printerr("'%s' is not a valid UPC.\n", cmd_parts[1].c_str());
                continue;
            } catch (const invalid_argument &e) {
                g_printerr("'%s' is not a valid UPC.\n", cmd_parts[1].c_str());
                continue;
            }
            if (cmd_parts.size() == 2) {
                qty = 1;
            } else {
                try {
                    qty = (unsigned int)stoul(cmd_parts[2]);
                } catch (const invalid_argument &e) {
                    g_printerr("'%s' is not a valid quantity.\n", cmd_parts[2].c_str());
                    goto mainhelp;
                }
            }
            if (qty > prod.getQuantity()) {
                g_printerr("WARNING: There are only %u of this item in stock.\n", prod.getQuantity());
                g_printerr("         The specified quantity %u is greater than our on-hand count.\n", qty);
                g_printerr("         You will not be able to checkout with an invalid quantity.\n");
                g_printerr("         WE CANNOT PLACE ITEMS ON BACKORDER.\n");
            }
            if (qty == 1) {
                globals::logged_in.shopping_cart.addProduct(prod);
            } else {
                globals::logged_in.shopping_cart.addProduct(prod, qty);
            }
            g_print("%u of %s added to cart.\n", qty, prod.getName().c_str());
            goto mainshell;
        } else if (cmd_parts[0] == "cart") {
            while (true) {
                g_print("\n\033[1;4mShopping cart:\033[0m\n");
                bool invalid_qty = false;
                float total = 0;
                for (auto prod_spp : globals::logged_in.shopping_cart.cart) {
                    total += prod_spp.second.first.getPrice() * float(prod_spp.second.second);
                    g_print("%lu:\t\033[1m%s\033[0m\n  ", prod_spp.first, prod_spp.second.first.getName().c_str());
                    g_print("Qty: %u\t", prod_spp.second.second);
                    g_print("Price: $%04.2f ($%04.2f/ea)\n", prod_spp.second.first.getPrice() * float(prod_spp.second.second), prod_spp.second.first.getPrice());
                    if (prod_spp.second.second > prod_spp.second.first.getQuantity()) {
                        invalid_qty = true;
                    }
                }
                g_print("\033[1mTotal: $%04.2f\033[0m\n", total);
                if (invalid_qty) {
                    g_printerr("\n");
                    g_printerr("WARNING: One or more entries in your cart have an invalid qauntity.\n");
                    g_printerr("         You will not be able to checkout with an invalid quantity.\n");
                    g_printerr("         WE CANNOT PLACE ITEMS ON BACKORDER.\n");
                }
                carthelp:
                g_print("\n\033[1;4mAvailable commands:\033[0m\n");
                g_print("  changeqty <UPC> <qty>     Change quantity of item\n");
                g_print("  remove <UPC>              Remove item from cart\n");
                g_print("  clear                     Remove all items from cart\n");
                g_print("  back                      Back to main menu\n");
                g_print("  exit                      Log out and close OSC\n");
                g_print("\n");
                cartshell:
                g_print("\033[1m>>>\033[0m ");
                getline(cin, cmd);
                cmd_parts = split(cmd, ' ');
                if (cmd_parts.empty()) {
                    goto cartshell;
                } else if (cmd_parts[0] == "exit") {
                    g_print("Exiting.\n");
                    exit(0);
                } else if (cmd_parts[0] == "back") {
                    break;
                } else if (cmd_parts[0] == "remove") {
                    // code goes here
                } else if (cmd_parts[0] == "changeqty") {
                    if (cmd_parts.size() < 3) {
                        g_printerr("Invalid use of changeqty.\n");
                        goto carthelp;
                    }
                    unsigned int new_qty;
                    SKU sel_upc;
                    try {
                        sel_upc = stoul(cmd_parts[1]);
                    } catch (const invalid_argument &e) {
                        g_printerr("'%s' is not a valid UPC.\n", cmd_parts[1].c_str());
                        continue;
                    }
                    try {
                        new_qty = (unsigned int)stoul(cmd_parts[2]);
                    } catch (const invalid_argument &e) {
                        g_printerr("'%s' is not a valid quantity.\n", cmd_parts[2].c_str());
                        goto carthelp;
                    }
                    if (!globals::logged_in.shopping_cart.checkCart(sel_upc)) {
                        g_printerr("No item with UPC %lu exists in the cart.\v", sel_upc);
                        continue;
                    }
                    if (new_qty > globals::logged_in.shopping_cart.cart[sel_upc].first.getQuantity()) {
                        g_printerr("WARNING: There are only %u of this item in stock.\n", globals::logged_in.shopping_cart.cart[sel_upc].first.getQuantity());
                        g_printerr("         The specified quantity %u is greater than our on-hand count.\n", new_qty);
                        g_printerr("         You will not be able to checkout with an invalid quantity.\n");
                        g_printerr("         WE CANNOT PLACE ITEMS ON BACKORDER.\n");
                        g_printerr("\n");
                    }
                    globals::logged_in.shopping_cart.changeQuantity(sel_upc, new_qty);
                    g_print("Quantity updated.\n");
                    continue;
                } else if (cmd_parts[0] == "clear") {
                    globals::logged_in.shopping_cart.emptyCart();
                    g_print("Your cart has been emptied.\n");
                    g_print("Returning to main menu...\n");
                    break;
                } else {
                    g_printerr("'%s' is an unrecognized command.\n", cmd_parts[0].c_str());
                    goto carthelp;
                }
            }
        } else if (cmd_parts[0] == "profile") {
            // code goes here
        } else {
            g_printerr("'%s' is an unrecognized command.\n", cmd_parts[0].c_str());
            goto mainhelp;
        }
    }

    return 0;
}
