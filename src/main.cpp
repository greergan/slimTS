#include "config.h"
#include <iostream>
#include <slim.h>
#include <slim/common/log.h>
int main(int argc, char *argv[]) {
    slim::common::log::debug(true);
    slim::common::log::info(true);
    slim::common::log::trace(true);
    slim::common::log::trace(slim::common::log::Message("begins",__FILE__, __LINE__));
    try {
        slim::common::log::trace(slim::common::log::Message("top of try",__FILE__, __LINE__));
        slim::version();
        slim::start(argc, argv);
        slim::common::log::trace(slim::common::log::Message("bottom of try",__FILE__, __LINE__));
    }
    catch(std::string exception) {
        slim::common::log::trace(slim::common::log::Message("caught exception",__FILE__, __LINE__));
        slim::common::log::error(slim::common::log::Message(exception,__FILE__, __LINE__));
        slim::stop();
        slim::common::log::trace(slim::common::log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    catch(...) {
        slim::common::log::trace(slim::common::log::Message("caught unknown exception",__FILE__, __LINE__));
        slim::common::log::error(slim::common::log::Message("caught unknown exception",__FILE__, __LINE__));
        slim::stop();
        slim::common::log::trace(slim::common::log::Message("exiting",__FILE__, __LINE__));
        exit(1);
    }
    slim::common::log::trace(slim::common::log::Message("exiting",__FILE__, __LINE__));
    return 0;
}