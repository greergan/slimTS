slim.load("console");
console.debug("loading kafka");
slim.load("kafka");
console.debug("loaded kafka");
configuration = {
	"bootstrap.servers": "dev"
};
console.debug("kafka configuration created");
console.debug("configuring kafka");
kafka.configure(configuration);
console.debug("configured kafka");
topic_configuration = {
};
console.debug("topic configuration created");
console.debug("creating new topic");
const topic_name = kafka.create.topic("raw_tcp_messages", topic_configuration);
console.debug("created a topic named =>", topic_name);
console.debug("consuming for topic =>", topic_name);
const first_message = kafka.consume(topic_name);
console.debug(first_message);
console.debug("done consuming for topic =>", topic_name);
console.debug("creating new topic for replies");
const producer_topic_name = kafka.create.topic("formatted_tcp_messages_reply", topic_configuration);
console.debug("created a topic named =>", producer_topic_name);
const reply_packet = {
	"payload": "HTTP/1.1 200 OK\r\nContent-Length:20\r\nContent-Type: text/html\r\n\r\n<html>hello</html>\r\n",
	"key": first_message.key,
};
console.debug("reply_packet created");
console.debug("producing a message");
const result = kafka.produce(producer_topic_name, reply_packet);
console.debug("done producing message result =>", result);
function sleepFor(sleepDuration){
    var now = new Date().getTime();
    while(new Date().getTime() < now + sleepDuration){ 
        /* Do nothing */ 
    }
}
console.debug("sleeping before shutdown");
sleepFor(10000);
console.debug("shutting down kafka");
kafka.shutdown();
console.debug("kafka shut down");