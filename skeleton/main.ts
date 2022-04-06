import { debug, error, trace, todo, merge_json, SlimTypes} from './dep.ts'
const dotrace:boolean = false;
const dodebug:boolean = false;
if(dotrace) new trace({message:"command line arguments", value:Deno.args})
try {
	const program_definitions:SlimTypes.KeyValueAny = JSON.parse(Deno.readTextFileSync('./program.json'));
	if(Deno.args.length == 0) {
		do_handler(program_definitions);
	}
	else {
		do_handler(program_definitions, Deno.args);
	}
}
catch(e) {
	new error(e);
}

function do_handler(configuration:SlimTypes.KeyValueAny, args:Array<string>=[]) {
	if(dotrace) new trace({"message":"arguments", value:args});
	if(dotrace) new trace({"message":"program_definitions", value:configuration});
	switch(args[0]) {
		case 'render':
			// help(configuration.handlers.render, args);
			break;
		case 'help':
		case '--help':
		default:
				configuration.handlers.find((handler:SlimTypes.KeyValueAny) => {
					//new debug(handler.name)
					if(handler.name == 'help') {
						if(dodebug) new debug(handler)
						help(handler, args);
					}
				});
			break;
	}
}
function help(handlers:SlimTypes.KeyValueAny, args:Array<string>=[]) {
	if(dotrace) new trace({"message":"arguments", value:args});
	if(dotrace) new trace({"message":"", value:handlers});
	switch(args[0]) {
		case 'help':
		case '--help':
				print_help(helps.primary);		
			switch(args[1]) {
				case 'render':
					print_help(handler.primary);
					break;
				default:
					break;
			}
			break;
		default:
				print_help(handler.default);
			break;
	}
}
async function print_help(lines:Array<SlimTypes.KeyValueAny>) {
	if(dotrace) new trace({message:"", value:lines})
	for(let line of lines) {
		line = await merge_json({ text:"", indent:0, newline: { pre: 0, post: 0, }},line);
		for(let index = 0; index < line.newline.pre; index++) {
			await Deno.stdout.write(new TextEncoder().encode('\n'));
		}
		for(let index = 0; index < line.indent; index++) {
			await Deno.stdout.write(new TextEncoder().encode('\t'));
		}
		await Deno.stdout.write(new TextEncoder().encode(line.text));
		for(let index = 0; index < line.newline.post; index++) {
			await Deno.stdout.write(new TextEncoder().encode('\n'));
		}
	}
}
