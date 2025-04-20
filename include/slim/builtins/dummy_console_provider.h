#ifndef __SLIM_DUMMY_CONSOLE_PROVIDER__HPP
#define __SLIM_DUMMY_CONSOLE_PROVIDER__HPP
#include <v8.h>
#include <slim/plugin.hpp>
/*
 * Reference https://console.spec.whatwg.org/#printer
 */
namespace slim::dummy_console {
	void expose_plugin(v8::Isolate* isolate);
}
#endif