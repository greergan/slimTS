#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <regex>
#include <slim/common/exception.h>
#include <slim/common/fetch.h>
#include <slim/common/log.h>
namespace slim::common::fetch {
    using namespace slim;
    using namespace slim::common;
    const std::regex url_pattern("((http|https)://)[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)");
}
std::unique_ptr<std::string> slim::common::fetch::string(std::string& file_name_string) {
    log::trace(log::Message("slim::common::fetch::string()",std::string("begins, file name => " + file_name_string).c_str(),__FILE__, __LINE__));
    auto a = stream(file_name_string)->get();
    auto return_string = std::make_unique<std::string>("");
    log::trace(log::Message("slim::common::fetch::string()",std::string("ends, file name => " + file_name_string).c_str(),__FILE__, __LINE__));
    return return_string;
}
std::unique_ptr<std::stringstream> slim::common::fetch::stream(std::string& file_name_string) {
    log::trace(log::Message("slim::common::fetch::stream()",std::string("begins" + file_name_string).c_str(),__FILE__, __LINE__));
    std::string input_file_name = file_name_string;
    if(input_file_name.starts_with("file:///") || input_file_name.starts_with("file://")) {
		input_file_name = input_file_name.substr(7);
	}
    log::debug(log::Message("slim::common::fetch::stream()",std::string("file_name => " + input_file_name).c_str(),__FILE__, __LINE__));
    std::ifstream input_file_stream(input_file_name, std::ios::in);
    auto file_contents_stringstream = std::make_unique<std::stringstream>();
    if(input_file_stream.is_open()) {
        *file_contents_stringstream << input_file_stream.rdbuf();
        input_file_stream.close();
    }
    else {
        log::error(log::Message("slim::common::fetch::stream()",(std::string("file access errno => ") + std::string(strerror(errno))).c_str(),__FILE__, __LINE__));
        std::string error_message(strerror(errno));
        error_message += " opening file";
        throw slim::common::SlimFileException("slim::common::fetch::stream()", error_message.c_str(), file_name_string.c_str(), errno);
    }
    log::trace(log::Message("slim::common::fetch::stream()",std::string("ends" + file_name_string).c_str(),__FILE__, __LINE__));
    return file_contents_stringstream;
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