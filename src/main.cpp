#include "config.h"
#include <iostream>
#include <slim.h>
int main(int argc, char *argv[]) {
    try {
        slim::version();
        slim::start(argc, argv);
    }
    catch(std::string exception) {
        std::cerr << exception << "\n";
        slim::stop();
        exit(1);
    }
    catch(...) {
        std::cerr << "Unknown exception" << "\n";
        slim::stop();
        exit(1);
    }
    return 0;
}