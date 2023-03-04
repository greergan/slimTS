import { SlimTypes, todo, debug, error  } from './dep.ts'
export async function get_file(url:string): Promise<string> {
	/* was hoping to use the "file:///" prefix with deno
		* it is not working as of version 1.19.3
		* it throws a network error that I have not looked into further	*/
	if(url.match(/^file:\/{3}/)) {
		new todo({message:'re-evaluate fetch(file:/// handling)'})
		try {
			return await Deno.readTextFile(url.substring(8));
		}
		catch(e) {
			new error({message:"getting file"}, e);
			new error({message:"getting file"}, url)
			new error({message:"current working directory"}, new URL('.', import.meta.url).pathname)
		}
	}
	else if(url.match(/^https|http:{2}/)) {
		return await (await fetch(url)).text();
	}
	else {
		new error({message:"getting file not supported", value:url});
	}
	return "";
}
export async function get_node_value(model:SlimTypes.KeyValueAny, property:string): Promise<string|SlimTypes.KeyValueAny> {
	const node_array:Array<any> = property.trim().split('.');
	let node_value:any = "";
	let next_property_string:string = "";
	for(let node of node_array) {
		next_property_string = property.substring(property.indexOf('.') + 1);
		node_value = model[`${node}`];
 		if(typeof node_value === 'object') {
			if(next_property_string.length == 0) {
				break;
			}
			else {
				node_value = get_node_value(model[`${node}`], next_property_string);
			}
		}
		if(property.endsWith(next_property_string)) {
			break;
		}
	}
	return node_value || "";
}
export async function merge_json(...sources:Array<SlimTypes.KeyValueAny>): Promise<SlimTypes.KeyValueAny> {
	const merged_objects:SlimTypes.KeyValueAny = {};
	for(const source of sources) {
		for(const key in source) {
			if(typeof source[key] == 'string' || typeof source[key] == 'number' || Array.isArray(source[key])) {
				merged_objects[key] = source[key];
			}
			else if(typeof source[key] == 'object') {
				merged_objects[key] = await merge_json(merged_objects[key], source[key]);
			}
		}
	}
	return merged_objects;
}
export async function normalize_url(url:string, file_type:'html'|'json'|'png'|'jpg'='html'): Promise<string> {
	let new_url:string = url.trim();
	if(url.match(/^https|http:{2}/) && url.match(/^file:{3}/)) {
		url = `file:///${new_url}`;
	}
	if(new_url.endsWith('/')) {
		new_url = `index.${file_type}`;
	}
	else if(!new_url.endsWith(file_type)) {
		new_url = `${new_url}.${file_type}`;
	}
	return new_url;
}