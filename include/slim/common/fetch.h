#ifndef __SLIM__COMMON__FETCH__H
#define __SLIM__COMMON__FETCH__H
#include <memory>
#include <sstream>
#include <slim/common/http/request.h>
namespace slim::common::fetch {
    std::unique_ptr<std::string> string(std::string& file_name);
    std::unique_ptr<std::stringstream> stream(std::string& file_name);
    std::unique_ptr<slim::common::http::Request> http_request(std::string& request_string);
    //std::unique_ptr<slim::common::http::Request> http_request(std::unique_ptr<slim::common::http::Request> request_object);
}
#endif