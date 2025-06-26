import console from 'console'
import fs from 'fs'
console.log("attempting to write temp_file.txt")
fs.writeFileSync("temp_file.txt", "output to temp_file.txt\n")