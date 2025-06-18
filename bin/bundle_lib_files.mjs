import console from 'console'
import fs from 'fs'
import path from 'path'
const custom_library_files = [];
const library_paths = [path.join(process.cwd(), "../lib/typescript"), path.join(process.cwd(), "../lib/custom_definitions")];
const cpp_file = '../src/slim_system_libraries_typescript.cpp'
const cpp_custom_libraries = '../src/slim_builtins_typescript_initial_custom_libraries.cpp'
let output = "#include <memory>\n"
output += "#include <string>\n"
output += "#include <slim/common/memory_mapper.h>\n"
output += "namespace slim::system::libraries {\n"
output += "slim::common::memory_mapper::map_container typescript_map_container = {"
library_paths.map((library_path) => {
	const files = fs.readdirSync(library_path)
	files.map((file) => {
		if(file.endsWith(".d.ts")) {{
			const content = fs.readFileSync(library_path + "/" + file, 'utf8')
			let library_file = file;
			if(library_path.endsWith("lib/custom_definitions")) {
				library_file = `/lib/${file}`;
				custom_library_files.push(library_file)
			}
			output += `\{"${library_file}", std::make_shared<std::string>(R"+++(${content})+++")\},`
		}}
	})
})
const content = fs.readFileSync(path.join(process.cwd(), "../lib") + "/" + "slim_typescript.mjs", 'utf8')
output += `\{"file:///bin/typescript.mjs", std::make_shared<std::string>(R"+++(${content})+++")\},`
let fixed_output = output.slice(0, -1) + "};}"
console.log(`writing ${cpp_file}`)
fs.writeFileSync(cpp_file, fixed_output)
