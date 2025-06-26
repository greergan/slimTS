import { Hello } from "file:///home/greergan/product/slim/test/import/lib/hello.js";
import { World } from "file:///home/greergan/product/slim/test/import/lib/world.js";
export function HelloWorld() {
	console.log(Hello() + " " + World() + "!");
}