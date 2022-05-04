#ifndef SLIM__LOG__H
#define SLIM__LOG__H
#include <cstdarg>
#include <initializer_list>
namespace slim::log {
    extern void critical(Args... args);
};
#endif