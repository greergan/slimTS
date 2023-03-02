import { trace, error, debug, todo, SlimTypes } from './mod.ts';
export class SlimViewHandler {
	handler:string;
	key:string;
	match:string;
	left?:SlimViewHandler;
	right?:SlimViewHandler;
	current_node:'right'|'left'|'main' = 'main';
	node_index:Array<number> = [];
	constructor(handler?:string, key?:string, match?:string) {
		this.handler = handler || "";
		this.key = key || "";
		this.match = match || "";
	}
	is_complex():boolean {
		return (this.handler.length > 0 && this.left != undefined && this.right != undefined) ? true : false;
	}
	is_valid():boolean {
		return (this.handler.length > 0 && this.key != undefined && this.match != undefined) ? true : false;
	}
}
export function do_handler(model:SlimTypes.KeyValueAny, handler:SlimViewHandler):Array<SlimTypes.KeyValueAny> {
	let node:Array<SlimTypes.KeyValueAny> = [];
	if(handler.is_valid()) {
		switch(handler.handler.toLowerCase()) {
			case 'equal':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] == handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'greater_than':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] > handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'greater_than_equal':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] >= handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'less_than':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] < handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'less_than_equal':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] <= handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'not_equal':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] != handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'not_greater_than':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] !> handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'not_less_than':
				node = model.filter((member:SlimTypes.KeyValueAny, index:number) => {
					if(member[`${handler.key}`] !< handler.match) {
						handler.node_index.push(index);
						return member;
					}
				});
				break;
			case 'and':
				(handler.left != undefined) ? do_handler(model, handler.left) : [];
				(handler.right != undefined) ? do_handler(model, handler.right) : [];
				if(handler.left?.node_index != undefined && handler.right?.node_index != undefined) {
					handler.left.node_index = handler.left?.node_index.sort();
					handler.right.node_index = handler.right?.node_index.sort();
new debug({message:"valueof"}, handler.left?.node_index.valueOf())
					if(handler.left?.node_index == handler.right?.node_index) {
						for(let index of handler.left?.node_index) {
							node.push(model[index]);
						}
new debug({message:"left and right arrays match"})
					}
				}
				else {
					new error({message: "one or both handlers are undefined"}, handler.left, handler.right)
				}
			case 'or':
				(handler.left != undefined) ? do_handler(model, handler.left) : [];
				(handler.right != undefined) ? do_handler(model, handler.right) : [];
				if(handler.left?.node_index != undefined && handler.right?.node_index != undefined) {
					handler.node_index = [...new Set([...handler.left?.node_index,...handler.right?.node_index])].sort();
					for(let index of handler.node_index) {
						node.push(model[index]);
					}
				}
				else {
					new error({message: "one or both handlers are undefined"}, handler.left, handler.right)
				}
				break;
			default:
				new error({message: "unknown handler", value:handler.handler}, handler)
				break;
		}
	}
	else {
		new error({message:"invalid handler", value:handler});
	}
//new debug({message:'exit value', value:node}, handler);
	return node;
}
export function get_handler(handler_string:string):SlimViewHandler {
new debug({message:"entry"}, handler_string);
	const handler:SlimViewHandler = new SlimViewHandler();
	const complex_match:Array<string> = handler_string.match(/^(and|or)\((.+)\)/i) || [];
	if(complex_match.length == 3) {
		//new debug({message:"complex array"}, complex_match)
		handler.handler = complex_match[1];
		const left_right_match = complex_match[2].match(/[a-z_]+\([a-z0-9- ]+,[a-z0-9'" ]+\)/gi) || [];
new debug({message: "left_right_match"}, left_right_match);
		if(left_right_match.length == 2) {
			handler.left = get_handler(left_right_match[0])
			handler.left.current_node = 'left';
			handler.right = get_handler(left_right_match[1]);
			handler.right.current_node = 'right';
		}
	}
	else{
		let handler_array:Array<string> = handler_string.match(/([a-z]+)\(\s*([a-z0-9-]+)\s*,\s*(.+?)\s*\)/i) || [];
		//new debug({message:"simple handler_array "}, handler_array)
		handler.handler = handler_array[1];
		handler.key = handler_array[2];
		//tick_array[1].replace(/\\"/g, "'") 
		let tick_array:Array<string> = handler_array[3].match(/^'(.+)'$/) || [];
		new todo({message:"need to continue working on proper quote and double quote handling in string matches"})
		handler.match = (tick_array.length == 2) ? tick_array[1]: handler_array[3];
	}
	return handler;
}

export function parse_handlers(handler_string:string):Array<SlimViewHandler> {
//new trace({message:'entry', value: "/" + handler_string + "/"});
	let handlers:Array<SlimViewHandler> = [];
		
	let handler_array:Array<string> = handler_string.match(/[a-z]+\(.+\)/ig) || [];
new debug({message:"handler_array", value:handler_array});
	const handler:SlimViewHandler = get_handler(handler_array[0])
	if(handler.is_valid()) {
		handlers.push(handler);
	}
	else if(handler.is_complex()) {
		new debug({message:"handler.is_complex()"})
	}
//new trace({message:'exit value', value:handlers});
	return handlers;
}