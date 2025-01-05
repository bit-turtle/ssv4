/* ss4vm.cpp
* The Super Seal Version 4 Virtual Machine
* Usage: ss4vm [rom file]
*/

#include <iostream>
#include <fstream>

// SFML for Graphics
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// Argh! Library
#include "argh.h"

// CPU Flags
bool zero = false;
bool overflow = false;
bool error = false;

// CPU Registers
#include <cstdint>
// Main Register
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
	DISPLAY,
	HARDWARE,
};

iotype iomap[0xff][0xff] = {
	CONSOLE,
	TIMER,
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

void execute(uint8_t instruction) {
	// Test Condition
	uint8_t condition = instruction >> 6;
	if (condition == 0b01 && !zero) return;
	if (condition == 0b10 && !overflow) return;
	if (condition == 0b11 && !error) return;
	// Run Opcode
	// TODO
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
		file >> rom[i];
		if (file.eof()) break;
	}
	// Virtual Machine Start
	while (running) {
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
