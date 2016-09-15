#include "common.h"
#include "storagesystem.h"
#include "user.h"

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <system_error>
#include <cmath>

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

/* The main function contains all the code for the command line interface. I
 * feel it might have been a lot neater, and much better practice to write a new
 * class for this, rather than shoving it all in main(), however, in the,
 * interest of time, and in the interest of adhering as closely as possible to
 * our initial diagrams from assignment 2, which we have already had to deviate
 * from significantly.
 */
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
        mainbby:
        g_print("\n\033[1;4mCurrent inventory:\033[0m\n");
        vector<SKU> prodlist = globals::local_storage.getProducts();
        for (auto upc : prodlist) {
            Product prod = globals::local_storage.getProduct(upc);
            if (prod.getQuantity() == 0) {
                continue;
            }
            g_print("%lu:\t\033[1m%s\033[0m\n  ", upc, prod.getName().c_str());
            g_print("Price: $%04.2f\t", prod.getPrice());
            g_print("On hand: %u\t", prod.getQuantity());
            g_print("Category: %s\n", prod.getCategory().c_str());
        }
        mainhelp:
        g_print("\n\033[1;4mAvailable commands:\033[0m\n");
        g_print("  addtocart <UPC> [qty]     Add product to cart\n");
        g_print("  cart                      Manage shopping cart\n");
        g_print("  account                   Manage OSC account\n");
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
                if (globals::logged_in.shopping_cart.isEmpty()) {
                    g_print("Cart is empty.\n");
                } else {
                    total = 0;
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
                }
                carthelp:
                g_print("\n\033[1;4mAvailable commands:\033[0m\n");
                if (!globals::logged_in.shopping_cart.isEmpty()) {
                    g_print("  changeqty <UPC> <qty>     Change quantity of item\n");
                    g_print("  remove <UPC>              Remove item from cart\n");
                    g_print("  clear                     Remove all items from cart\n");
                    if(!invalid_qty) {
                        g_print("  checkout                  Begin checkout process\n");
                    }
                }
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
                    if (cmd_parts.size() < 2) {
                        g_printerr("Invalid use of remove.\n");
                        goto carthelp;
                    }
                    if (globals::logged_in.shopping_cart.isEmpty()) {
                        g_printerr("Cart is empty. Nothing to remove.\n");
                        goto carthelp;
                    }
                    SKU sel_upc;
                    try {
                        sel_upc = stoul(cmd_parts[1]);
                    } catch (const invalid_argument &e) {
                        g_printerr("'%s' is not a valid UPC.\n", cmd_parts[1].c_str());
                        continue;
                    }
                    if (!globals::logged_in.shopping_cart.checkCart(sel_upc)) {
                        g_printerr("No item with UPC %lu exists in the cart.\n", sel_upc);
                        continue;
                    }
                    globals::logged_in.shopping_cart.deleteProduct(sel_upc);
                    g_print("Item removed from cart.\n");
                    continue;
                } else if (cmd_parts[0] == "changeqty") {
                    if (cmd_parts.size() < 3) {
                        g_printerr("Invalid use of changeqty.\n");
                        goto carthelp;
                    }
                    if (globals::logged_in.shopping_cart.isEmpty()) {
                        g_printerr("Cart is empty. No item quantities to change.\n");
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
                        g_printerr("No item with UPC %lu exists in the cart.\n", sel_upc);
                        continue;
                    }
                    if (new_qty > globals::logged_in.shopping_cart.cart[sel_upc].first.getQuantity()) {
                        g_printerr("WARNING: There are only %u of this item in stock.\n", globals::logged_in.shopping_cart.cart[sel_upc].first.getQuantity());
                        g_printerr("         The specified quantity %u is greater than our on-hand count.\n", new_qty);
                        g_printerr("         You will noif (cmd.front() != 'n' && cmd.front() != 'N')t be able to checkout with an invalid quantity.\n");
                        g_printerr("         WE CANNOT PLACE ITEMS ON BACKORDER.\n");
                        g_printerr("\n");
                    }
                    globals::logged_in.shopping_cart.changeQuantity(sel_upc, new_qty);
                    g_print("Quantity updated.\n");
                    continue;
                } else if (cmd_parts[0] == "clear") {
                    if (globals::logged_in.shopping_cart.isEmpty()) {
                        g_printerr("Cart is empty. Nothing to do.\n");
                        goto carthelp;
                    }
                    globals::logged_in.shopping_cart.emptyCart();
                    g_print("Your cart has been emptied.\n");
                    g_print("Returning to main menu...\n");
                    break;
                } else if (cmd_parts[0] == "checkout") {
                    // ohhhh boy, here we go
                    if (globals::logged_in.shopping_cart.isEmpty()) {
                        g_printerr("Cannot checkout with an empty cart.\n");
                        goto carthelp;
                    }
                    if (invalid_qty) {
                        g_printerr("Cart contains one or more invalid quantities. Cannot checkout.\n");
                        continue;
                    }
                    Transaction order = globals::logged_in.shopping_cart.buyCart();
                    orderaddr:
                    if (globals::logged_in.shipping_address.is_set) {
                        vector<string> addr_vs;
                        g_print("We have a shipping address on file for you.\n\n");
                        addr_vs = split(globals::logged_in.shipping_address.shipping_name, '\n');
                        for (auto nameline : addr_vs) {
                            g_print("  %s\n", nameline.c_str());
                        }
                        addr_vs = split(globals::logged_in.shipping_address.street, '\n');
                        for (auto streetline : addr_vs) {
                            g_print("  %s\n", streetline.c_str());
                        }
                        g_print("  %s, %s %u\n", globals::logged_in.shipping_address.city.c_str(), globals::logged_in.shipping_address.state.c_str(), globals::logged_in.shipping_address.zip);
                        g_print("  %s\n\n", globals::logged_in.shipping_address.country.c_str());
                        g_print("Would you like to use this shipping address for this order [Y/n]? ");
                        getline(cin, cmd);
                        if (cmd.front() != 'n' && cmd.front() != 'N') {
                            order.setShippingAddress(globals::logged_in.shipping_address);
                        } else {
                            g_print("\n");
                        }
                    }
                    if (!order.shipping_address.is_set) {
                        ShippingAddress addr;
                        string name1_s, name2_s, street1_s, street2_s;
                        string city_s, state_s, zip_s, country_s;
                        g_print("We need a shipping address for this order\n\n");
                        g_print("Please enter the first line of the name field.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, name1_s);
                        string name1_ts;
                        for (auto c : name1_s) {
                            if (!isspace(c))
                                name1_ts += c;
                        }
                        if (name1_ts.empty()) {
                            g_printerr("This field cannot be blank.\n\n");
                            goto orderaddr;
                        }

                        g_print("Please enter the second line of the name field.\n");
                        g_print("If you do not need a second line for this field, just press enter.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, name2_s);
                        string name2_ts;
                        for (auto c : name2_s) {
                            if (!isspace(c))
                                name2_ts += c;
                        }
                        if (!name2_ts.empty()) {
                            name1_s.append("\n");
                            name1_s.append(name2_s);
                        }
                        addr.changeShippingName(name1_s);

                        g_print("Please enter the first line of the street address field.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, street1_s);
                        string street1_ts;
                        for (auto c : street1_s) {
                            if (!isspace(c))
                                street1_ts += c;
                        }
                        if (street1_ts.empty()) {
                            g_printerr("This field cannot be blank.\n\n");
                            goto orderaddr;
                        }

                        g_print("Please enter the second line of the street address field.\n");
                        g_print("If you do not need a second line for this field, just press enter.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, street2_s);
                        string street2_ts;
                        for (auto c : street2_s) {
                            if (!isspace(c))
                                street2_ts += c;
                        }
                        if (!street2_ts.empty()) {
                            street1_s.append("\n");
                            street1_s.append(street2_s);
                        }
                        addr.changeStreet(street1_s);

                        g_print("Please enter the city field.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, city_s);
                        string city_ts;
                        for (auto c : city_s) {
                            if (!isspace(c))
                                city_ts += c;
                        }
                        if (city_ts.empty()) {
                            g_printerr("This field cannot be blank.\n\n");
                            goto orderaddr;
                        }
                        addr.changeCity(city_s);

                        g_print("Please enter the state/province/territory field.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, state_s);
                        string state_ts;
                        for (auto c : state_s) {
                            if (!isspace(c))
                                state_ts += c;
                        }
                        if (state_ts.empty()) {
                            g_printerr("This field cannot be blank.\n\n");
                            goto orderaddr;
                        }
                        addr.changeState(state_s);

                        g_print("Please enter the postal code field.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, zip_s);
                        unsigned int zip;
                        try {
                            zip = (unsigned int)stoul(zip_s);
                        } catch (const invalid_argument &e) {
                            g_printerr("This is not a valid entry for this field.\n");
                            g_printerr("Currently, only numerical postal codes are accepted.\n");
                            g_printerr("Sorry, Canadian customers! You'll have to call in to place an order.\n");
                            goto orderaddr;
                        }
                        addr.changeZip(zip);

                        g_print("Please enter the country field.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, country_s);
                        string country_ts;
                        for (auto c : country_s) {
                            if (!isspace(c))
                                country_ts += c;
                        }
                        if (country_ts.empty()) {
                            g_printerr("This field cannot be blank.\n\n");
                            goto orderaddr;
                        }
                        addr.changeCountry(country_s);

                        vector<string> addr_vs;
                        g_print("\n");
                        g_print("You entered:\n\n");
                        addr_vs = split(addr.shipping_name, '\n');
                        for (auto nameline : addr_vs) {
                            g_print("  %s\n", nameline.c_str());
                        }
                        addr_vs = split(addr.street, '\n');
                        for (auto streetline : addr_vs) {
                            g_print("  %s\n", streetline.c_str());
                        }
                        g_print("  %s, %s %u\n", addr.city.c_str(), addr.state.c_str(), addr.zip);
                        g_print("  %s\n\n", addr.country.c_str());

                        g_print("Is this correct [Y/n]? ");
                        getline(cin, cmd);
                        if (cmd.front() == 'n' || cmd.front() == 'N') {
                            g_print("\n");
                            goto orderaddr;
                        }
                        order.setShippingAddress(addr);

                        g_print("Would you like to keep this address on file [Y/n]?");
                        if (cmd.front() != 'n' && cmd.front() != 'N') {
                            globals::logged_in.changeShippingAddress(addr);
                        }
                    }
                    ordercc:
                    if (globals::logged_in.payment_info.is_set) {
                        g_print("\nWe have the following payment method on file for you:\n");
                        g_print("  Card ending in %lu, expiring on %s\n\n", globals::logged_in.payment_info.cc_num % (unsigned long)10000, globals::logged_in.payment_info.cc_exp.c_str());
                        g_print("Would you like to use this payment method for this order [Y/n]? ");
                        getline(cin, cmd);
                        if (cmd.front() != 'n' && cmd.front() != 'N') {
                            order.setPaymentInfo(globals::logged_in.payment_info);
                        }
                    }
                    if (!order.payment_info.is_set) {
                        unsigned long cc_num;
                        unsigned int cc_cv2;
                        string cc_exp, num_s, cv2_s;
                        g_print("\nWe need a payment method for this order\n\n");
                        g_print("Please enter your card number.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, num_s);
                        try {
                            cc_num = stoul(num_s);
                        } catch (const invalid_argument &e) {
                            g_printerr("This is not a valid entry for this field.\n");
                            goto ordercc;
                        }

                        g_print("Please enter the expiration date of the card.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, cc_exp);
                        string exp_ts;
                        for (auto c : cc_exp) {
                            if (!isspace(c))
                                exp_ts += c;
                        }
                        if (exp_ts.empty()) {
                            g_printerr("This field cannot be blank.\n");
                            goto ordercc;
                        }

                        g_print("Please enter your card's cv2/cvv2/cvv number.\n");
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, cv2_s);
                        try {
                            cc_cv2 = (unsigned int)stoul(num_s);
                        } catch (const invalid_argument &e) {
                            g_printerr("This is not a valid entry for this field.\n");
                            goto ordercc;
                        }

                        PaymentInfo cc(cc_num, cc_exp, cc_cv2);

                        g_print("\n");
                        g_print("You entered:\n\n");
                        g_print("  %lu\n", cc.cc_num);
                        g_print("  Exp: %s\tCV2:%u\n\n", cc.cc_exp.c_str(), cc.cc_cv2);

                        g_print("Is this correct [Y/n]? ");
                        getline(cin, cmd);
                        if (cmd.front() == 'n' || cmd.front() == 'N') {
                            g_print("\n");
                            goto orderaddr;
                        }
                        order.setPaymentInfo(cc);

                        g_print("Would you like to keep this payment method on file [Y/n]?");
                        if (cmd.front() != 'n' && cmd.front() != 'N') {
                            globals::logged_in.changeCreditCard(cc);
                        }
                    }
                    g_print("\n\n");
                    g_print("\n\033[1;4mORDER CONFIRMATION\033[0m\n\n");
                    total = 0;
                    for (auto prod_spp : order.shopping_cart.cart) {
                        total += prod_spp.second.first.getPrice() * float(prod_spp.second.second);
                        g_print("%lu:\t\033[1m%s\033[0m\n  ", prod_spp.first, prod_spp.second.first.getName().c_str());
                        g_print("Qty: %u\t", prod_spp.second.second);
                        g_print("Price: $%04.2f ($%04.2f/ea)\n", prod_spp.second.first.getPrice() * float(prod_spp.second.second), prod_spp.second.first.getPrice());
                    }
                    g_print("\033[1mOrder Total: $%04.2f\033[0m\n\n", total);

                    g_print("\033[4mShip to:\033[0m\n");
                    g_print("%s\n", order.shipping_address.shipping_name.c_str());
                    g_print("%s\n", order.shipping_address.street.c_str());
                    g_print("%s, %s %u\n", order.shipping_address.city.c_str(), order.shipping_address.state.c_str(), order.shipping_address.zip);
                    g_print("%s\n\n", order.shipping_address.country.c_str());

                    g_print("\033[4mPayment method:\033[0m\n");
                    g_print("Card ending in %lu, expiring on %s\n\n\n", order.payment_info.cc_num % (unsigned long)10000, order.payment_info.cc_exp.c_str());

                    g_print("Place order [Y/n]?");
                    getline(cin, cmd);
                    if (cmd.front() != 'n' && cmd.front() != 'N') {
                        order.chargeCreditCard();
                        g_print("\nOrder placed!\n\n\n");
                        goto mainbby;
                    }
                } else {
                    g_printerr("'%s' is an unrecognized command.\n", cmd_parts[0].c_str());
                    goto carthelp;
                }
            }
        } else if (cmd_parts[0] == "account") {
            while(true) {
                g_print("\n\033[1;4mAccount Details:\033[0m\n");
                if (globals::logged_in.shipping_address.is_set) {
                    g_print("\n\033[4mShipping address:\033[0m\n");
                    g_print("%s\n", globals::logged_in.shipping_address.shipping_name.c_str());
                    g_print("%s\n", globals::logged_in.shipping_address.street.c_str());
                    g_print("%s, %s %u\n", globals::logged_in.shipping_address.city.c_str(), globals::logged_in.shipping_address.state.c_str(), globals::logged_in.shipping_address.zip);
                    g_print("%s\n\n", globals::logged_in.shipping_address.country.c_str());
                } else {
                    g_print("No shipping address on file.\n");
                }
                if (globals::logged_in.payment_info.is_set) {
                    g_print("\033[4mPayment method:\033[0m\n");
                    g_print("Card ending in %lu, expiring on %s\n", globals::logged_in.payment_info.cc_num % (unsigned long)10000, globals::logged_in.payment_info.cc_exp.c_str());
                } else {
                    g_print("No payment method on file.\n");
                }
                acchelp:
                g_print("\n\033[1;4mAvailable commands:\033[0m\n");
                g_print("  addr                      Change/set shipping address\n");
                g_print("  pay                       Change/set payment method\n");
                g_print("  orders                    View order history\n");
                g_print("  back                      Back to main menu\n");
                g_print("  exit                      Log out and close OSC\n");
                g_print("\n");
                accshell:
                g_print("\033[1m>>>\033[0m ");
                getline(cin, cmd);
                cmd_parts = split(cmd, ' ');
                if (cmd_parts.empty()) {
                    goto accshell;
                } else if (cmd_parts[0] == "exit") {
                    g_print("Exiting.\n");
                    exit(0);
                } else if (cmd_parts[0] == "back") {
                    break;
                } else if (cmd_parts[0] == "addr") {
                    ShippingAddress addr;
                    string name1_s, name2_s, street1_s, street2_s;
                    string city_s, state_s, zip_s, country_s;
                    g_print("\nPlease enter the first line of the name field.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, name1_s);
                    string name1_ts;
                    for (auto c : name1_s) {
                        if (!isspace(c))
                            name1_ts += c;
                    }
                    if (name1_ts.empty()) {
                        g_printerr("This field cannot be blank.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }
                    g_print("Please enter the second line of the name field.\n");
                    g_print("If you do not need a second line for this field, just press enter.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, name2_s);
                    string name2_ts;
                    for (auto c : name2_s) {
                        if (!isspace(c))
                            name2_ts += c;
                    }
                    if (!name2_ts.empty()) {
                        name1_s.append("\n");
                        name1_s.append(name2_s);
                    }
                    addr.changeShippingName(name1_s);

                    g_print("Please enter the first line of the street address field.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, street1_s);
                    string street1_ts;
                    for (auto c : street1_s) {
                        if (!isspace(c))
                            street1_ts += c;
                    }
                    if (street1_ts.empty()) {
                        g_printerr("This field cannot be blank.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }

                    g_print("Please enter the second line of the street address field.\n");
                    g_print("If you do not need a second line for this field, just press enter.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, street2_s);
                    string street2_ts;
                    for (auto c : street2_s) {
                        if (!isspace(c))
                            street2_ts += c;
                    }
                    if (!street2_ts.empty()) {
                        street1_s.append("\n");
                        street1_s.append(street2_s);
                    }
                    addr.changeStreet(street1_s);

                    g_print("Please enter the city field.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, city_s);
                    string city_ts;
                    for (auto c : city_s) {
                        if (!isspace(c))
                            city_ts += c;
                    }
                    if (city_ts.empty()) {
                        g_printerr("This field cannot be blank.\n\n");
                        g_printerr("This field cannot be blank.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }
                    addr.changeCity(city_s);

                    g_print("Please enter the state/province/territory field.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, state_s);
                    string state_ts;
                    for (auto c : state_s) {
                        if (!isspace(c))
                            state_ts += c;
                    }
                    if (state_ts.empty()) {
                        g_printerr("This field cannot be blank.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }
                    addr.changeState(state_s);

                    g_print("Please enter the postal code field.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, zip_s);
                    unsigned int zip;
                    try {
                        zip = (unsigned int)stoul(zip_s);
                    } catch (const invalid_argument &e) {
                        g_printerr("This is not a valid entry for this field.\n");
                        g_printerr("Currently, only numerical postal codes are accepted.\n");
                        g_printerr("Sorry, Canadian customers! You'll have to call in to place an order.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }
                    addr.changeZip(zip);

                    g_print("Please enter the country field.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, country_s);
                    string country_ts;
                    for (auto c : country_s) {
                        if (!isspace(c))
                            country_ts += c;
                    }
                    if (country_ts.empty()) {
                        g_printerr("This field cannot be blank.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }
                    addr.changeCountry(country_s);

                    vector<string> addr_vs;
                    g_print("\n");
                    g_print("You entered:\n\n");
                    addr_vs = split(addr.shipping_name, '\n');
                    for (auto nameline : addr_vs) {
                        g_print("  %s\n", nameline.c_str());
                    }
                    addr_vs = split(addr.street, '\n');
                    for (auto streetline : addr_vs) {
                        g_print("  %s\n", streetline.c_str());
                    }
                    g_print("  %s, %s %u\n", addr.city.c_str(), addr.state.c_str(), addr.zip);
                    g_print("  %s\n\n", addr.country.c_str());

                    g_print("Is this correct [Y/n]? ");
                    getline(cin, cmd);
                    if (cmd.front() == 'n' || cmd.front() == 'N') {
                        g_print("No changes made.\n\n");
                        continue;
                    }
                    globals::logged_in.changeShippingAddress(addr);
                    g_print("New address set.\n\n");
                } else if (cmd_parts[0] == "pay") {
                    unsigned long cc_num;
                    unsigned int cc_cv2;
                    string cc_exp, num_s, cv2_s;
                    g_print("\nPlease enter your card number.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, num_s);
                    try {
                        cc_num = stoul(num_s);
                    } catch (const invalid_argument &e) {
                        g_printerr("This is not a valid entry for this field.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }

                    g_print("Please enter the expiration date of the card.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, cc_exp);
                    string exp_ts;
                    for (auto c : cc_exp) {
                        if (!isspace(c))
                            exp_ts += c;
                    }
                    if (exp_ts.empty()) {
                        g_printerr("This field cannot be blank.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }

                    g_print("Please enter your card's cv2/cvv2/cvv number.\n");
                    g_print("\033[1m>>>\033[0m ");
                    getline(cin, cv2_s);
                    try {
                        cc_cv2 = (unsigned int)stoul(num_s);
                    } catch (const invalid_argument &e) {
                        g_printerr("This is not a valid entry for this field.\n");
                        g_printerr("No changes made.\n\n");
                        continue;
                    }

                    PaymentInfo cc(cc_num, cc_exp, cc_cv2);

                    g_print("\n");
                    g_print("You entered:\n\n");
                    g_print("  %lu\n", cc.cc_num);
                    g_print("  Exp: %s\tCV2:%u\n\n", cc.cc_exp.c_str(), cc.cc_cv2);

                    g_print("Is this correct [Y/n]? ");
                    getline(cin, cmd);
                    if (cmd.front() == 'n' || cmd.front() == 'N') {
                        g_print("No changes made.\n\n");
                        continue;
                    }
                    globals::logged_in.changeCreditCard(cc);
                    g_print("New payment method set.\n\n");
                } else if (cmd_parts[0] == "orders") {
                    if (globals::logged_in.transaction_history.transaction_list.empty()) {
                        g_print("No past orders.\n");
                        goto accshell;
                    }
                    while(true) {
                        g_print("\n\033[1;4mOrder history:\033[0m\n");
                        unsigned int idx = 1;
                        char tid_cs[37];
                        char time_cs[81];
                        unsigned int idx_w = ((unsigned int)log10(globals::logged_in.transaction_history.transaction_list.size())+1);
                        if (idx_w < 3)
                            idx_w = 3;
                        for (auto t : globals::logged_in.transaction_history.transaction_list) {
                            uuid_unparse_lower(t.transaction_id, (char *)tid_cs);
                            g_print("%0*u:\t%s\n", idx_w, idx, (char *)tid_cs);
                            strftime((char *)time_cs, 81, "%a, %d %b %Y %H:%M:%S", &t.transaction_date);
                            g_print("  Date: %s   Total: $%04.2f\n", (char *)time_cs, t.transaction_amount);
                            idx++;
                        }
                        g_print("\n\n");
                        thelp:
                        g_print("Enter the index number of the transaction you wish to view.\n"
                                "You can also enter 'back' to go back to the account menu,\n"
                                "or 'exit' to log out of OSC.\n");
                        tshell:
                        g_print("\033[1m>>>\033[0m ");
                        getline(cin, cmd);
                        cmd_parts = split(cmd, ' ');
                        if (cmd_parts.empty()) {
                            goto tshell;
                        } else if (cmd_parts[0] == "exit") {
                            g_print("Exiting.\n");
                            exit(0);
                        } else if (cmd_parts[0] == "back") {
                            break;
                        } else {
                            int t_idx;
                            try {
                                t_idx = (unsigned int)stoul(cmd_parts[0]);
                            } catch (const invalid_argument &e) {
                                goto thelp;
                            }
                            Transaction t;
                            try {
                                t = globals::logged_in.transaction_history.transaction_list.at(t_idx-1);
                            } catch (const out_of_range &e) {
                                continue;
                            }
                            uuid_unparse_lower(t.transaction_id, (char *)tid_cs);
                            strftime((char *)time_cs, 81, "%a, %d %b %Y %H:%M:%S", &t.transaction_date);
                            g_print("\n\n\033[1;4mOrder %s:\033[0m\n\n", (char *)tid_cs);;
                            for (auto prod_spp : t.shopping_cart.cart) {
                                g_print("%lu:\t\033[1m%s\033[0m\n  ", prod_spp.first, prod_spp.second.first.getName().c_str());
                                g_print("Qty: %u\n", prod_spp.second.second);
                            }
                            g_print("Date: %s     Total: $%04.2f\n\n", (char *)time_cs, t.transaction_amount);

                            g_print("\033[4mShip to:\033[0m\n");
                            g_print("%s\n", t.shipping_address.shipping_name.c_str());
                            g_print("%s\n", t.shipping_address.street.c_str());
                            g_print("%s, %s %u\n", t.shipping_address.city.c_str(), t.shipping_address.state.c_str(), t.shipping_address.zip);
                            g_print("%s\n\n", t.shipping_address.country.c_str());
                            g_print("\033[4mPayment method:\033[0m\n");
                            g_print("Card ending in %lu, expiring on %s\n\n\n", t.payment_info.cc_num % (unsigned long)10000, t.payment_info.cc_exp.c_str());
                            goto tshell;
                        }
                    }
                } else {
                    g_printerr("'%s' is an unrecognized command.\n", cmd_parts[0].c_str());
                    goto acchelp;
                }
            }
        } else {
            g_printerr("'%s' is an unrecognized command.\n", cmd_parts[0].c_str());
            goto mainhelp;
        }
    }

    return 0;
}
