import console from 'console'
import {readdirSync} from 'fs'
console.log(readdirSync("."))
console.log(readdirSync(".."))
console.log(readdirSync("../.."))
