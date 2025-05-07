#ifndef __SLIM__H
#define __SLIM__H
#include <unordered_map>
#include <string>
#include <slim/module_resolver.h>
namespace slim {
    void run(std::string file_name_string_in);
    void start(int argc, char* argv[]);
    void stop(void);
    void version(void);
}
#endif