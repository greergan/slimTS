#include "config.h"
#include <iostream>
#include <slim/slim.h>
#include <slim/common/exception.h>
#include <slim/common/log.h>
int main(int argc, char *argv[]) {
    using namespace slim::common;
    log::trace(log::Message("begins",__FILE__, __LINE__));
    try {
        log::trace(log::Message("top of try",__FILE__, __LINE__));
        slim::start(argc, argv);
        log::trace(log::Message("bottom of try",__FILE__, __LINE__));
    }
    catch(std::string exception) {
        log::trace(log::Message("caught exception",__FILE__, __LINE__));
        log::error(log::Message(exception,__FILE__, __LINE__));
        slim::stop();
        log::trace(log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    catch(const slim::common::SlimFileException& error) {
        std::string error_message = error.message + ", path => " + error.path;
        log::error(log::Message(error.call.c_str(), error_message.c_str(),__FILE__, __LINE__));
        slim::stop();
        log::trace(log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    catch(const slim::common::SlimException& error) {
        log::error(log::Message(error.call.c_str(), error.message.c_str(),__FILE__, __LINE__));
        slim::stop();
        log::trace(log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        slim::stop();
        log::trace(log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    catch(...) {
        log::error(log::Message("caught unknown exception",__FILE__, __LINE__));
        slim::stop();
        log::trace(log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    slim::stop();
    log::trace(log::Message("exiting",__FILE__, __LINE__));
    return 0;
}