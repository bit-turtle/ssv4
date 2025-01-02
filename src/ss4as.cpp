/* ss4as.cpp
* The Super Seal Version 4 Assembler
* Usage: ss4as [assembly source file] [output binary file]
*/

#include <iostream>
#include <string>
#include <vector>

// Argh! Library
#include "argh.h"

int main(int argc, char* argv[]) {
	// Argh! Parser
	argh::parser cmdl;
	// Add "-o" parameter
	cmdl.add_params({"-o", "--output"});
	// Parse Arguments
	cmdl.parse(argc, argv);

	// Check for "--help" Flag
	if (cmdl[{"-h", "--help"}]) {
		std::cout << "ss4as help" << std::endl;
		std::cout << "Usage: ss4as [Assembly Source Files...]" << std::endl;
		std::cout << "-o, --output: Set Binary Output filename (Default: output.bin)" << std::endl;
		std::cout << "-h, --help: Display this Help Message" << std::endl;
		return EXIT_SUCCESS;
	}

	// Names of Assembly Source files
	std::vector<std::string> src;
	for (int i = 1; cmdl(i); i++)
		src.push_back(cmdl(i).str());
	// Name of Output Binary file
	std::string bin;
	if (cmdl({"-o", "--output"}))
		bin = cmdl({"-o", "--output"}).str();
	else
		bin = "output.bin";

	// Check number of Assembly Source files
	if (src.size() == 0) {
		std::cout << "No files to compile!" << std::endl;
		std::cout << "For help, run: ss4as --help" << std::endl;
		return EXIT_FAILURE;
	}

	// Compile each Assembly Source file
	for (int i = 0; i < src.size(); i++) {
		std::cout << "Compiling Source File: " << src.at(i) << std::endl;
	}

	return EXIT_SUCCESS;
}
