import { SlimTypes, SlimViewHandler } from './mod.ts';
import { debug, todo, trace } from './mod.ts'
import { do_handler, parse_handlers, merge, normalize_url } from './mod.ts';
export function parse_statement(model:SlimTypes.KeyValueAny, statement:string, compiler:any):string {
	let statement_string:string = statement.replace(/{|%|}/g, '').trim();
	new trace({message:"entry value", value: "/" + statement_string +"/"})
	let merged_view:string = "";
 	const verb_match:Array<any> = statement_string.match(/^include|for\s/i) || [];
	const verb:string = String(verb_match[0]).toLowerCase();
	new trace({message:"parsing", value:verb});
	switch(verb) {
		case 'for':
			const for_match:Array<string> = statement_string.match(/^for\s+["]\s*([a-z0-9-]+)(.*)["](.+)/i) || [];
 			if(for_match.length == 4) {
 				let handler_array:Array<SlimViewHandler> = [];
				let node:Array<SlimTypes.KeyValueAny> = model[for_match[1]];
				if(for_match[2]) {
					handler_array = parse_handlers(for_match[2].trim());
					//new debug({message:"value for handler_array"}, handler_array);
					if(handler_array[0] != undefined) {
						node = do_handler(node, handler_array[0]);
					}
				}
 				for(const member of node) {
					merged_view += merge(member, for_match[3], compiler);
				}
			}
			break;
		case 'include':
			const include_match:Array<string> = statement_string.match(/\s+"\s*(.+?)"\s*"(.+?)"/) || [];
new debug({message:"include_match",value:include_match});
   			if(include_match.length == 3) {
				let file_url = (include_match[1].match('http|https|file:\/\/\/')) ? include_match[1] : compiler.name_space_url + include_match[1];
				file_url = normalize_url(file_url);
				new debug(include_match[2])
				new debug({message:"node value", value: "/" + include_match[2] +"/"})

/* 				
				const with_view:string = compiler.compile(file_url);
				const merge_model:Array<SlimTypes.KeyValueAny> = model[`${include_match[3]}`];
				merge_model.forEach((member, index) => member.index = index);
 				for(const member of merge_model) {
					merged_view += merge(member, with_view, compiler);
				} */
			}
			break;
	}
	new trace({message:"exit value", value:merged_view})
	return "merged_view";
}