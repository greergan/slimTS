console.clear()
console.configuration.log.text_color = "red"
console.log("\ttext color: %s, background_color: %s",
    console.configuration.log.text_color,
    console.configuration.log.background_color
);
console.configuration.log.text_color = 222
console.log("\tint %i, int %d, float %f",1,200,32.5);
console.configuration.log.text_color = 22
console.log("\toptimally useful formatting %o", {})
console.configuration.log.text_color = 13
console.log("\tgeneric JavaScript object formatting%O", {"object": "conversion"})