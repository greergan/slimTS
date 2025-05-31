import { Hello } from "./hello.mjs";
import { World } from "./world.mjs";
export function HelloWorld() {
	return Hello() + " " + World()
}
export default HelloWorld