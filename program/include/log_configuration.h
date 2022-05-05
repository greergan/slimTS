#ifndef __SLIM__LOG__CONFIGURATION__H
#define __SLIM__LOG__CONFIGURATION__H
namespace slim::log::configuration {
    extern int facility;
    //.options=LOG_PID|LOG_CONS|LOG_PERROR;
    struct Configuration {
        int options;
    };
    extern Configuration critical, debug, error, info, notice, warn;
};
#endif