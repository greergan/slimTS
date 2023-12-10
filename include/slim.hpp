#ifndef __SLIM__HPP
#define __SLIM__HPP
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <v8.h>
#include <slim/v8.hpp>
#include <slim/dummy_console_provider.hpp>
namespace slim::path {
    std::string getExecutablePath();
    std::string getExecutableDir();
    std::string mergePaths(std::string pathA, std::string pathB);
    bool checkIfFileExists (const std::string& filePath);
#if defined(_WIN32)
    #include <windows.h>
    #include <Shlwapi.h>
    #include <io.h> 
    #define access _access_s
#endif
#ifdef __APPLE__
    #include <libgen.h>
    #include <limits.h>
    #include <mach-o/dyld.h>
    #include <unistd.h>
#endif
#ifdef __linux__
    #include <limits.h>
    #include <libgen.h>
    #include <unistd.h>
    #if defined(__sun)
        #define PROC_SELF_EXE "/proc/self/path/a.out"
    #else
        #define PROC_SELF_EXE "/proc/self/exe"
    #endif
#endif
#if defined(_WIN32)
    std::string getExecutablePath() {
        char rawPathName[MAX_PATH];
        GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
        return std::string(rawPathName);
    }
    std::string getExecutableDir() {
        std::string executablePath = getExecutablePath();
        char* exePath = new char[executablePath.length()];
        strcpy_s(exePath, executablePath.c_str());
        PathRemoveFileSpecA(exePath);
        std::string directory = std::string(exePath);
        delete[] exePath;
        return directory;
    }
    std::string mergePaths(std::string pathA, std::string pathB) {
        char combined[MAX_PATH];
        PathCombineA(combined, pathA.c_str(), pathB.c_str());
        std::string mergedPath(combined);
        return mergedPath;
    }
#endif
#ifdef __linux__
    std::string getExecutablePath() {
        char rawPathName[PATH_MAX];
        auto result = realpath(PROC_SELF_EXE, rawPathName);
        return std::string(rawPathName);
    }
    std::string getExecutableDir() {
        std::string executablePath = getExecutablePath();
        char *executablePathStr = new char[executablePath.length() + 1];
        strcpy(executablePathStr, executablePath.c_str());
        std::string executableDir(dirname(executablePathStr));
        delete [] executablePathStr;
        return executableDir;
    }
    std::string mergePaths(std::string pathA, std::string pathB) {
        return pathA+"/"+pathB;
    }
#endif
#ifdef __APPLE__
    std::string getExecutablePath() {
        char rawPathName[PATH_MAX];
        char realPathName[PATH_MAX];
        uint32_t rawPathSize = (uint32_t)sizeof(rawPathName);
        if(!_NSGetExecutablePath(rawPathName, &rawPathSize)) {
            realpath(rawPathName, realPathName);
        }
        return std::string(realPathName);
    }
    std::string getExecutableDir() {
        std::string executablePath = getExecutablePath();
        char *executablePathStr = new char[executablePath.length() + 1];
        strcpy(executablePathStr, executablePath.c_str());
        char* executableDir = dirname(executablePathStr);
        delete [] executablePathStr;
        return std::string(executableDir);
    }
    std::string mergePaths(std::string pathA, std::string pathB) {
        return pathA+"/"+pathB;
    }
#endif
    bool checkIfFileExists (const std::string& filePath) {
        return access( filePath.c_str(), 0 ) == 0;
    }
}
namespace slim {
    void load_plugins(v8::Isolate* isolate);
    void run(const std::string file_name, const std::string file_contents);
    void stop(void);
    void start(int argc, char* argv[]) {
        if(argc == 2) {
            std::string file_contents;
            std::string file_name{argv[1]};
            std::ifstream file(file_name);
            getline(file, file_contents, '\0');
            file.close();
            if(file_contents.length() > 2) {
                slim::gv8::init(argc, argv);
                run(file_name, file_contents);
                stop();
            }
        }
    }
    void stop() {
        slim::gv8::stop();
    }
    void load_plugins(v8::Isolate* isolate) {
        DummyConsoleProvider dummy_console;
        dummy_console.expose_plugin(isolate);
        auto console_so_path = slim::path::getExecutableDir() + "/../lib/slim/console.so";
        void* plugin = dlopen(console_so_path.c_str(), RTLD_NOW);
        if(!plugin) {
            throw(std::string(dlerror()));
        }
        else {
            typedef void (*expose_plugin_t)(v8::Isolate* isolate);
            dlerror();
            //create_t* create_plugin = (create_t*) dlsym(plugin, "create");
            expose_plugin_t expose_plugin = (expose_plugin_t) dlsym(plugin, "expose_plugin");
            if(!expose_plugin) {
                dlclose(plugin);
                throw(std::string(dlerror()));
            }
            expose_plugin(isolate);
            //dlclose(plugin);
        }
    }
    void run(const std::string file_name, const std::string file_contents) {
        auto isolate = slim::gv8::GetIsolate();
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        slim::gv8::CreateGlobal();
        auto context = slim::gv8::GetNewContext();
        if(context.IsEmpty()) {
            throw("Error creating context");
        }
        v8::Context::Scope context_scope(context);
        slim::load_plugins(isolate);
        v8::TryCatch try_catch(isolate);
        auto script = slim::gv8::CompileScript(file_contents, file_name);
        if(try_catch.HasCaught()) {
            slim::gv8::ReportException(&try_catch);
        }
        if(!script.IsEmpty()) {
            bool result = slim::gv8::RunScript(script);
            if(try_catch.HasCaught()) {
                slim::gv8::ReportException(&try_catch);
            }
        }
        return;
    }
}
#endif