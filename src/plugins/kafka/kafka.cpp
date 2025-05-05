//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L510
#include <iostream>
#include <unordered_map>
#include <vector>
#include <v8.h>
#include <librdkafka/rdkafkacpp.h>
#include <slim/plugin.hpp>
namespace slim::kafka {
	static bool trace = false;
	static RdKafka::Conf* consumer_configuration;
	static RdKafka::Conf* producer_configuration;
	static RdKafka::Conf* consumer_topic_configuration;
	static RdKafka::Conf* producer_topic_configuration;
	static RdKafka::KafkaConsumer *kafka_consumer;
	static RdKafka::Producer *kafka_producer;
	static std::unordered_map<std::string, RdKafka::Topic*> consumer_topics;
	static std::unordered_map<std::string, RdKafka::Topic*> producer_topics;
	static std::vector<std::string> consumer_topics_list;
	static std::vector<std::string> producer_topics_list;
	enum ROLE {
		CONSUMER,
		PRODUCER
	};
	namespace common {
		v8::Local<v8::Object> configure(const v8::FunctionCallbackInfo<v8::Value>& args, RdKafka::Conf* configuration);
		void shutdown(const v8::FunctionCallbackInfo<v8::Value>& args);
		void trace(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
	namespace consumer {
		void configure(const v8::FunctionCallbackInfo<v8::Value>& args);
		void consume(const v8::FunctionCallbackInfo<v8::Value>& args);
		void get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
		namespace topic {
			void create(const v8::FunctionCallbackInfo<v8::Value>& args);
			void remove(const v8::FunctionCallbackInfo<v8::Value>& args);
			void subscribe(const v8::FunctionCallbackInfo<v8::Value>& args);
			void subscription(const v8::FunctionCallbackInfo<v8::Value>& args);
		}
	}
	namespace producer {
		void configure (const v8::FunctionCallbackInfo<v8::Value>& args);
		void produce(const v8::FunctionCallbackInfo<v8::Value>& args);
		void get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
		void shutdown(const v8::FunctionCallbackInfo<v8::Value>& args);
		namespace topic {
			void create(const v8::FunctionCallbackInfo<v8::Value>& args);
			void remove(const v8::FunctionCallbackInfo<v8::Value>& args);
		}
		class DeliveryReportCb : public RdKafka::DeliveryReportCb {
			public:
				void dr_cb(RdKafka::Message &message);
		};
	}
	namespace topic {
		void create(const v8::FunctionCallbackInfo<v8::Value>& args, ROLE role);
	}
};
void slim::kafka::consumer::topic::subscribe(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::common::subscribe" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	std::string topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	consumer_topics_list.push_back(topic_name);
	RdKafka::ErrorCode error_code = kafka_consumer->subscribe(consumer_topics_list);
	if(error_code != RdKafka::ERR_NO_ERROR) {
		std::cout << __LINE__ << ": slim::kafka::common::subscribe consumer->subscribe() = " << RdKafka::err2str(error_code) << std::endl;
	}
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, topic_name));
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::common::subscribe" << std::endl;
}
void slim::kafka::common::trace(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if(!args[0]->IsBoolean()) {
		isolate->ThrowException(slim::utilities::StringToV8String(isolate, "slim::kafka::common::trace expects boolean value"));
	}
	else {
		slim::kafka::trace = (args[0]->BooleanValue(isolate)) ? "true" : "false";
		std::cout << "slim::kafka::trace enabled" << std::endl;
	}
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::trace" << std::endl;
}
void slim::kafka::producer::DeliveryReportCb::dr_cb(RdKafka::Message &message) {
	if(message.err()) {
		std::cerr << "Message delivery failed: " << message.errstr() << std::endl;				
	}
	else {
		std::cerr << "Message delivered to topic " << message.topic_name() << " [" << message.partition() << "] at offset " << message.offset() << std::endl;
	}
}
v8::Local<v8::Object> slim::kafka::common::configure(const v8::FunctionCallbackInfo<v8::Value>& args, RdKafka::Conf* configuration) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::common::configure" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	std::string error_string;
	v8::Local<v8::Object> v8_configuration_object = slim::utilities::GetObject(isolate, args[0]);
/* 	if(slim::utilities::PropertyCount(isolate, local_configuration) == 0) {
		isolate->ThrowError("slim::kafka::common::configure requires a valid configuration object");
		return v8_configuration_object;
	} */
	std::string brokers = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "bootstrap.servers", v8_configuration_object));
	if(configuration->set("bootstrap.servers", brokers, error_string) != RdKafka::Conf::CONF_OK) {
		if(!error_string.empty()) {
			std::cout << __LINE__ << ": slim::kafka::common::configure" << std::endl;
			std::cerr << error_string << std::endl;
		}
	}
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::common::configure" << std::endl;
	return v8_configuration_object;
}
void slim::kafka::topic::create(const v8::FunctionCallbackInfo<v8::Value>& args, ROLE role) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::topic::create role=" << role << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	if(args.Length() == 0) {
		isolate->ThrowError("kafka.create.topic requires a string argument and optional configuration object");
		return;
	}
	std::string topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	if(topic_name.empty()) {
		isolate->ThrowError("kafka.create.topic requires a string argument and optional configuration object");
		return;
	}
	std::string error_string;
	std::string created_topic_name;
	switch(role) {
		case CONSUMER:
			consumer_topics[topic_name] = RdKafka::Topic::create(kafka_consumer, topic_name, consumer_topic_configuration, error_string);
			if(slim::kafka::trace)
				std::cout << __LINE__ << ": slim::kafka::topic::create " << topic_name << " => " << error_string << std::endl;
			if(!consumer_topics[topic_name]) {
				std::cout << __LINE__ << ": slim::kafka::topic::create failed" << std::endl;
			}
			else {
				consumer_topics_list.push_back(topic_name);
				RdKafka::ErrorCode error_code = kafka_consumer->subscribe(consumer_topics_list);
				if(error_code != RdKafka::ERR_NO_ERROR) {
					std::cout << __LINE__ << ": slim::kafka::topic::create consumer->subscribe() = " << RdKafka::err2str(error_code) << std::endl;
				}
				else {
					created_topic_name = consumer_topics[topic_name]->name();
				}
			}
			if(slim::kafka::trace)
				std::cout << __LINE__ << ": slim::kafka::topic::create role=" << role << " topic=" << topic_name << std::endl;
			break;
		case PRODUCER:
			producer_topics[topic_name] = RdKafka::Topic::create(kafka_producer, topic_name, producer_topic_configuration, error_string);
			if(!producer_topics[topic_name]) {
				std::cout << __LINE__ << ": slim::kafka::topic::create failed" << std::endl;
			}
			else {
				created_topic_name = producer_topics[topic_name]->name();
			}
			if(slim::kafka::trace)
				std::cout << __LINE__ << ": slim::kafka::topic::create role=" << role << std::endl;
			break;
		default:
			isolate->ThrowError("slim::kafka::topic::create unknown role");
	}
	if(!error_string.empty()) {
		std::cout << __LINE__ << ": slim::kafka::topic::create =>" << error_string << std::endl;
		error_string = "slim::kafka::topic::create error " + error_string;
		isolate->ThrowError(slim::utilities::StringToString(isolate, error_string));
		return;
	}
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::topic::create role=" << role << " topic=" << created_topic_name << std::endl;
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, created_topic_name));
}

#define USE_CURRENT_TIME_STAMP 0
void slim::kafka::common::shutdown(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::shutdown" << std::endl;
 	auto time_out = 5000;
	if(kafka_consumer) {
		kafka_consumer->close();
		delete kafka_consumer;
	}
	if(kafka_producer)
		delete kafka_producer;
	if(consumer_configuration)
		delete consumer_configuration;
	if(producer_configuration)
		delete producer_configuration;
	if(consumer_topic_configuration)
		delete consumer_topic_configuration;
	if(producer_topic_configuration)
		delete producer_topic_configuration;
	RdKafka::wait_destroyed(time_out);
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::shutdown" << std::endl;
}
void slim::kafka::consumer::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::configure" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	consumer_configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	v8::Local<v8::Object> v8_configuration_object = slim::kafka::common::configure(args, slim::kafka::consumer_configuration);
	std::string group_id = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "group.id", v8_configuration_object));
	std::string error_string;
	if(consumer_configuration->set("group.id", group_id, error_string) != RdKafka::Conf::CONF_OK) {
		if(!error_string.empty()) {
			std::cout << __LINE__ << ": slim::kafka::consumer::configure" << std::endl;
			std::cerr << error_string << std::endl;
			return;
		}
	}
	kafka_consumer = RdKafka::KafkaConsumer::create(consumer_configuration, error_string);
	if(!error_string.empty()) {
		std::cout << __LINE__ << ": slim::kafka::consumer::configure" << std::endl;
		std::cerr << __LINE__ << " " << error_string << std::endl;
		return;
	}
	consumer_topic_configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
	consumer_configuration->set("default_topic_conf", consumer_topic_configuration, error_string);
	if(!error_string.empty()) {
		std::cout << __LINE__ << ": slim::kafka::consumer::configure" << std::endl;
		std::cerr << error_string << std::endl;
		return;
	}
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, kafka_consumer->name()));
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::configure" << std::endl;
}
void slim::kafka::producer::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::producer::configure" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	producer_configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	v8::Local<v8::Object> v8_configuration_object = slim::kafka::common::configure(args, slim::kafka::producer_configuration);
	DeliveryReportCb message_delivery_call_back;
	std::string error_string;
	kafka_producer = RdKafka::Producer::create(producer_configuration, error_string);
	if(producer_configuration->set("dr_cb", &message_delivery_call_back, error_string) != RdKafka::Conf::CONF_OK) {
		if(!error_string.empty()) {
			std::cout << __LINE__ << ": slim::kafka::producer::configure" << std::endl;
			std::cerr << error_string << std::endl;
			return;
		}
	}
	producer_topic_configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
	producer_configuration->set("default_topic_conf", producer_topic_configuration, error_string);
	if(!error_string.empty()) {
		std::cout << __LINE__ << ": slim::kafka::producer::configure" << std::endl;
		std::cerr << error_string << std::endl;
		return;
	}
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, kafka_producer->name()));
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::producer::configure" << std::endl;
}
void slim::kafka::consumer::consume(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::consume" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	int timeout = 500;
	int retries_max = 1;
	int retries = 0;
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::consume timeout =>" << timeout << ", retries_max => " << retries_max << std::endl;
	if(args.Length() > 0 && !args[0]->IsUndefined()) {
		timeout = slim::utilities::V8ValueToInt(isolate, args[0]);
		if(args.Length() > 1 && !args[1]->IsUndefined()) {
			retries_max = slim::utilities::V8ValueToInt(isolate, args[1]);
		}
		if(slim::kafka::trace)
			std::cout << __LINE__ << ": slim::kafka::consumer::consume timeout =>" << timeout << ", retries_max => " << retries_max << std::endl;
	}
	do_consume:
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::consume before consume call" << std::endl;
	RdKafka::Message* kafka_message = kafka_consumer->consume(timeout);
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::consume after consume call" << std::endl;
	if(kafka_message->err() != RdKafka::ERR_NO_ERROR) {
/* 		if(retries <= retries_max) {
			retries++;
			std::cout << "attempting retry" << std::endl;
			goto do_consume;
		} */
		goto do_consume;
		if(slim::kafka::trace)
			std::cout << __LINE__ << ": slim::kafka::consumer::consume after consume call" << std::endl;
		v8::Local<v8::Object> exception_string_object = v8::Object::New(isolate);
		exception_string_object->Set(context, slim::utilities::StringToV8String(isolate, "code"), slim::utilities::IntToV8Integer(isolate, kafka_message->err())).Check();
		exception_string_object->Set(context, slim::utilities::StringToV8String(isolate, "message"), slim::utilities::StringToV8String(isolate, kafka_message->errstr())).Check();
		v8::Local<v8::String> v8_exception_string = v8::JSON::Stringify(context, exception_string_object).ToLocalChecked();
		isolate->ThrowException(v8_exception_string);
		if(slim::kafka::trace)
			std::cout << __LINE__ << ": slim::kafka::consumer::consume =>" << " ERROR =>" << slim::utilities::v8StringToString(isolate, v8_exception_string) << std::endl;
		return;
	}
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::consume after consume call" << std::endl;
	//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L216C7-L216C71
/* 	const RdKafka::Headers* topic_headers = topic_message->headers();
	if(topic_headers) {
		topic_headers->size();
		std::vector<RdKafka::Headers::Header> headers_vector = topic_headers->get_all();
	} */
	
	v8::Local<v8::Object> v8_topic_message = v8::Object::New(isolate);
	if(!kafka_message->err()) {
		if(slim::kafka::trace)
			std::cout << __LINE__ << ": slim::kafka::consume =>" << " " << kafka_message->len() << std::endl;
		auto no_warning = v8_topic_message->DefineOwnProperty(
			context,
			slim::utilities::StringToName(isolate, "len"),
			slim::utilities::size_t_ToV8Integer(isolate, kafka_message->len())
		);
		no_warning = v8_topic_message->DefineOwnProperty(
			context,
			slim::utilities::StringToName(isolate, "key"),
			slim::utilities::StringToV8Value(isolate, kafka_message->key())
		);
		no_warning = v8_topic_message->DefineOwnProperty(
			context,
			slim::utilities::StringToName(isolate, "payload"),
			slim::utilities::CharPointerToV8Value(isolate, (char *)kafka_message->payload())
		);
		delete kafka_message;
	}
	else {
		std::cout << __LINE__ << ": slim::kafka::consumer::consume =>" << " ERROR => " << RdKafka::err2str(kafka_message->err())  << std::endl;
	}
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::consume" << std::endl;
	args.GetReturnValue().Set(v8_topic_message);
}
void slim::kafka::producer::produce(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::producer::produce" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	
	if(args.Length() == 0) {
		isolate->ThrowError("kafka.produce requires a string argument and a valid reply object");
		return;
	}
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::producer::produce topic => " << topic_name << std::endl;
	if(topic_name.empty() || !args[1]->IsObject()) {
		isolate->ThrowError("kafka.produce requires a string argument and a valid reply object");
		return;
	}
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::producer::produce" << std::endl;
	auto packet = slim::utilities::GetObject(isolate, args[1]);
	auto key = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "key", packet));
	auto payload = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "payload", packet));
	auto result = kafka_producer->produce(topic_name, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY,
        				const_cast<char *>(payload.c_str()), payload.length(),
						const_cast<char *>(key.c_str()), key.length(),
						USE_CURRENT_TIME_STAMP ,NULL, NULL);
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::producer::produce" << std::endl;
	args.GetReturnValue().Set(slim::utilities::StringToV8Value(isolate, RdKafka::err2str(result)));
}
void slim::kafka::consumer::get_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, kafka_consumer->name()));
}
void slim::kafka::producer::get_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, kafka_producer->name()));
}
void slim::kafka::consumer::topic::create(const v8::FunctionCallbackInfo<v8::Value>& args) {
	ROLE role = CONSUMER;
	slim::kafka::topic::create(args, role);
}
void slim::kafka::consumer::topic::subscription(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::subscription" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	std::vector<std::string> topics;
	RdKafka::ErrorCode error_code = kafka_consumer->subscription(topics);
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::subscription => " << RdKafka::err2str(error_code) << std::endl;
	v8::Local<v8::Array> subscriptions_array = v8::Array::New(isolate);
	int subscriptions_array_index = 0;
	for(const std::string& topic_name : topics) {
		if(slim::kafka::trace)
			std::cout << __LINE__ << ": slim::kafka::consumer::subscription => " << topic_name << std::endl;
		subscriptions_array->Set(context, subscriptions_array_index, slim::utilities::StringToV8String(isolate, topic_name)).Check();
		subscriptions_array_index++;
	}
	args.GetReturnValue().Set(subscriptions_array);
	if(slim::kafka::trace)
		std::cout << __LINE__ << ": slim::kafka::consumer::subscription" << std::endl;
}
void slim::kafka::producer::topic::create(const v8::FunctionCallbackInfo<v8::Value>& args) {
	ROLE role = PRODUCER;
	slim::kafka::topic::create(args, role);
}
void slim::kafka::consumer::topic::remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::producer::topic::remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin kafka_plugin(isolate, "kafka");
	kafka_plugin.add_function("shutdown", slim::kafka::common::shutdown);
	kafka_plugin.add_function("trace", slim::kafka::common::trace);
 	slim::plugin::plugin kafka_consumer_plugin(isolate, "consumer");
	slim::plugin::plugin kafka_producer_plugin(isolate, "producer");
	slim::plugin::plugin kafka_consumer_topic_plugin(isolate, "topic");
	slim::plugin::plugin kafka_producer_topic_plugin(isolate, "topic");
	kafka_consumer_plugin.add_function("configure", slim::kafka::consumer::configure);
	kafka_producer_plugin.add_function("configure", slim::kafka::producer::configure);
	kafka_consumer_plugin.add_function("consume", slim::kafka::consumer::consume);
	kafka_producer_plugin.add_function("produce", slim::kafka::producer::produce);
	kafka_consumer_plugin.add_function("get_name", slim::kafka::consumer::get_name);
	kafka_producer_plugin.add_function("get_name", slim::kafka::producer::get_name);
	kafka_consumer_topic_plugin.add_function("subscribe", slim::kafka::consumer::topic::subscribe);
	kafka_consumer_topic_plugin.add_function("subscription", slim::kafka::consumer::topic::subscription);
	kafka_consumer_topic_plugin.add_function("create", slim::kafka::consumer::topic::create);
	kafka_consumer_topic_plugin.add_function("delete", slim::kafka::consumer::topic::remove);
	kafka_producer_topic_plugin.add_function("create", slim::kafka::producer::topic::create);
	kafka_producer_topic_plugin.add_function("delete", slim::kafka::producer::topic::remove);
	kafka_consumer_plugin.add_plugin("topic", &kafka_consumer_topic_plugin);
	kafka_producer_plugin.add_plugin("topic", &kafka_producer_topic_plugin);
	kafka_plugin.add_plugin("consumer", &kafka_consumer_plugin);
	kafka_plugin.add_plugin("producer", &kafka_producer_plugin);
	kafka_plugin.expose_plugin();
	return;
}