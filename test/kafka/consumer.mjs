import { configure, create_topic, get_message, handle_message, list_subscriptions, subscribe, Roles } from "./lib/functions.js"

let consumer_name
let producer_name

slim.load("console")
console.debug("loading kafka")
slim.load("kafka")
console.debug("loaded kafka")


configure(Roles.CONSUMER)
const subscribed_topic = subscribe("raw_tcp_messages")
list_subscriptions()
//const request_topic_name = create_topic("raw_tcp_messages", Roles.CONSUMER)

try {
	//kafka.trace(true)
	while(handle_message(get_message(subscribed_topic))) {}
}
catch(error) {
	console.error(error)
}


/*
const reply_packet = {
	"payload": "HTTP/1.1 200 OK\r\nContent-Length:20\r\nContent-Type: text/html\r\n\r\n<html>hello</html>\r\n",
	"key": first_message.key,
};
*/
