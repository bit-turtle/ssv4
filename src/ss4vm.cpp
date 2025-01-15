/* ss4vm.cpp
* The Super Seal Version 4 Virtual Machine
* Usage: ss4vm [rom file]
*/

#include <iostream>
#include <fstream>

// CImg for Graphics
#include "CImg.h"
using namespace cimg_library;

// Display Variables
CImg<uint8_t>* display;
CImgDisplay* window;
uint8_t display_x = 0;
uint8_t display_y = 0;

// Argh! Library
#include "argh.h"

// CPU Flags
bool zero = false;
bool overflow = false;
bool error = false;

// CPU Registers
#include <cstdint>
// r Register
uint8_t r = 0;
// ALU Registers
uint8_t x = 0;
uint8_t y = 0;
// Instruction Registers
uint8_t instl = 0;
uint8_t insth = 0;
// Address Registers
uint8_t addrl = 0;
uint8_t addrh = 0;

// Register Dump
void regdump() {
	std::cout << "registers:" << std::endl;
	std::cout << "- main (r): " << std::hex << static_cast<unsigned>(r) << std::endl;
	std::cout << "- x: " << std::hex << static_cast<unsigned>(x) << std::endl;
	std::cout << "- y: " << std::hex << static_cast<unsigned>(y) << std::endl;
	std::cout << "- inst: " << std::hex << static_cast<unsigned>(insth << 4 | instl) << std::endl;
	std::cout << "- addr: " << std::hex << static_cast<unsigned>(addrh << 4 | addrl) << std::endl;
}

// Memory
uint8_t memory[0xff][0xff] = { 0 };

// I/O Ports

// I/O Mapping
enum iotype {
	NONE = 0,
	CONSOLE,
	TIMER,
	DISPLAY_X,
	DISPLAY_Y,
	DISPLAY_R,
	DISPLAY_B,
	DISPLAY_G
};

iotype iomap[0xff][0xff] = {
	CONSOLE,
};

// Instruction Rom
uint8_t rom[0xff][0xff] = { 0 };

// Hardware Values
bool running = true;

// Input Processing
uint8_t input() {
	iotype type = iomap[addrh][addrl];
	switch (type) {
		case CONSOLE: {
			uint8_t input;
			std::cin >> input;
			return input;
		} break;
		default:
			std::cout << "Warning: Input Not Implemented" << std::endl;
			return 0x00;
	}
}

// Output Processing
void output(uint8_t value) {
	iotype type = iomap[addrh][addrl];
	switch (type) {
		case CONSOLE: {
			std::cout << static_cast<char>(value);
		} break;
		default:
			std::cout << "Warning: Output Not Implemented" << std::endl;
	}
}

// Returns conditional to increment inst
bool setlocation(uint8_t location) {
	switch (location) {
		case 0x0:
			x = r;
			break;
		case 0x1:
			y = r;
			break;
		case 0x2:
			instl = r;
			break;
		case 0x3:
			insth = r;
			break;
		case 0x4:
			addrl = r;
			return false;
		case 0x5:
			addrh = r;
			return false;
		case 0x6:
			memory[addrh][addrl] = r;
			break;
		case 0x7:
			output(r);
			break;
		default:
			std::cout << "Warning: Set Location Not Implemented!" << std::endl;
	}
	return true;
}
void getlocation(uint8_t location) {
	switch (location) {
		case 0x0:
			r = x;
			break;
		case 0x1:
			r = y;
			break;
		case 0x2:
			r = instl;
			break;
		case 0x3:
			r = insth;
			break;
		case 0x4:
			r = addrl;
			break;
		case 0x5:
			r = addrh;
			break;
		case 0x6:
			r = memory[addrh][addrl];
			break;
		case 0x7:
			r = input();
			break;
		default:
			std::cout << "Warning: Get Location Not Implemented!" << std::endl;
	}
}
// Alu Function
void aluoperation(uint8_t operation) {
	switch (operation) {
		default:
			std::cout << "Warning: ALU Operation Not Implemented!" << std::endl;
	}
}

// Returns conditional to increment inst or not
bool execute(uint8_t instruction) {
	// Test Condition
	uint8_t condition = instruction >> 6;
	if (condition == 0b01 && !zero) return true;
	if (condition == 0b10 && !overflow) return true;
	if (condition == 0b11 && !error) return true;
	// Get Opcode and Operand
	uint8_t opcode = instruction >> 4 & 0b11;
	uint8_t operand = instruction & 0b1111;
	if (opcode == 0) {
		// Data Move
		bool get = (operand >> 3) ? true : false;
		uint8_t location = operand & 0b111;
		if (get) getlocation(location);
		else return setlocation(location);	// If inst set return conditional to not increment inst
	}
	else if (opcode == 1) {
		// ALU Operation
		aluoperation(operand);
	}
	else if (opcode == 2) {
		// Set Low Bits of r
		r &= 0xf0;
		r |= operand & 0x0f;
	}
	else if (opcode == 3) {
		// Set High Bits of r
		r &= 0x0f;
		r |= operand << 4 & 0xf0;
	}
	return true;
}

int main(int argc, char* argv[]) {
	// Argh! Parser
	argh::parser cmdl(argc, argv);
	// Check Parameters
	if (!cmdl(1)) {
		std::cout << "ss4vm help:" << std::endl;
		std::cout << "Usage: ss4vm [rom file]" << std::endl;
		return EXIT_FAILURE;
	}
	// Open Rom File
	std::ifstream file(cmdl[1]);
	if (!file.is_open()) {
		std::cout << "Failed to open Rom File!" << std::endl;
		return EXIT_FAILURE;
	}
	// Load Rom File
	for (int i = 0; i < 0xffff; i++) {
		char c;
		file.get(c);
		rom[i/0xff][i%0xff] = static_cast<uint8_t>(c);
		if (file.eof()) break;
	}
	std::cout << "ROM Loaded!" << std::endl;
	// Create Window
	display = new CImg<uint8_t>(256,256,1,3,0);
	window = new CImgDisplay(*display, "ss4vm");
	// Virtual Machine Start
	while (!window->is_closed()) {
		// Get Instruction
		uint8_t instruction = rom[insth][instl];
		// Execute Instruction
		execute(instruction);
		// Increment Instruction Register
		instl++;
		if (instl == 0) insth++;
	}
	return EXIT_SUCCESS;
}
