m4_define([_SLIM_VERSION], [0.1.0])
m4_define([_SLIM_LIB_VERSION], [1:0:0])
m4_define([_SLIM_CONSOLE_VERSION], [1:0:0])
AC_COPYRIGHT([[Copyright 2022-2023 Jeff Greer. All rights reserved. Restrictions apply, see COPYRIGHT and LICENSE files.]])
AC_INIT([slim], [_SLIM_VERSION])
libdir='${exec_prefix}/lib/slim'
moduledir='${exec_prefix}/share/slim'
AC_SUBST([SLIM_VERSION], [_SLIM_VERSION])
AC_SUBST([SLIM_LIB_VERSION], [_SLIM_LIB_VERSION])
AC_SUBST([SLIM_CONSOLE_VERSION], [_SLIM_CONSOLE_VERSION])
#AC_ARG_ENABLE(graphical-console, [--disable-graphical-console build graphical_console plugin])
AC_ARG_WITH(google-v8-dir, [--with-google-v8-dir=DIR   path to installed Google v8 source directory])
AC_ARG_WITH(jsoncpp-dir,   [--with-jsoncpp-dir=DIR     path to installed jsoncpp source directory])
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
if[ "$with_jsoncpp_dir" == "" ]; then
	AC_MSG_ERROR([could not locate jsoncpp main library header file, use --with-jsoncpp-dir=DIR])
else
	CPPFLAGS="-I$with_jsoncpp_dir/include $CPPFLAGS"
	LDFLAGS="-L$with_jsoncpp_dir/build-static $LDFLAGS"
	libjsoncpp="$with_jsoncpp_dir/build-static/libjsoncpp.a"
fi
AC_CHECK_HEADERS([v8.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8--dir=DIR])])
AC_CHECK_HEADERS([libplatform/libplatform.h], [],[AC_MSG_ERROR([could not locate Google v8 main library header file, use --with-google-v8-dir=DIR])])
AC_CHECK_FILE([$libv8_libbase], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_libbase.a, has it been compiled?"])])
AC_CHECK_FILE([$libv8_monolith], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_monolith.a, has it been compiled?"])])
AC_CHECK_FILE([$libv8_libplatform], [], [AC_MSG_ERROR(["could not locate Google v8 static library libv8_libplatform.a, has it been compiled?"])])
AC_CHECK_HEADERS([json/json.h], [],[AC_MSG_ERROR([could not locate jsoncpp main library header file, use --with-jsoncpp-dir=DIR])])
AC_CHECK_FILE([$libjsoncpp], [], [AC_MSG_ERROR(["could not locate jsoncpp static library libjsoncpp.a, has it been compiled?"])])

PKG_PROG_PKG_CONFIG
#gtkmm dependencies
#sigc++-3.0
#gtk4
#glibmm-2.68
#cairomm-1.16
#pangomm-2.48
# dependencies of above dependencies
#pkg-config
#glib-2.0
#pango
#cairo
#gdk-pixbuf-2.0
#graphene-1.0

AC_MSG_CHECKING([pangomm-2.48])
PKG_CHECK_EXISTS([pangomm-2.48],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([cairomm-1.16])
PKG_CHECK_EXISTS([cairomm-1.16],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
#AC_MSG_CHECKING([libgtk-4-1])
#PKG_CHECK_EXISTS([libgtk-4-1],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
#AC_MSG_CHECKING([ibgtk-4-dev])
#PKG_CHECK_EXISTS([ibgtk-4-dev],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
#AC_MSG_CHECKING([pangocairo-1.0])
#PKG_CHECK_EXISTS([pangocairo-1.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
#AC_MSG_CHECKING([pango-1.0])
#PKG_CHECK_EXISTS([pango-1.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([harfbuzz])
PKG_CHECK_EXISTS([harfbuzz],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
#AC_MSG_CHECKING([gdk_pixbuf-2.0])
#PKG_CHECK_EXISTS([gdk_pixbuf-2.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([cairo-gobject])
PKG_CHECK_EXISTS([cairo-gobject],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([cairo])
PKG_CHECK_EXISTS([cairo],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([graphene-1.0])
PKG_CHECK_EXISTS([graphene-1.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([gio-2.0])
PKG_CHECK_EXISTS([gio-2.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([gobject-2.0])
PKG_CHECK_EXISTS([gobject-2.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])

AC_MSG_CHECKING([sigc++-3.0])
PKG_CHECK_EXISTS([sigc++-3.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([glib-2.0])
PKG_CHECK_EXISTS([glib-2.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([glibmm-2.68])
PKG_CHECK_EXISTS([glibmm-2.68],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([giomm-2.68])
PKG_CHECK_EXISTS([giomm-2.68],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])
AC_MSG_CHECKING([gtkmm-4.0])
PKG_CHECK_EXISTS([gtkmm-4.0],[AC_MSG_RESULT([yes])],[AC_MSG_ERROR([no])])

cwd=`pwd`
CPPFLAGS="-I$cwd/include $CPPFLAGS"
LDFLAGS="-L$cwd/src/lib/.libs $LDFLAGS"
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AM_INIT_AUTOMAKE
AC_PROG_CXX
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/plugins/console/Makefile
	src/plugins/graphical_console/Makefile
])
LT_INIT
AC_OUTPUT