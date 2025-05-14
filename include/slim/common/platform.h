#ifndef __SLIM__COMMON__PLATFORM
#define __SLIM__COMMON__PLATFORM
#include <string>
#ifdef _AIX
#define OS_NAME  "aix"
#elif __MACH__
#define OS_NAME  "darwin";
#elif __FreeBSD__
#define OS_NAME  "freebsd"
#elif __linux__
#define OS_NAME  "linux"
#elif __OpenBSD__
#define OS_NAME  "openbsd"
#elif __sun || __SunOS_5_11
#define OS_NAME  "openbsd"
#elif  _WIN32 || _WIN64
#define OS_NAME  "Win32"
#else
#define OS_NAME  "unknown"
#endif
#if  _WIN32 || _WIN64
#define OS_EOL  "\r\n"
#else
#define OS_EOL  "\n"
#endif
namespace slim::common::platform {
	const std::string eol(OS_EOL);
}

#endif