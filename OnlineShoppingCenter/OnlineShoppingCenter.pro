TEMPLATE = app
CONFIG += console c++11 exceptions
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $${PWD}/../externs/rocksdb/rocksdb-4.9/include

INCLUDEPATH += $${PWD}/../externs/glib/glib-2.49.6/glib
INCLUDEPATH += $${PWD}/../externs/glib/glib-2.49.6
INCLUDEPATH += $${PWD}/../externs/glib/glib-2.49.6/gmodule
INCLUDEPATH += $${OUT_PWD}/../externs/glib/glib

LIBS += $${PWD}/../externs/rocksdb/rocksdb-4.9/librocksdb.a
LIBS += $${PWD}/../externs/rocksdb/rocksdb-4.9/libsnappy.a
LIBS += $${PWD}/../externs/rocksdb/rocksdb-4.9/libbz2.a
LIBS += $${PWD}/../externs/rocksdb/rocksdb-4.9/libz.a
LIBS += $${PWD}/../externs/rocksdb/rocksdb-4.9/liblz4.a

LIBS += $${OUT_PWD}/../externs/glib/glib/.libs/libglib-2.0.a
LIBS += $${OUT_PWD}/../externs/glib/gio/.libs/libgio-2.0.a
LIBS += $${OUT_PWD}/../externs/glib/gmodule/.libs/libgmodule-2.0.a
LIBS += $${OUT_PWD}/../externs/glib/gobject/.libs/libgobject-2.0.a
LIBS += $${OUT_PWD}/../externs/glib/gthread/.libs/libgthread-2.0.a
LIBS += -pthread  -lpcre


SOURCES += \
    main.cpp \
    product.cpp \
    shoppingcart.cpp \
    storagesystem.cpp \
    shippingaddress.cpp \
    user.cpp \
    paymentinfo.cpp \
    transactionhistory.cpp \
    transaction.cpp

HEADERS += \
    product.h \
    shoppingcart.h \
    storagesystem.h \
    shippingaddress.h \
    user.h \
    paymentinfo.h \
    transactionhistory.h \
    common.h \
    transaction.h
