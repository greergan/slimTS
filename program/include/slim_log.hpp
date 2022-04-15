#ifndef __SLIM__LOG__HPP
#define __SLIM__LOG__HPP
#include <iostream>
#include <sstream>
#include <initializer_list>
#include <cstdarg>
#include <syslog.h>
#include <uv.h>
namespace slim::log::console
{
    
};
namespace slim::log::system {
    uv_loop_t *log_loop;
    void init(uv_loop_t *loop) {
        log_loop = loop;
    }
    class _base_log_info {
        public:
        uv_work_t request;
        int priority;
        int options = LOG_CONS | LOG_PID;
        std::string message;
        std::string program = "slim";
        int facility = LOG_LOCAL7;
        ~_base_log_info() {
            std::cout << "destructing\n";
        }
    };
    void write_syslog(uv_work_t* request) {
        _base_log_info *log_info = (_base_log_info*) request->data;
        setlogmask (LOG_UPTO (LOG_DEBUG));
        openlog(log_info->program.c_str(), log_info->options, log_info->facility);
        syslog(log_info->priority, "%s", log_info->message.c_str());
    }
    void done_write_syslog(uv_work_t* request, int status) {
        delete request;
    }
    template<class... Args>
    std::string fold_message_args(const char* level, Args... args) {
        std::stringstream message;
        message << level << ": ";
        (message << ... << std::forward<Args>(args));
        return message.str();
    }
    struct critical {
        template<class... Args>
        critical(Args... args) {
            _base_log_info* log_info = new _base_log_info();
            log_info->request.data = (void*) log_info;
            log_info->priority = LOG_CRIT;
            log_info->options = log_info->options | LOG_PERROR;
            log_info->message = fold_message_args("CRITICAL", args...);
            uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
        }
    };
    struct debug {
        template<class... Args>
        debug(Args... args) {
            _base_log_info* log_info = new _base_log_info();
            log_info->request.data = (void*) log_info;
            log_info->priority = LOG_DEBUG;
            log_info->message = fold_message_args("DEBUG", args...);
            uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
        }
    };
    struct error {
        template<class... Args>
        error(Args... args) {
            _base_log_info* log_info = new _base_log_info();
            log_info->request.data = (void*) log_info;
            log_info->priority = LOG_ERR;
            log_info->message = fold_message_args("ERROR", args...);
            uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
        }
    };
    struct info {
        template<class... Args>
        info(Args... args) {
            _base_log_info* log_info = new _base_log_info();
            log_info->request.data = (void*) log_info;
            log_info->priority = LOG_INFO;
            log_info->message = fold_message_args("info", args...);
            uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
        }
    };
    struct notice {
        template<class... Args>
        notice(Args... args) {
            _base_log_info* log_info = new _base_log_info();
            log_info->request.data = (void*) log_info;
            log_info->priority = LOG_NOTICE;
            log_info->options = log_info->options | LOG_PERROR;
            log_info->message = fold_message_args("NOTICE", args...);
            uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
        }
    };
    struct warn {
        template<class... Args>
        warn(Args... args) {
            _base_log_info* log_info = (_base_log_info*) malloc(sizeof(_base_log_info));
            log_info->request.data = (void*) log_info;
            log_info->priority = LOG_WARNING;
            log_info->message = fold_message_args("WARN", args...);
            uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
        }
    };
    void handle_libuv_error(const char* message, int error) {
        if(error) {
            critical(message, uv_strerror(error));
            exit(error);
        }
    }
};
#endif