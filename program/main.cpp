#define VERSION "0.0"
#include <iostream>
#include <slim.hpp>
int main(int argc, char *argv[]) {
    try {
        slim::init(argc, argv);
        slim::start();
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