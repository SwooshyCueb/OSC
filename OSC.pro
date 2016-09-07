TEMPLATE = subdirs

SUBDIRS = \
    glib \
    rocksdb \
    OnlineShoppingCenter

glib.subdir = externs/glib
glib.makefile = Makefile.glib

rocksdb.subdir = externs/rocksdb

OnlineShoppingCenter.subdir = OnlineShoppingCenter
OnlineShoppingCenter.depends = glib rocksdb

