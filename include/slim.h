#ifndef __SLIM__H
#define __SLIM__H
#include <string>
namespace slim {
    void run(const std::string& file_name, const std::string& file_contents);
    void start(int argc, char* argv[]);
    void stop(void);
    void version(void);
}
#endif