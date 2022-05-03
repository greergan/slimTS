console.clear()
console.configuration.log.text_color=200
console.configuration.log.dim = true;
console.log("testing dim", console.configuration.log.dim)
console.configuration.log.dim = false;
console.log("testing dim", console.configuration.log.dim)

console.configuration.log.text_color=100
console.configuration.log.bold = true;
console.log("testing bold", console.configuration.log.bold)
console.configuration.log.bold = false;
console.log("testing bold", console.configuration.log.bold)

console.configuration.log.text_color=50
console.configuration.log.italic = true;
console.log("testing italic", console.configuration.log.italic)
console.configuration.log.italic = false;
console.log("testing italic", console.configuration.log.italic)

console.configuration.log.text_color=75
console.configuration.log.underline = true;
console.log("testing underline", console.configuration.log.underline)
console.configuration.log.underline = false;
console.log("testing underline", console.configuration.log.underline)

console.configuration.log.text_color=150
console.configuration.log.inverse = true;
console.log("testing inverse", console.configuration.log.inverse)
console.configuration.log.inverse = false;
console.log("testing inverse", console.configuration.log.inverse)
