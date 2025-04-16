import { Message } from './message.mjs'
const Roles = Object.freeze({
	CONSUMER: 0,
	PRODUCER: 1
})
let consumer_name
let producer_name

slim.load("console");
console.debug("loading kafka");
slim.load("kafka");
console.debug("loaded kafka");

const consumer_configuration = {
	"bootstrap.servers": "localhost:9092",
	"group.id": "development"
};
const producer_configuration = {
	"bootstrap.servers": "localhost:9092"
};
const topic_configuration = {};

function configure() {
	console.debug("configuring kafka consumer")
	consumer_name = kafka.consumer.configure(consumer_configuration)
	console.debug("configured kafka consumer=>", consumer_name)
	console.debug("configuring kafka producer");
	producer_name = kafka.producer.configure(producer_configuration)
	console.debug("configured kafka producer=>", producer_name)
}
function create_topic(topic_string, role=-1) {
	console.debug("creating consumer topic =>", topic_string)
	switch(role) {
		case Roles.CONSUMER:
			break;
		case Roles.PRODUCER:
			break;
		default:
			console.log("Unknown role...exiting")
			exit(1)
	}
	const consumer_topic_name = kafka.consumer.topic.create(topic_string, topic_configuration);
	console.debug("created a consumer topic named =>", consumer_topic_name);
}
function get_consumer_name() {
	return kafka.get_consumer_name()
}
function get_message(topic_name) {
	//console.debug("getting message from =>", topic_name);
	const message = kafka.consume(topic_name);
	//console.debug("message received from =>", topic_name, Math.random());
	//console.info(message)
	return message;
}
function put_message(topic_name, message) {
	console.debug("producing a message");
	const result = kafka.produce(topic_name, message);
	console.debug("message produced=>", result);
}
function shutdown() {
	console.debug("shutting down kafka")
	kafka.shutdown()
	console.debug("kafka is shutdown")
}
async function handle_message(message) {
	console.log(message)
}
configure()
shutdown()
exit
const request_topic_string = create_topic("raw_tcp_messages")
const response_topic_string = create_topic("response_messages")

while(request_message = get_message("request_topic_string")) {
	if(Object.keys(request_message).length > 0)
		handle_message(request_message)
}
console.log("done")
/*
const reply_packet = {
	"payload": "HTTP/1.1 200 OK\r\nContent-Length:20\r\nContent-Type: text/html\r\n\r\n<html>hello</html>\r\n",
	"key": first_message.key,
};
*/
slim kafka configure Isolate Local Object RdKafka Conf