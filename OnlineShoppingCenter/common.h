#ifndef COMMON_H
#define COMMON_H

#include <glib.h>
/* Relevant documentation:
 * https://developer.gnome.org/glib/stable/glib-Shell-related-Utilities.html
 * https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html
 */

#include <string>
#include <cstdlib>

#include <sstream>
#include <vector>

using namespace std;

typedef unsigned long SKU;

inline vector<string> split(const string &s, char delim) {
    stringstream ss;
    ss.str(s);
    string elem;
    vector<string> ret;
    while (getline(ss, elem, delim)) {
        if (!elem.empty())
            ret.push_back(elem);
    }
    return ret;
}

#endif // COMMON_H
