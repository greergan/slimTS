#ifndef __SLIM__LOG__H
#define __SLIM__LOG__H
#include <uv.h>
#include <initializer_list>
#include <string>
#include <sstream>
#include <syslog.h>
#include <log_configuration.h>
#include <iostream>
namespace slim::log {
    static uv_loop_t *log_loop;
    struct LogInformation {
        uv_work_t request;
        int priority;
        int options;
        std::string message;
        std::string program;
        int facility;
        template<class... Args>
        LogInformation(const std::string level, Args... args) {
            std::ostringstream messagestream;
            messagestream << level << ": ";
            (messagestream << ... << std::forward<Args>(args));
            message = messagestream.str();
        }
        ~LogInformation(){};
    };
    extern void Init(uv_loop_t *loop);
    static void write_syslog(uv_work_t* request) {
        LogInformation *log_info = (LogInformation*) request->data;
        setlogmask (LOG_UPTO (LOG_DEBUG));
        openlog(log_info->program.c_str(), log_info->options, slim::log::configuration::facility);
        syslog(log_info->priority, "%s", log_info->message.c_str());
    }
    static void write_syslog_complete(uv_work_t* request, int status) {
        LogInformation *log_info = (LogInformation*) request->data;
        log_info->~LogInformation();
        delete request;
    }
    template<class... Args>
    void critical(Args... args) {
        LogInformation* log_info = new LogInformation("CRITICAL", args...);
        log_info->priority = LOG_CRIT;
        log_info->options = slim::log::configuration::critical.options;
        log_info->request.data = (void*) log_info;
        uv_queue_work(log_loop, &log_info->request, write_syslog, write_syslog_complete);
    }
    template<class... Args>
    void debug(Args... args) {
        LogInformation* log_info = new LogInformation("DEBUG", args...);
        log_info->priority = LOG_CRIT;
        log_info->options = slim::log::configuration::debug.options;
        log_info->request.data = (void*) log_info;
        uv_queue_work(log_loop, &log_info->request, write_syslog, write_syslog_complete);
    }
    template<class... Args>
    void error(Args... args) {
        LogInformation* log_info = new LogInformation("ERROR", args...);
        log_info->priority = LOG_CRIT;
        log_info->options = slim::log::configuration::error.options;
        log_info->request.data = (void*) log_info;
        uv_queue_work(log_loop, &log_info->request, write_syslog, write_syslog_complete);
    }
    template<class... Args>
    void info(Args... args) {
        LogInformation* log_info = new LogInformation("INFO", args...);
        log_info->priority = LOG_CRIT;
        log_info->options = slim::log::configuration::info.options;
        log_info->request.data = (void*) log_info;
        uv_queue_work(log_loop, &log_info->request, write_syslog, write_syslog_complete);
    }
    template<class... Args>
    void notice(Args... args) {
        LogInformation* log_info = new LogInformation("NOTICE", args...);
        log_info->priority = LOG_CRIT;
        log_info->options = slim::log::configuration::notice.options;
        log_info->request.data = (void*) log_info;
        uv_queue_work(log_loop, &log_info->request, write_syslog, write_syslog_complete);
    }
    template<class... Args>
    void warn(Args... args) {
        LogInformation* log_info = new LogInformation("WARN", args...);
        log_info->priority = LOG_CRIT;
        log_info->options = slim::log::configuration::warn.options;
        log_info->request.data = (void*) log_info;
std::cerr << "warn 1\n";
        uv_queue_work(log_loop, &log_info->request, write_syslog, write_syslog_complete);
    }
};
#endif