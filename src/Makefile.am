#https://stackoverflow.com/questions/70584133/when-using-libadd-in-automake-libtool-links-incorrectly
# might need this for windows linking -> libbar_la_LDFLAGS += -no-undefined
bin_PROGRAMS  = slim
#slim_SOURCES = example.cpp slim_utilities.cpp slim_common_log.cpp
slim_SOURCES  = main.cpp \
				slim.cpp \
				slim_builtins.cpp \
				slim_builtins_dummy_console_provider.cpp \
				slim_builtins_typescript.cpp \
				slim_builtins_typescript_initial_library.cpp \
				slim_command_line_handler.cpp \
				slim_common_fetch.cpp \
				slim_common_fetch_and_apply_macros.cpp \
				slim_common_http.cpp \
				slim_common_http_parser.cpp \
				slim_common_log.cpp \
				slim_common_memory_mapper.cpp \
				slim_common_metrics.cpp \
				slim_common_network_address_set.cpp \
				slim_common_network_client_connection_handler_common.cpp \
				slim_common_network_client_request_handler_metrics.cpp \
				slim_common_network_listener.cpp \
				slim_common_utilities.cpp \
				slim_exception_handler.cpp \
				slim_launcher.cpp \
				slim_macros.cpp \
				slim_metrics_server.cpp \
				slim_module_import_specifier.cpp \
				slim_module_resolver.cpp \
				slim_path.cpp \
				slim_plugin_loader.cpp\
				slim_utilities.cpp \
				simdutf.cpp 
				
slim_CPPFLAGS = -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX -std=c++20 -fno-rtti
slim_LDFLAGS  = -static-libgcc -static-libstdc++ -pthread -rdynamic
slim_LDADD    = -ldl -lv8_monolith -lv8_libbase -lv8_libplatform
