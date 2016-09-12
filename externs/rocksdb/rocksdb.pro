#MAKEFILE = Makefile.rocksdb

ROCKSDB_SRCDIR = $${PWD}/rocksdb-4.9

snappy.target = $${ROCKSDB_SRCDIR}/libsnappy.a
snappy.commands = cd $${ROCKSDB_SRCDIR} && make -j9 libsnappy.a DEBUG_LEVEL=0 MAKEFLAGS='-j9'

zlib.target = $${ROCKSDB_SRCDIR}/libz.a
zlib.commands = cd $${ROCKSDB_SRCDIR} && make -j9 libz.a DEBUG_LEVEL=0 MAKEFLAGS='-j9'

bzip2.target = $${ROCKSDB_SRCDIR}/libbz2.a
bzip2.commands = cd $${ROCKSDB_SRCDIR} && make -j9 libbz2.a DEBUG_LEVEL=0 MAKEFLAGS='-j9'

lz4.target = $${ROCKSDB_SRCDIR}/liblz4.a
lz4.commands = cd $${ROCKSDB_SRCDIR} && make -j9 liblz4.a DEBUG_LEVEL=0 MAKEFLAGS='-j9'

librocksdb.target = $${ROCKSDB_SRCDIR}/librocksdb.a
librocksdb.commands = cd $${ROCKSDB_SRCDIR} && make -j9 static_lib
librocksdb.depends = snappy zlib bzip2 lz4

librocksdb_clean.commands = cd $${ROCKSDB_SRCDIR} && make -f Makefile clean
librocksdb_distclean.commands = cd $${ROCKSDB_SRCDIR} && make -f Makefile distclean

qmake_all.depends = librocksdb
all.depends = librocksdb
clean.depends = librocksdb_clean
distclean.depends = librocksdb_distclean

CONFIG = staticlib
TARGET =

QMAKE_EXTRA_TARGETS = librocksdb librocksdb_clean all qmake_all distclean clean librocksdb_distclean snappy zlib bzip2 lz4
