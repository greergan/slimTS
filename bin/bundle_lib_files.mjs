import fs from 'fs'
import path from 'path'
const library_path = path.join(process.cwd(), "../lib/typescript")
const cpp_file = '../src/slim_builtins_typescript_initial_library.cpp'
let output = "#include <memory>\n"
output += "#include <string>\n"
output += "#include <slim/common/memory_mapper.h>\n"
output += "namespace slim::builtins::typescript {\n"
output += "slim::common::memory_mapper::map_container typescript_map_container = {"
const files = fs.readdirSync(library_path)
files.map((file) => {
	if(file.endsWith(".d.ts")) {{
		const content = fs.readFileSync(library_path + "/" + file, 'utf8')
		output += `\{"${file}", std::make_shared<std::string>(R"+++(${content})+++")\},`
	}}
})
const content = fs.readFileSync(path.join(process.cwd(), "../lib") + "/" + "slim_typescript.mjs", 'utf8')
output += `\{"slim_typescript.mjs", std::make_shared<std::string>(R"+++(${content})+++")\},`
const fixed_output = output.slice(0, -1) + "};}"
console.log(`writing ${cpp_file}`)
fs.writeFileSync(cpp_file, fixed_output)
