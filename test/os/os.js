slim.load("console")
slim.load("os")
console.trace("inspecting os object after creation")
console.log(os)
os.platform = "new_platform"
console.trace("inspecting os object after attempted mutation of os.platform")
console.log(os)