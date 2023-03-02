console.clear()
console.configuration.dir.text_color="blue"
console.dir("dir.text_color", console.configuration.dir.text_color)
console.configuration.log.text_color=50
console.configuration.log.bold = true
console.log("log.text_color", console.configuration.log.text_color)
console.configuration.log.location.text_color=30
console.log("testing stack trace log")
console.configuration.warn.location.text_color=30
console.configuration.warn.remainder.text_color=50
console.warn("testing stack trace warn")