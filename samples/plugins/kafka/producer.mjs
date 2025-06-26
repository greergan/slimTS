import { Message } from './lib/message.js'
import { configure, put_message, Roles} from './lib/functions.js'
slim.load("console");
console.debug("loading kafka");
try {
	slim.load("kafka");
	console.debug("loaded kafka");
}
catch(error) {
	console.log(error)
	exit
}

configure(Roles.PRODUCER)
//const request_topic_name = create_topic("raw_tcp_messages", Roles.PRODUCER)

try {
	console.debug("calling put_message()")
	let created = 0
	//for(let i = 0; i < 1; i++) {
	while(true) {
		const result = put_message("raw_tcp_messages")
		//console.log(result)
		created++
	}
	console.debug("created messages =>", created)
}
catch(error) {
	console.error("put_message error =>", error)
}



/*
const reply_packet = {
	"payload": "HTTP/1.1 200 OK\r\nContent-Length:20\r\nContent-Type: text/html\r\n\r\n<html>hello</html>\r\n",
	"key": first_message.key,
};
*/
