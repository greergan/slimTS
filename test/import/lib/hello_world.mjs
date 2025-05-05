import { Hello } from "lib/hello.mjs";
import { World } from "lib/world.mjs";
export function HelloWorld() {
	console.log(Hello() + " " + World() + " while using imports!");
}