import { Hello } from "file:///home/greergan/product/slim/test/import/lib/hello.mjs";
import { World } from "file:///home/greergan/product/slim/test/import/lib/world.mjs";
export function HelloWorld() {
	console.log(Hello() + " " + World() + "!");
}