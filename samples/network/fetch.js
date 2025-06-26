slim.load("console");
(async() => {
	console.debug("start fetch");
	const v = await fetch("https://google.com");
	console.debug("end fetch");
	console.debug(v);
})();


/* fetch("https://victim.example/naïve-endpoint", {
  method: "POST",
  headers: [
    ["Content-Type", "application/json"],
    ["Content-Type", "text/plain"]
  ],
  credentials: "include",
  body: JSON.stringify(exerciseForTheReader)
}); */
