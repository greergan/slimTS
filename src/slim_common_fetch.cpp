#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <v8.h>
#include <slim/common/exception.h>
#include <slim/common/fetch.h>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
namespace slim::common::fetch {
    const std::regex url_pattern("((http|https)://)[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)");
}
std::stringstream slim::common::fetch::fetch(const char* file_name) {
    slim::common::log::trace(slim::common::log::Message("slim::common::fetch::fetch()","begins",__FILE__, __LINE__));
    std::string input_file_name(file_name);
    if(input_file_name.starts_with("file:///") || input_file_name.starts_with("file://")) {
		input_file_name = input_file_name.substr(7);
	}
    slim::common::log::trace(slim::common::log::Message("slim::common::fetch::fetch()",std::string("file_name => " + input_file_name).c_str(),__FILE__, __LINE__));
    std::ifstream input_file_stream(input_file_name, std::ios::in);
    std::stringstream file_contents_stringstream;
    if(input_file_stream.is_open()) {
        file_contents_stringstream << input_file_stream.rdbuf();
        input_file_stream.close();
    }
    else {
        slim::common::log::trace(slim::common::log::Message("slim::common::fetch::fetch()",(std::string("file access errno => ") + std::string(strerror(errno))).c_str(),__FILE__, __LINE__));
        std::string error_message(strerror(errno));
        error_message += " opening file";
        throw slim::common::SlimFileException("slim::common::fetch::fetch()", error_message.c_str(), file_name, errno);
    }
    slim::common::log::trace(slim::common::log::Message("slim::common::fetch::fetch()","ends",__FILE__, __LINE__));
    return file_contents_stringstream;
}
std::stringstream slim::common::fetch::fetch(const std::string* file_name) {
    return slim::common::fetch::fetch(file_name->c_str());
}
/* void slim::common::fetch::fetch(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Context::Scope context_scope(context);
    const std::regex url_pattern("((http|https)://)[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)");
    if(args.Length() == 0) {
        isolate->ThrowException(slim::utilities::StringToString(isolate, "fetch requires at least 1 argument"));
    }
    if(!args[0]->IsObject() && !args[0]->IsString()) {
        isolate->ThrowException(slim::utilities::StringToValue(isolate, "argument 1 of fetch must be a valid url string or a valid Request object"));
    }
    if(args[0]->IsString()) {
        std::string passed_url = slim::utilities::v8ValueToString(isolate, args[0]);
        if(!std::regex_match(passed_url, url_pattern)) {
            isolate->ThrowException(slim::utilities::StringToValue(isolate, "argument 1 of fetch must be a valid url string or a valid Request object"));
        }
    }
    else if(args[0]->IsObject()) {

    }

    auto resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
    auto result = resolver->Resolve(context, slim::utilities::StringToValue(isolate, "hello world"));
    args.GetReturnValue().Set(resolver->GetPromise());
} */