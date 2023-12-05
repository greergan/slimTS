#https://stackoverflow.com/questions/70584133/when-using-libadd-in-automake-libtool-links-incorrectly
# might need this for windows linking -> libbar_la_LDFLAGS += -no-undefined
bin_PROGRAMS  = slim
slim_SOURCES  = main.cpp
slim_CPPFLAGS = "-I$(srcdir)/include" -DV8_COMPRESS_POINTERS 
#-std=c++20 -fconcepts
slim_LDFLAGS  = -static-libgcc -static-libstdc++ -pthread -rdynamic
slim_LDADD    = -ldl -lv8_monolith -ljsoncpp


