#include <fstream>
#include <algorithm>
#include <iostream>
/*
 * args: file_to_hex_array input_file_name
 * output: stdout
 * purpose: convert tsc typescript compiler into hex format
 *          for use as typescript interpreter in embedded
 *          c++ executables
 * author: greergan@gmail.com
 * copywrite: Jeff Greer
 * date: April 6, 2022
 * license: MIT
 */
int main(int argc, char* argv[]) {
	std::ifstream input_file(argv[1]);
	if(input_file) {
		std::cout << "#ifndef __TYPESCRIPT__INCLUDE\n#define __TYPESCRIPT__INCLUDE\n";
		std::cout << "const unsigned char type_script_source[] = {";
        std::for_each(std::istreambuf_iterator<char>(input_file), 
			std::istreambuf_iterator<char>(),
				[](const int& c) {std::cout << "0x" << std::hex << c << ","; });
		std::cout << "}\n#endif\n";
	}
	return 0;
} 