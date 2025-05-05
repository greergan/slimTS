class Message {
	constructor(payload, key) {
		this.payload = payload + key
		this.key = key
	}
}
let number_of_messages_sent = 0;
const consumer_configuration = {
	"bootstrap.servers": "localhost:9092",
	"group.id": "development"
}
const producer_configuration = {
	"bootstrap.servers": "localhost:9092"
}
const topic_configuration = {}

export const Roles = Object.freeze({
	CONSUMER: 0,
	PRODUCER: 1
})

export function configure(role) {
	switch(role) {
		case Roles.CONSUMER:
			console.debug("configuring kafka consumer")
			let consumer_name = kafka.consumer.configure(consumer_configuration)
			console.debug("configured kafka consumer=>", consumer_name)
			break;
		case Roles.PRODUCER:
			console.debug("configuring kafka producer");
			let producer_name = kafka.producer.configure(producer_configuration)
			console.debug("configured kafka producer=>", producer_name)
			break;
		default:
			console.error("configure role is unknown")
	}
}

export function create_topic(topic_string, role=-1) {
	let created_topic_name;
	switch(role) {
		case Roles.CONSUMER:
			console.debug("creating consumer topic =>", topic_string)
			created_topic_name = kafka.consumer.topic.create(topic_string)
			break;
		case Roles.PRODUCER:
			console.debug("creating producer topic =>", topic_string)
			created_topic_name = kafka.producer.topic.create(topic_string)
			break;
		default:
			console.log("Unknown role...exiting")
	}
	console.debug("created topic =>", created_topic_name)
}

export function get_consumer_name() {
	return kafka.get_consumer_name()
}
export function get_message() {
	console.debug("getting message")
	let message
	try {
		message = kafka.consumer.consume(1000, 5)
		console.debug("message received")
	}
	catch(error) {
		message = JSON.parse(error)
		console.error(error)
		console.debug("message not received:", message)
		return message
	}
	return message;
}
export async function handle_message(request_message) {
	console.debug("handle_message =>", request_message)
}
export function list_subscriptions() {
	console.debug("getting consumer subscriptions")
	const subscriptions_array = kafka.consumer.topic.subscription()
	console.debug(subscriptions_array)
	console.debug("done getting consumer subscriptions")
}
export function put_message(topic_name) {
	console.debug("producing a message topic =>", topic_name)
	const new_message = new Message("payload string", ++number_of_messages_sent)
	console.debug(new_message)
	const result = kafka.producer.produce(topic_name, new_message)
	console.debug("message produced=>", result)
	return result
}
export function subscribe(topic_name) {
	console.debug("subscribing consumer to =>", topic_name)
	const result = kafka.consumer.topic.subscribe(topic_name)
	console.debug("consumer subscribed to =>", result)
}