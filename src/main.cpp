#define VERSION "0.0"
#include <iostream>
#include <slim.h>
int main(int argc, char *argv[]) {
    try {
        slim::Start(argc, argv);
    }
    catch(std::string exception) {
        std::cerr << exception << "\n";
        slim::Stop();
        exit(1);
    }
    catch(...) {
        std::cerr << "Unknown exception" << "\n";
        slim::Stop();
        exit(1);
    }
    return 0;
}