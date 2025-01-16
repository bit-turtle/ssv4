/* ss4as.cpp
* The Super Seal Version 4 Assembler
* Usage: ss4as [assembly source file] [output binary file]
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Argh! Library
#include "argh.h"

#include <cstdint>
// Compilation Rom (65536 addresses)
uint8_t rom[65536] = { 0 };
uint16_t ptr = 0;

// Debug Message Flag
bool debug = true;

// Write Instruction to Rom
void instruction(uint8_t condition, uint8_t opcode, uint8_t operands) {
	uint8_t inst = 0x00;
	inst |= condition << 6;
	inst |= opcode << 4;
	inst |= operands & 0xf;
	rom[ptr] = inst;
	ptr++;
	std::cout << "- instruction: 0x" << std::hex << static_cast<unsigned>(inst) << std::endl;
}

// Location Name Parser
uint8_t location(std::string name) {
	if (name == "x") return 0x0;
	else if (name == "y") return 0x1;
	else if (name == "instl") return 0x2;
	else if (name == "insth") return 0x3;
	else if (name == "addrl") return 0x4;
	else if (name == "addrh") return 0x5;
	else if (name == "memory") return 0x6;
	else if (name == "port") return 0x7;
	return 0xff;
}

// ALU Operation Parser
uint8_t operation(std::string name) {
	if (name == "resetflags") return 0x0;
	else if (name == "resetopflags") return 0x1;
	else if (name == "reseterror") return 0x2;
	else if (name == "seterror") return 0x3;
	else if (name == "add") return 0x4;
	else if (name == "carryadd") return 0x5;
	else if (name == "subtract") return 0x6;
	else if (name == "borrowsubtract") return 0x7;
	else if (name == "or") return 0x8;
	else if (name == "xor") return 0x9;
	else if (name == "and") return 0xa;
	else if (name == "nand") return 0xb;
	else if (name == "rightshift") return 0xc;
	else if (name == "carryrightshift") return 0xd;
	else if (name == "leftshift") return 0xe;
	else if (name == "carryleftshift") return 0xf;
	return 0xff;
}

// Compile Line
bool compile(std::string code) {
	if (debug) std::cout << "compile: " << code << std::endl;
	std::stringstream tokens(code);
	std::string op;
	tokens >> op;
	// Conditions
	uint8_t condition = 0;
	if (op == "zero") {
		if (debug) std::cout << "- condition: zero" << std::endl;
		condition = 1;
		tokens >> op;
	}
	else if (op == "overflow") {
		if (debug) std::cout << "- condition: overflow" << std::endl;
		condition = 2;
		tokens >> op;
	}
	else if (op == "error") {
		if (debug) std::cout << "- condition: error" << std::endl;
		condition = 3;
		tokens >> op;
	}
	// Operations
	if (debug) std::cout << "- operation: " << op << std::endl;
	// Load Operations
	if (op == "load") {
		tokens >> op;
		if (debug) std::cout << "- operand: " << op << std::endl;
		uint8_t value;
		try {
			value = std::stoi(op, nullptr, 0);
		}
		catch (...) {
			std::cout << "Invalid Value!" << std::endl;
			return false;
		}
		instruction(condition, 0b10, value & 0xf);
		instruction(condition, 0b11, value >> 4);
	}
	else if (op == "loadl") {
		tokens >> op;
		if (debug) std::cout << "- operand: " << op << std::endl;
		uint8_t value;
		try {
			value = std::stoi(op, nullptr, 0);
		}
		catch (...) {
			std::cout << "Invalid Value!" << std::endl;
			return false;
		}
		instruction(condition, 0b10, value & 0xf);
	}
	else if (op == "loadh") {
		tokens >> op;
		if (debug) std::cout << "- operand: " << op << std::endl;
		uint8_t value;
		try {
			value = std::stoi(op, nullptr, 0);
		}
		catch (...) {
			std::cout << "Invalid Value!" << std::endl;
			return false;
		}
		instruction(condition, 0b11, value & 0xf);
	}
	// Data Move Operations
	else if (op == "set") {
		tokens >> op;
		if (debug) std::cout << "- operand: " << op << std::endl;
		uint8_t id = location(op);
		if (id == 0xff) {
			std::cout << "Invalid Location!" << std::endl;
			return false;
		}
		id &= 0x7;
		instruction(condition, 0b00, id);
	}
	else if (op == "get") {
		tokens >> op;
		if (debug) std::cout << "- operand: " << op << std::endl;
		uint8_t id = location(op);
		if (id == 0xff) {
			std::cout << "Invalid Location!" << std::endl;
			return false;
		}
		id |= 0b1000;
		instruction(condition, 0b00, id & 0b1111);
	}
	// ALU Operations
	else {
		uint8_t id = operation(op);
		if (id == 0xff) {
			std::cout << "Invalid Command!" << std::endl;
			return false;
		}
		instruction(condition, 0b01, id & 0xf);
	}
	return true;
}

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
	std::vector<std::string> srcs;
	for (int i = 1; cmdl(i); i++)
		srcs.push_back(cmdl(i).str());

	// Name of Output Binary file
	std::string bin;
	if (cmdl({"-o", "--output"}))
		bin = cmdl({"-o", "--output"}).str();
	else
		bin = "output.bin";

	// Check number of Assembly Source files
	if (srcs.size() == 0) {
		std::cout << "No files to compile!" << std::endl;
		std::cout << "For help, run: ss4as --help" << std::endl;
		return EXIT_FAILURE;
	}

	// Compile each Assembly Source file
	for (int i = 0; i < srcs.size(); i++) {
		std::cout << "Compiling Source File: " << srcs.at(i) << std::endl;

		// Open Assembly Source File
		std::ifstream src(srcs.at(i));

		// Test if Assembly Source File opened
		if (!src.is_open()) {
			std::cout << "Failed to open Assembly Source File!" << std::endl;
			return EXIT_FAILURE;
		}

		// Process Assembly Source file
		int loc = 0;
		std::string line;
		while(std::getline(src, line)) {
			// Increment location
			loc++;

			// Trim Comments
			std::stringstream comment(line);
			std::string code;
			std::getline(comment, code, '#');
			
			// Skip Empty Lines
			if (code.length() == 0) continue;

			// Process Macros
			if (code[0] == '@') {
				if (debug) std::cout << "macro: " << code << std::endl;
				// Set Rom Pointer
				code.erase(code.begin());
				try {
					ptr = std::stoi(code, nullptr, 0);
				}
				catch (...) {
					std::cout << "Invalid Value!" << std::endl;
					std::cout << "Macro Error on Line " << std::dec << loc << "!" << std::endl;
					return EXIT_FAILURE;
				}
				if (debug) std::cout << "- location: 0x" << std::hex << ptr << std::endl;

				// Next Line
				continue;
			}

			// Compile Code and Check for Errors
			if (!compile(code)) {
				std::cout << "Compile Error on Line " << std::dec << loc << "!" << std::endl;
				return EXIT_FAILURE;
			}
		}

		// Close Assembly Source File
		src.close();

		// Compiled File
		std::cout << "Compiled Source File: " << srcs.at(i) << std::endl;
	}

	// Compilation Finished
	std::cout << "Compilation Finished!" << std::endl;

	// Output Binary file
	std::ofstream output(bin);
	if (!output.is_open()) {
		std::cout << "Failed to open Output Binary File!" << std::endl;
		return EXIT_FAILURE;
	}
	for (int i = 0; i < 65536; i++) {
		output << rom[i];
	}
	output.close();

	return EXIT_SUCCESS;
}
