#ifndef __SLIM__LOG__HPP
#define __SLIM__LOG__HPP
#include <iostream>
#include <sstream>
#include <initializer_list>
#include <cstdarg>
#include <syslog.h>
#include <uv.h>
namespace slim::log {
    static uv_loop_t *log_loop;
    void init(uv_loop_t *loop) {
        log_loop = loop;
    }
    struct _base_log_info {
        uv_work_t request;
        int priority;
        int options = LOG_CONS | LOG_PID;
        std::string message;
        std::string program = "slim";
        int facility = LOG_LOCAL7;
        template<class... Args>
        _base_log_info(const char* level, Args... args) {
            std::ostringstream messagestream;
            messagestream << level << ": ";
            (messagestream << ... << std::forward<Args>(args));
            message = messagestream.str();
        }
        ~_base_log_info() = default;
    };
    void write_syslog(uv_work_t* request) {
        _base_log_info *log_info = (_base_log_info*) request->data;
        setlogmask (LOG_UPTO (LOG_DEBUG));
        openlog(log_info->program.c_str(), log_info->options, log_info->facility);
        syslog(log_info->priority, "%s", log_info->message.c_str());
    }
    void done_write_syslog(uv_work_t* request, int status) {
        _base_log_info *log_info = (_base_log_info*) request->data;
        log_info->~_base_log_info();
        delete request;
    }
    template<class... Args>
    static void critical(Args... args) {
        _base_log_info* log_info = new _base_log_info("CRITICAL", args...);
        log_info->request.data = (void*) log_info;
        log_info->priority = LOG_CRIT;
        log_info->options = log_info->options | LOG_PERROR;
        uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
    }
    template<class... Args>
    static void debug(Args... args) {
        _base_log_info* log_info = new _base_log_info("DEBUG", args...);
        log_info->request.data = (void*) log_info;
        log_info->priority = LOG_DEBUG;
        uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
    }
    template<class... Args>
    static void error(Args... args) {
        _base_log_info* log_info = new _base_log_info("ERROR", args...);
        log_info->request.data = (void*) log_info;
        log_info->priority = LOG_ERR;
        uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
    }
    template<class... Args>
    static void info(Args... args) {
        _base_log_info* log_info = new _base_log_info("INFO", args...);
        log_info->request.data = (void*) log_info;
        log_info->priority = LOG_INFO;
        uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
    }
    template<class... Args>
    static void notice(Args... args) {
        _base_log_info* log_info = new _base_log_info("NOTICE", args...);
        log_info->request.data = (void*) log_info;
        log_info->priority = LOG_NOTICE;
        log_info->options = log_info->options | LOG_PERROR;
        uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
    }
    template<class... Args>
    static void warn(Args... args) {
        _base_log_info* log_info = new _base_log_info("WARN", args...);
        log_info->request.data = (void*) log_info;
        log_info->priority = LOG_WARNING;
        uv_queue_work(log_loop, &log_info->request, write_syslog, done_write_syslog);
    }
    void handle_libuv_error(const char* message, int error) {
        if(error) {
            critical(message, uv_strerror(error));
            exit(error);
        }
    }
    void _notice(const v8::FunctionCallbackInfo<v8::Value> &args) {
        std::cerr << "log.notice called \n";
        //notice(args);
    }
};
#endif