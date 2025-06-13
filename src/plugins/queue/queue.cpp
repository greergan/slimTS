#include <future>
#include <string>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
#include <slim/queue/queue.h>
namespace slim::plugin::queue {
	using namespace slim;
	using namespace slim::common;
	void listen(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void slim::plugin::queue::listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log::trace(log::Message("slim::plugin::queue::listen()", "begins",__FILE__, __LINE__));
	auto* isolate = args.GetIsolate();
	v8::HandleScope handle_scope(isolate);
	auto context = isolate->GetCurrentContext();
	auto maybe_resolver = v8::Promise::Resolver::New(context);
	if(maybe_resolver.IsEmpty()) {
		isolate->ThrowException(utilities::StringToV8String(isolate, "slim::plugin::queue::listen() failed to create promise"));
	}
	auto resolver = maybe_resolver.ToLocalChecked();
	args.GetReturnValue().Set(resolver->GetPromise());
	auto queue_name_string = utilities::v8ValueToString(isolate, args[0]);
	auto job_future = std::async(std::launch::async, slim::queue::get_job, queue_name_string);
	auto* job = job_future.get();
	auto v8_job_object = v8::Object::New(isolate);
	auto v8_ingress_file_store_object = v8::Object::New(isolate);
	auto v8_egress_file_store_object = v8::Object::New(isolate);
	v8_egress_file_store_object->Set(context, utilities::StringToV8String(isolate, "file_name_string"), utilities::StringToV8String(isolate, ""));
	v8_egress_file_store_object->Set(context, utilities::StringToV8String(isolate, "storage_container_handle"), utilities::StringToV8String(isolate, job->egress_job_file.storage_container_handle));
	v8_job_object->Set(context, utilities::StringToV8String(isolate, "egress_job_file"), v8_egress_file_store_object);
	v8_ingress_file_store_object->Set(context, utilities::StringToV8String(isolate, "file_name_string"), utilities::StringToV8String(isolate, job->ingress_job_file.file_name_string));
	v8_ingress_file_store_object->Set(context, utilities::StringToV8String(isolate, "storage_container_handle"), utilities::StringToV8String(isolate, job->ingress_job_file.storage_container_handle));
	v8_job_object->Set(context, utilities::StringToV8String(isolate, "ingress_job_file"), v8_ingress_file_store_object);
	v8_job_object->Set(context, utilities::StringToV8String(isolate, "errors"), v8::Array::New(isolate));
	//v8_job_object->Set(context, utilities::StringToV8String(isolate, "errored"), v8::Boolean::New(isolate, true));
	v8_job_object->Set(context, utilities::StringToV8String(isolate, "ticket_id"), v8::Integer::New(isolate, job->get_ticket_id()));
	resolver->Resolve(context, v8_job_object);
	log::debug(log::Message("slim::plugin::queue::listen()", "received a job",__FILE__, __LINE__));
	log::trace(log::Message("slim::plugin::queue::listen()", "ends",__FILE__, __LINE__));
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin queue_plugin(isolate, "queue");
	queue_plugin.add_function("listen", slim::plugin::queue::listen);
	queue_plugin.expose_plugin();
	return;
}
