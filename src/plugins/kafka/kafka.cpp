//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L510
#include <iostream>
#include <unordered_map>
#include <vector>
#include <v8.h>
#include <librdkafka/rdkafkacpp.h>
#include <slim/plugin.hpp>
namespace slim::kafka {
	std::string errstr;
	RdKafka::Conf* configuration;
	RdKafka::Conf* topic_configuration;
	RdKafka::Consumer* consumer;
	RdKafka::Producer* producer;
	std::unordered_map<std::string, RdKafka::Topic*> topics;
	void configure(const v8::FunctionCallbackInfo<v8::Value>& args);
	void consume(const v8::FunctionCallbackInfo<v8::Value>& args);
	void create_topic(const v8::FunctionCallbackInfo<v8::Value>& args);
	void delete_topic(const v8::FunctionCallbackInfo<v8::Value>& args);
	void produce(const v8::FunctionCallbackInfo<v8::Value>& args);
	void shutdown(const v8::FunctionCallbackInfo<v8::Value>& args);
	class DeliveryReportCb : public RdKafka::DeliveryReportCb {
		public:
			void dr_cb(RdKafka::Message &message);
	};
};
void slim::kafka::DeliveryReportCb::dr_cb(RdKafka::Message &message) {
	std::cerr << "message here\n";
	if(message.err()) {
		std::cerr << "Message delivery failed: " << message.errstr() << std::endl;				
	}
	else {
	std::cerr << "Message delivered to topic " << message.topic_name()
		<< " [" << message.partition() << "] at offset " << message.offset() << std::endl;
	}
}
void slim::kafka::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	auto local_configuration = slim::utilities::GetObject(isolate, args[0]);
	if(slim::utilities::PropertyCount(isolate, local_configuration) == 0) {
		isolate->ThrowError("slim::kafka::configure requires a valid configuration object");
		return;
	}
	DeliveryReportCb message_cb;
	configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	auto brokers = slim::utilities::v8ValueToString(isolate, 
		slim::utilities::GetValue(isolate, "bootstrap.servers", local_configuration));
	if(configuration->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
		if(!errstr.empty()) {
			std::cerr << errstr << std::endl;
			return;
		}
	}
	if(configuration->set("dr_cb", &message_cb, errstr) != RdKafka::Conf::CONF_OK) {
		if(!errstr.empty()) {
			std::cerr << errstr << std::endl;
			return;
		}
	}
	consumer = RdKafka::Consumer::create(configuration, errstr);
	if(!errstr.empty()) {
		std::cerr << errstr << std::endl;
		return;
	}
	producer = RdKafka::Producer::create(configuration, errstr);
	if(!errstr.empty()) {
		std::cerr << errstr << std::endl;
		return;
	}
	topic_configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
	configuration->set("default_topic_conf", topic_configuration, errstr);
	if(!errstr.empty()) {
		std::cerr << errstr << std::endl;
		return;
	}
}
void slim::kafka::consume(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	if(topic_name.empty() || args.Length() == 0) {
		isolate->ThrowError("kafka.consume requires a string argument");
		return;
	}
	RdKafka::ErrorCode result = consumer->start(topics[topic_name], 0, 0);
    if(result != RdKafka::ERR_NO_ERROR) {
		std::string error_string = "kafka.consume start error " + RdKafka::err2str(result);
		isolate->ThrowError(slim::utilities::StringToString(isolate, error_string));
		return;
    }
	RdKafka::Message* topic_message = consumer->consume(topics[topic_name], 0, 1000);
	if(topic_message->err() != RdKafka::ERR_NO_ERROR) {
		std::string error_string = "kafka.consume error " + RdKafka::err2str(topic_message->err());
		isolate->ThrowError(slim::utilities::StringToString(isolate, error_string));
		return;
	}
	//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L216C7-L216C71
/* 	const RdKafka::Headers* topic_headers = topic_message->headers();
	if(topic_headers) {
		topic_headers->size();
		std::vector<RdKafka::Headers::Header> headers_vector = topic_headers->get_all();
	} */
	auto context = isolate->GetCurrentContext();
	auto v8_topic_message = v8::Object::New(isolate);
	auto no_warning = v8_topic_message->DefineOwnProperty(
		context,
        slim::utilities::StringToName(isolate, "len"),
        slim::utilities::size_t_ToV8Integer(isolate, topic_message->len())
    );
	no_warning = v8_topic_message->DefineOwnProperty(
		context,
        slim::utilities::StringToName(isolate, "key"),
        slim::utilities::StringToV8Value(isolate, topic_message->key())
    );
	no_warning = v8_topic_message->DefineOwnProperty(
		context,
        slim::utilities::StringToName(isolate, "payload"),
        slim::utilities::CharPointerToV8Value(isolate, (char *)topic_message->payload())
    );
    delete topic_message;
	result = consumer->stop(topics[topic_name], 0);
    if(result != RdKafka::ERR_NO_ERROR) {
		std::string error_string = "kafka.consume stop error " + RdKafka::err2str(result);
		isolate->ThrowError(slim::utilities::StringToString(isolate, error_string));
		return;
    }
	args.GetReturnValue().Set(v8_topic_message);
}
void slim::kafka::create_topic(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if(args.Length() == 0) {
		isolate->ThrowError("kafka.create.topic requires a string argument and optional configuration object");
		return;
	}
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	if(topic_name.empty()) {
		isolate->ThrowError("kafka.create.topic requires a string argument and optional configuration object");
		return;
	}
	topics[topic_name] = RdKafka::Topic::create(consumer, topic_name, topic_configuration, errstr);
    if(!errstr.empty()) {
		std::string error_string = "kafka.create.topic error " + errstr;
		isolate->ThrowError(slim::utilities::StringToString(isolate, error_string));
		return;
    }
	args.GetReturnValue().Set(slim::utilities::StringToString(isolate, topics[topic_name]->name()));
}
void slim::kafka::delete_topic(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
#define USE_CURRENT_TIME_STAMP 0
void slim::kafka::produce(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if(args.Length() == 0) {
		isolate->ThrowError("kafka.create.topic requires a string argument and a valid reply object");
		return;
	}
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	if(topic_name.empty() || !args[1]->IsObject()) {
		isolate->ThrowError("kafka.create.topic requires a string argument and a valid reply object");
		return;
	}
	auto packet = slim::utilities::GetObject(isolate, args[1]);
	auto key = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "key", packet));
	auto payload = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "payload", packet));
	auto result = producer->produce(topic_name, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY,
        				const_cast<char *>(payload.c_str()), payload.length(),
						const_cast<char *>(key.c_str()), key.length(),
						USE_CURRENT_TIME_STAMP ,NULL, NULL);
	args.GetReturnValue().Set(slim::utilities::StringToV8Value(isolate, RdKafka::err2str(result)));
}
void slim::kafka::shutdown(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto time_out = 5000;
	delete consumer;
	delete producer;
	delete configuration;
	delete topic_configuration;
	RdKafka::wait_destroyed(time_out);
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin kafka_create_plugin(isolate, "create");
	kafka_create_plugin.add_function("topic", slim::kafka::create_topic);
	//slim::plugin::plugin kafka_delete_plugin(isolate, "delete");
	//kafka_create_plugin.add_function("topic", &slim::kafka::delete_topic);
	slim::plugin::plugin kafka_plugin(isolate, "kafka");
	kafka_plugin.add_function("configure", slim::kafka::configure);
	kafka_plugin.add_plugin("create", &kafka_create_plugin);
// 	kafka_plugin.add_plugin("delete", &kafka_delete_plugin);
	kafka_plugin.add_function("consume", &slim::kafka::consume);
	kafka_plugin.add_function("produce", &slim::kafka::produce);
	kafka_plugin.add_function("shutdown", slim::kafka::shutdown);
	kafka_plugin.expose_plugin();
	return;
}