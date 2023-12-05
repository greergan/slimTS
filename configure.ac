dnl run using autoreconf -vfi

AC_COPYRIGHT([[Copyright 2022-2023 Jeff Greer. All rights reserved. Restrictions apply, see COPYRIGHT and LICENSE files.]])
AC_CONFIG_SRCDIR(src)
AC_INIT([slim], [0.1.0])
AC_ARG_WITH(google-v8-dir, [--with-google-v8-dir=DIR   path to installed Google v8 source directory])
AC_LANG_PUSH([C++])
if[ "$with_google_v8_dir" == "" ]; then
	AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8--dir=DIR])
else
	CPPFLAGS="-I$with_google_v8_dir/include"
	LDFLAGS="-L$with_google_v8_dir/out.gn/x64.release.sample/obj"
fi
AC_CHECK_HEADERS([v8.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8--dir=DIR])])
AC_CHECK_HEADERS([libplatform/libplatform.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8--dir=DIR])])
AC_CHECK_FILE([$with_google_v8_dir/out.gn/x64.release.sample/obj/libv8_monolith.a], [], [AC_MSG_ERROR(["could not locate Google v8 libraries, have they been compiled?"])])
AC_CHECK_FILE([$with_google_v8_dir/out.gn/x64.release.sample/obj/libv8_libplatform.a], [], [AC_MSG_ERROR(["could not locate Google v8 libraries, have they been compiled?"])])
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