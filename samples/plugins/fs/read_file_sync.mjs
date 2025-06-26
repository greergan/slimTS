import console from 'console'
import fs from 'fs'
const temp_file = "temp_file.txt"
fs.writeFileSync(temp_file, "output to temp_file.txt\n")
console.log("read from written file =>", fs.readFileSync(temp_file))