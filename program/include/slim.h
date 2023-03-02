#ifndef __SLIM__H
#define __SLIM__H
#include <string>
namespace slim {
    static void Run(const std::string file_name, const std::string file_contents);
    extern void Stop(void);
    extern void Start(int argc, char* argv[]);
}
#endif