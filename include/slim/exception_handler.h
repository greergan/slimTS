#ifndef __SLIM__EXCEPTION_HANDLER__H
#define __SLIM__EXCEPTION_HANDLER__H
#include <v8.h>
namespace slim::exception_handler {
	void v8_try_catch_handler(v8::TryCatch* try_catch);
}
#endif