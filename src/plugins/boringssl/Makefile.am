lib_LTLIBRARIES   = boringssl.la
boringssl_la_SOURCES  = boringssl.cpp
boringssl_la_CPPFLAGS = -std=c++20
boringssl_la_LDFLAGS  = -module -shared -avoid-version
boringssl_la_LIBADD   = -lssl -lpki -lcrypto