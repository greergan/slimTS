AC_COPYRIGHT([[Copyright 2022-2023 Jeff Greer. All rights reserved. Restrictions apply, see COPYRIGHT and LICENSE files.]])
AC_INIT([slim], [0.1.0])
libdir='${exec_prefix}/lib/slim'
moduledir='${exec_prefix}/share/slim'
AC_DEFINE([SLIM_ASIO_VERSION],   ["1.28.2"], [ASIO async library version])
AC_SUBST([SLIM_CONSOLE_VERSION], [_SLIM_CONSOLE_VERSION])
AC_SUBST([SLIM_NETWORK_VERSION], [_SLIM_NETWORK_VERSION])
AC_ARG_WITH(google-v8-dir,       [--with-google-v8-dir=DIR   path to installed Google v8 source directory])
AC_ARG_WITH(librdkafka,          [--with-librdkafka=DIR   path to librdkafka installation filesystem i.e. /usr/local])
AC_LANG_PUSH([C++])
if[ "$with_google_v8_dir" == "" ]; then
	AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8-dir=DIR])
else
	CPPFLAGS="-I$with_google_v8_dir/include $CPPFLAGS"
	LDFLAGS="-L$with_google_v8_dir/out.gn/x64.release.sample/obj $LDFLAGS"
	libv8_libbase="$with_google_v8_dir/out.gn/x64.release.sample/obj/libv8_libbase.a"
	libv8_monolith="$with_google_v8_dir/out.gn/x64.release.sample/obj/libv8_monolith.a"
	libv8_libplatform="$with_google_v8_dir/out.gn/x64.release.sample/obj/libv8_libplatform.a"
fi
if[ "$with_librdkafka" == "" ]; then
	AC_MSG_ERROR([could not locate librdkafka installation file system, use --with-librdkafka=DIR])
else
	CPPFLAGS="-I$with_librdkafka/include $CPPFLAGS"
	LDFLAGS="-L$with_librdkafka/lib $LDFLAGS"
	librdkafka="$with_librdkafka/lib/librdkafka.so"
	librdkafkapp="$with_librdkafka/lib/librdkafka++.so"
	KAFKALIB_PATH="$with_librdkafka/lib"
fi
AC_CHECK_HEADERS([v8.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8--dir=DIR])])
AC_CHECK_HEADERS([libplatform/libplatform.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8-dir=DIR])])
AC_CHECK_HEADERS([librdkafka/rdkafkacpp.h], [],[AC_MSG_ERROR([could not locate librdkafka installation file system, use --with-librdkafka=DIR])])
AC_CHECK_FILE([$libv8_libbase], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_libbase.a, has it been compiled?"])])
AC_CHECK_FILE([$libv8_monolith], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_monolith.a, has it been compiled?"])])
AC_CHECK_FILE([$libv8_libplatform], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_libplatform.a, has it been compiled?"])])
AC_CHECK_FILE([$librdkafka], [], [AC_MSG_ERROR([could not locate librdkafka installation file system, use --with-librdkafka=DIR])])
AC_CHECK_FILE([$librdkafkapp], [], [AC_MSG_ERROR([could not locate librdkafka installation file system, use --with-librdkafka=DIR])])
PKG_PROG_PKG_CONFIG
cwd=`pwd`
CPPFLAGS="-I$cwd/include $CPPFLAGS"
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AM_INIT_AUTOMAKE
AC_PROG_CXX
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/plugins/console/Makefile
	src/plugins/fs/Makefile
	src/plugins/kafka/Makefile
	src/plugins/network/Makefile
	src/plugins/process/Makefile
	src/plugins/rapidcsv/Makefile
])
LT_INIT
AC_OUTPUT