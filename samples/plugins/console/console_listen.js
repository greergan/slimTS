console.clear();
console.listening = true;
console.outputWhenListening = true;
console.info("beginning console.listen() test");
console.debug("working toward async console listening");
(async () => {
	console.debug("found a solution for async console listening");
	for await(const message of console.listen()) {
		console.write(message);
	}
})();
console.info("ending console.listen() test");
