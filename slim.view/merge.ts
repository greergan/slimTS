import { SlimTypes } from './mod.ts'
import { debug, trace } from './mod.ts'
import { parse_statement, get_node_value } from './mod.ts'
export function merge(model:SlimTypes.KeyValueAny, compiled_view:string, compiler:any):string {
	let merged_view = compiled_view;
	merged_view = merged_view.replace(/\{#.+?#\}/gm, ($0):string => {
		let replacement_string = $0.replace(/{{|{%/gm, "{#");
		replacement_string = replacement_string.replace(/%}|}}/gm, "#}")
		return "<!-- " + replacement_string + " -->";
	});
/* 	merged_view = merged_view.replace(/\{%[^%]+?%\}/gm, (statement):string => {
		return parse_statement(model, statement, compiler);
	}); */
	merged_view = merged_view.replace(/\{\{([^}]+?)\}\}/gm, ($1, property_string):string => {
		return get_node_value(model, property_string);
	});
	return merged_view;
}