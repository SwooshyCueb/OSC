#MAKEFILE = Makefile.rocksdb

ROCKSDB_SRCDIR = $${PWD}/rocksdb-4.9

librocksdb.target = $${ROCKSDB_SRCDIR}/librocksdb.a
librocksdb.commands = cd $${ROCKSDB_SRCDIR} && make -f Makefile static_lib -j 9

librocksdb_clean.commands = cd $${ROCKSDB_SRCDIR} && make -f Makefile clean
librocksdb_distclean.commands = cd $${ROCKSDB_SRCDIR} && make -f Makefile distclean

qmake_all.depends = librocksdb
all.depends = librocksdb
clean.depends = librocksdb_clean
distclean.depends = librocksdb_distclean

TARGET =

QMAKE_EXTRA_TARGETS = librocksdb librocksdb_clean all qmake_all distclean clean librocksdb_distclean
