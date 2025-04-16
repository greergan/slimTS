//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L510
#include <iostream>
#include <unordered_map>
#include <vector>
#include <v8.h>
#include <librdkafka/rdkafkacpp.h>
#include <slim/plugin.hpp>
namespace slim::kafka {
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
	namespace common {
		v8::Local<v8::Object> configure(const v8::FunctionCallbackInfo<v8::Value>& args, RdKafka::Conf* configuration);
		void shutdown(const v8::FunctionCallbackInfo<v8::Value>& args);
	}
	namespace consumer {
		void configure(const v8::FunctionCallbackInfo<v8::Value>& args);
		void consume(const v8::FunctionCallbackInfo<v8::Value>& args);
		void get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
		namespace topic {
			void create(const v8::FunctionCallbackInfo<v8::Value>& args);
			void remove(const v8::FunctionCallbackInfo<v8::Value>& args);
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
};
void slim::kafka::producer::DeliveryReportCb::dr_cb(RdKafka::Message &message) {
	if(message.err()) {
		std::cerr << "Message delivery failed: " << message.errstr() << std::endl;				
	}
	else {
		std::cerr << "Message delivered to topic " << message.topic_name() << " [" << message.partition() << "] at offset " << message.offset() << std::endl;
	}
}
v8::Local<v8::Object> slim::kafka::common::configure(const v8::FunctionCallbackInfo<v8::Value>& args, RdKafka::Conf* configuration) {
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
	std::cout << __LINE__ << ": slim::kafka::common::configure" << std::endl;
	return v8_configuration_object;
}
/*
void slim::kafka::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	auto local_configuration = slim::utilities::GetObject(isolate, args[0]);
	std::cout << __LINE__ << ": slim::kafka::configure" << std::endl;
	if(slim::utilities::PropertyCount(isolate, local_configuration) == 0) {
		isolate->ThrowError("slim::kafka::configure requires a valid configuration object");
		return;
	}
	std::cout << __LINE__ << ": slim::kafka::configure" << std::endl;

	//consumer = RdKafka::Consumer::create(configuration, errstr);
	consumer = RdKafka::KafkaConsumer::create(configuration, errstr);
	if(!errstr.empty()) {
		std::cout << __LINE__ << ": slim::kafka::configure" << std::endl;
		std::cerr << __LINE__ << " " << errstr << std::endl;
		return;
	}
	producer = RdKafka::Producer::create(configuration, errstr);
	if(!errstr.empty()) {
		std::cout << __LINE__ << ": slim::kafka::configure" << std::endl;
		std::cerr << errstr << std::endl;
		return;
	}
	topic_configuration = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
	configuration->set("default_topic_conf", topic_configuration, errstr);
	if(!errstr.empty()) {
		std::cout << __LINE__ << ": slim::kafka::configure" << std::endl;
		std::cerr << errstr << std::endl;
		return;
	}
	std::cout << __LINE__ << ": slim::kafka::configure consumer->name=>" << consumer->name() << std::endl;
}*/
/*
void slim::kafka::consume(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
	if(topic_name.empty() || args.Length() == 0) {
		isolate->ThrowError("kafka.consume requires a string argument");
		return;
	}*/
/* 	std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
	RdKafka::ErrorCode result = consumer->start(topics[topic_name], 0, 0);
	std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
    if(result != RdKafka::ERR_NO_ERROR) {
		std::string errstr = "kafka.consume start error " + RdKafka::err2str(result);
		isolate->ThrowError(slim::utilities::StringToString(isolate, errstr));
		return;
    } */
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
	//RdKafka::Message* topic_message = consumer->consume(topics[topic_name], 0, 1000);
	//topics_list.at(0) = topic_name;
/*
	std::vector<std::string> topics_list;
	topics_list.push_back(topic_name);
	consumer->subscribe(topics_list);
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
	RdKafka::Message* message = consumer->consume(1000);
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
	consumer->close();
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
	if(message->err() != RdKafka::ERR_NO_ERROR && message->err() != RdKafka::ERR__TIMED_OUT) {
		std::string errstr = RdKafka::err2str(message->err());
		std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " ERROR=>" << errstr << std::endl;
		//isolate->ThrowError(slim::utilities::StringToV8String(isolate, errstr));
		isolate->ThrowException(slim::utilities::StringToV8String(isolate, errstr));
		std::cout << __LINE__ << ": slim::kafka::consume =>" << errstr << std::endl;
		return;
	}*/
	//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L216C7-L216C71
/* 	const RdKafka::Headers* topic_headers = topic_message->headers();
	if(topic_headers) {
		topic_headers->size();
		std::vector<RdKafka::Headers::Header> headers_vector = topic_headers->get_all();
	} */
/*
	auto context = isolate->GetCurrentContext();
	auto v8_topic_message = v8::Object::New(isolate);
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << message->len() << std::endl;
	if(!message->err()) {
		std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << message->len() << std::endl;
		auto no_warning = v8_topic_message->DefineOwnProperty(
			context,
			slim::utilities::StringToName(isolate, "len"),
			slim::utilities::size_t_ToV8Integer(isolate, message->len())
		);
		no_warning = v8_topic_message->DefineOwnProperty(
			context,
			slim::utilities::StringToName(isolate, "key"),
			slim::utilities::StringToV8Value(isolate, message->key())
		);
		no_warning = v8_topic_message->DefineOwnProperty(
			context,
			slim::utilities::StringToName(isolate, "payload"),
			slim::utilities::CharPointerToV8Value(isolate, (char *)message->payload())
		);
		delete message;
	}
	else {
		//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << message->err() << std::endl;
	}*/
/* 	result = consumer->stop(topics[topic_name], 0);
    if(result != RdKafka::ERR_NO_ERROR) {
		std::string errstr = "kafka.consume stop error " + RdKafka::err2str(result);
		isolate->ThrowError(slim::utilities::StringToString(isolate, errstr));
		return;
    } */
	//std::cout << __LINE__ << ": slim::kafka::consume =>" << topic_name << " " << std::endl;
/*
	args.GetReturnValue().Set(v8_topic_message);
}*/
/*
void slim::kafka::consumer_subscribe(const v8::FunctionCallbackInfo<v8::Value>& args) {
	std::cout << __LINE__ << ": slim::kafka::consumer_subscribe" << std::endl;
	v8::Isolate* isolate = args.GetIsolate();
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	topics_list.push_back(topic_name);
	std::cout << __LINE__ << ": slim::kafka::consumer_subscribe" << std::endl;
	consumer->close();
	std::cout << __LINE__ << ": slim::kafka::consumer_subscribe" << std::endl;
	consumer->subscribe(topics_list);
	std::cout << __LINE__ << ": slim::kafka::consumer_subscribe" << std::endl;
}*/
/*void slim::kafka::create_topic(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
	topics[topic_name] = RdKafka::Topic::create(producer, topic_name, topic_configuration, errstr);
	if(!topics[topic_name]) {
		std::cout << __LINE__ << ": slim:kafka::create_topic failed" << std::endl;
	}
    if(!errstr.empty()) {
		std::cout << __LINE__ << ": slim::kafka::create_topic=>" << errstr << std::endl;
		std::string errstr = "kafka.create.topic error " + errstr;
		isolate->ThrowError(slim::utilities::StringToString(isolate, errstr));
		return;
    }
	std::cout << __LINE__ << ": slim::kafka::create_topic=>" << topics[topic_name]->name() << std::endl;
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, topics[topic_name]->name()));
}*/
/*void slim::kafka::get_consumer_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, consumer->name()));
}*/
#define USE_CURRENT_TIME_STAMP 0
/*void slim::kafka::produce(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::cout << __LINE__ << ": slim::kafka::produce=>" << std::endl;
	if(args.Length() == 0) {
		isolate->ThrowError("kafka.produce requires a string argument and a valid reply object");
		return;
	}
	auto topic_name = slim::utilities::v8ValueToString(isolate, args[0]);
	if(topic_name.empty() || !args[1]->IsObject()) {
		isolate->ThrowError("kafka.produce requires a string argument and a valid reply object");
		return;
	}
	std::cout << __LINE__ << ": slim::kafka::produce=>" << topic_name << std::endl;
	auto packet = slim::utilities::GetObject(isolate, args[1]);
	auto key = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "key", packet));
	auto payload = slim::utilities::v8ValueToString(isolate, slim::utilities::GetValue(isolate, "payload", packet));
	auto result = producer->produce(topic_name, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY,
        				const_cast<char *>(payload.c_str()), payload.length(),
						const_cast<char *>(key.c_str()), key.length(),
						USE_CURRENT_TIME_STAMP ,NULL, NULL);
	std::cout << __LINE__ << ": slim::kafka::produce=>" << topic_name << std::endl;
	args.GetReturnValue().Set(slim::utilities::StringToV8Value(isolate, RdKafka::err2str(result)));
}*/

void slim::kafka::common::shutdown(const v8::FunctionCallbackInfo<v8::Value>& args) {
	std::cout << __LINE__ << ": slim::kafka::shutdown" << std::endl;
 	auto time_out = 5000;
	if(kafka_consumer)
		delete kafka_consumer;
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
	std::cout << __LINE__ << ": slim::kafka::shutdown" << std::endl;
}
void slim::kafka::consumer::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, kafka_consumer->name()));
	std::cout << __LINE__ << ": slim::kafka::consumer::configure" << std::endl;
}
void slim::kafka::producer::configure(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
	args.GetReturnValue().Set(slim::utilities::StringToV8String(isolate, kafka_producer->name()));
	std::cout << __LINE__ << ": slim::kafka::producer::configure" << std::endl;
}
void slim::kafka::consumer::consume(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::producer::produce(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::consumer::get_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::producer::get_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::consumer::topic::create(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::producer::topic::create(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::consumer::topic::remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
void slim::kafka::producer::topic::remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin kafka_plugin(isolate, "kafka");
 	slim::plugin::plugin kafka_consumer_plugin(isolate, "consumer");
	slim::plugin::plugin kafka_producer_plugin(isolate, "producer");
	slim::plugin::plugin kafka_consumer_topic_plugin(isolate, "topic");
	slim::plugin::plugin kafka_producer_topic_plugin(isolate, "topic");
	kafka_plugin.add_function("shutdown", slim::kafka::common::shutdown);
	kafka_consumer_plugin.add_function("configure", slim::kafka::consumer::configure);
	kafka_producer_plugin.add_function("configure", slim::kafka::producer::configure);
	kafka_consumer_plugin.add_function("consume", slim::kafka::consumer::consume);
	kafka_producer_plugin.add_function("produce", slim::kafka::producer::produce);
	kafka_consumer_plugin.add_function("get_name", slim::kafka::consumer::get_name);
	kafka_producer_plugin.add_function("get_name", slim::kafka::producer::get_name);
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