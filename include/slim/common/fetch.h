#ifndef __SLIM__COMMON__FETCH__H
#define __SLIM__COMMON__FETCH__H
#include <memory>
#include <sstream>
namespace slim::common::fetch {
    std::unique_ptr<std::string> string(const std::string& file_name);
    std::unique_ptr<std::stringstream> stream(const std::string& file_name);
}
#endif