import fs from 'fs'
import path from 'path'
const library_path = path.join(process.cwd(), "../third_party/typescript")
const cpp_file = '../src/slim_builtins_typescript_definitions.cpp'
let output = "#include <unordered_map>\n"
output += "#include <string>\n"
output += "namespace slim::builtins::typescript {"
output += "std::unordered_map<std::string, std::string> raw_typescript_definitions = {"
const files = fs.readdirSync(library_path)
files.map((file) => {
	if(file.endsWith(".d.ts")) {{
		const content = fs.readFileSync(library_path + "/" + file, 'utf8')
		output += `\{"${file}", R"+++(${content})+++"\},`
	}}
})
const fixed_output = output.slice(0, -1) + "};}"
fs.writeFileSync(cpp_file, fixed_output)
