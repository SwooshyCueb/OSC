MAKEFILE = Makefile.glib

GLIB_SRCDIR = $${PWD}/glib-2.49.6

configure.target = Makefile
configure.commands = \
    $${GLIB_SRCDIR}/configure \
        --enable-static \
        --disable-shared \
        --disable-gtk-doc-html

glib.target = glib/libglib-2.0.la
glib.commands = make -j9
glib.depends = configure

glib_clean.commands = make clean || true
glib_distclean.commands = make distclean || true

all.depends = glib
qmake_all.depends = glib
clean.depends = glib_clean
distclean.depends = glib_distclean

CONFIG = staticlib
TARGET =

QMAKE_DISTCLEAN ''= Makefile
QMAKE_EXTRA_TARGETS''= configure all qmake_all glib glib_clean glib_distclean clean distclean
