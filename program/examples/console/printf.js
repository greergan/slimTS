//console.clear()
console.configuration.log.text_color = "red"
console.log("text color: %s, background_color: %s",
    console.configuration.log.text_color,
    console.configuration.log.background_color
);
console.log("int %i","string");
console.configuration.log.text_color = 222
console.log("int %i, float %f, int %d",1,32.5,200);
console.configuration.log.text_color = 22
console.log("optimally useful formatting %o", {})
console.configuration.log.text_color = 13
console.log("generic JavaScript object formatting %O", {"object": "conversion"})