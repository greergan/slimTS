import { configure, create_topic, get_message, handle_message, list_subscriptions, subscribe, Roles } from "./lib/functions.js"

let consumer_name
let producer_name

slim.load("console")
try {
	console.debug("loading kafka")
	slim.load("kafka")
	console.debug("loaded kafka")
}
catch(error) {
	console.debug("kafka not loaded:", error)
	exit()
}

configure(Roles.CONSUMER)
const subscribed_topic = subscribe("raw_tcp_messages")
list_subscriptions()
//const request_topic_name = create_topic("raw_tcp_messages", Roles.CONSUMER)

try {
	kafka.trace(true)
	while(request_message = get_message(subscribed_topic)) {
		console.info(request_message)
		//handle_message(request_message)
	}
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
