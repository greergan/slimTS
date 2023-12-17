#https://stackoverflow.com/questions/70584133/when-using-libadd-in-automake-libtool-links-incorrectly
# might need this for windows linking -> libbar_la_LDFLAGS += -no-undefined
bin_PROGRAMS  = slim
slim_SOURCES  = main.cpp slim_utilities.cpp http_headers.cpp http_response.cpp http_request.cpp http_url.cpp
slim_CPPFLAGS = -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX -std=c++20 -fno-rtti
slim_LDFLAGS  = -static-libgcc -static-libstdc++ -pthread -rdynamic
slim_LDADD    = -ldl -lv8_monolith -lv8_libbase -lv8_libplatform
