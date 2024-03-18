"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.HelloWorld = void 0;
var hello_js_1 = require("file:///home/greergan/product/slim/test/import/lib/hello.js");
var world_js_1 = require("file:///home/greergan/product/slim/test/import/lib/world.js");
function HelloWorld() {
    console.log((0, hello_js_1.Hello)() + " " + (0, world_js_1.World)() + "!");
}
exports.HelloWorld = HelloWorld;
