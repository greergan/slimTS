#ifndef __SLIM__COMMON__FETCH__H
#define __SLIM__COMMON__FETCH__H
#include <sstream>
#include <slim/common/fetch.h>
namespace slim::common::fetch {
    std::stringstream fetch(const char* file_name);
    std::stringstream fetch(const std::string file_name);
}
#endif