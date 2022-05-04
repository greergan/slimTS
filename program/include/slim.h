#ifndef __SLIM__H
#define __SLIM__H
#include <string>
namespace slim {
    static void expose(void);
    extern void run(const std::string file_name, const std::string file_contents);
    extern void stop(void);
    extern void start(int argc, char* argv[]);
}
#endif