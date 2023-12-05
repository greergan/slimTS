AC_COPYRIGHT([[Copyright 2022-2023 Jeff Greer. All rights reserved. Restrictions apply, see COPYRIGHT and LICENSE files.]])
AC_CONFIG_SRCDIR(src)
AC_INIT([slim], [0.1.0])
AC_ARG_WITH(google-v8-dir, [--with-google-v8-dir=DIR   path to installed Google v8 source directory])
AC_ARG_WITH(jsoncpp-dir,   [--with-jsoncpp-dir=DIR     path to installed jsoncpp source directory])
AC_LANG_PUSH([C++])
if[ "$with_google_v8_dir" == "" ]; then
	AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8-dir=DIR])
else
	CPPFLAGS="-I$with_google_v8_dir/include $CPPFLAGS"
	LDFLAGS="-L$with_google_v8_dir/out.gn/x64.release.sample/obj $LDFLAGS"
	google_v8_lib="$with_google_v8_dir/out.gn/x64.release.sample/obj/libv8_monolith.a"
fi
if[ "$with_jsoncpp_dir" == "" ]; then
	AC_MSG_ERROR([could not locate jsoncpp main library header file, use --with-jsoncpp-dir=DIR])
else
	CPPFLAGS="-I$with_jsoncpp_dir/include $CPPFLAGS"
	LDFLAGS="-L$with_jsoncpp_dir/build-static $LDFLAGS"
	jsoncpp_lib="$with_jsoncpp_dir/build-static/libjsoncpp.a"
fi
AC_CHECK_HEADERS([v8.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8--dir=DIR])])
AC_CHECK_HEADERS([libplatform/libplatform.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8-dir=DIR])])
AC_CHECK_FILE([$google_v8_lib], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_monolith.a, has it been compiled?"])])
AC_CHECK_HEADERS([json/json.h], [],[AC_MSG_ERROR([could not locate jsoncpp main library header file, use --with-jsoncpp-dir=DIR])])
AC_CHECK_FILE([$jsoncpp_lib], [], [AC_MSG_ERROR(["could not locate jsoncpp static library libjsoncpp.a, has it been compiled?"])])
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AM_INIT_AUTOMAKE
AC_PROG_CXX
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT