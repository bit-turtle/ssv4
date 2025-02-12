/* ss4vm.cpp
* The Super Seal Version 4 Virtual Machine
* Usage: ss4vm [rom file]
*/

#include <iostream>
#include <fstream>

// Thread for Speed
#include <thread>

// Chrono for Time
#include <chrono>

// CImg for Graphics
#include "CImg.h"
using namespace cimg_library;

// Display Variables
CImg<uint8_t>* display;
CImgDisplay* window;
uint8_t display_x = 0;
uint8_t display_y = 0;

// Display Update Loop
void updatedisplay(bool* running) {
	while (*running) {
		window->display(*display);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

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
	CONSOLE = 0,
	DISPLAY_X = 1,
	DISPLAY_Y = 2,
	DISPLAY_R = 3,
	DISPLAY_G = 4,
	DISPLAY_B = 5,
};

// Instruction Rom
uint8_t rom[0xff][0xff] = { 0 };

// Hardware Values
bool running = true;

// Input Processing
uint8_t input() {
	iotype type = static_cast<iotype>(addrh << 8 | addrl);
	switch (type) {
		case CONSOLE: {
			char c;
			std::cin.get(c);
			uint8_t input = static_cast<uint8_t>(c);
			return input;
		} break;
		case DISPLAY_X: {
			return display_x;
		} break;
		case DISPLAY_Y: {
			return display_y;
		} break;
		case DISPLAY_R: {
			return display->atXY(display_x, display_y, 0);
		} break;
		case DISPLAY_G: {
			return display->atXY(display_x, display_y, 1);
		} break;
		case DISPLAY_B: {
			return display->atXY(display_x, display_y, 2);
		} break;
		default:
			std::cout << "Warning: Input Not Implemented" << std::endl;
			return 0x00;
	}
}

// Output Processing
void output(uint8_t value) {
	iotype type = static_cast<iotype>(addrh << 8 | addrl);
	switch (type) {
		case CONSOLE: {
			std::cout << static_cast<char>(value);
		} break;
		case DISPLAY_X: {
			display_x = value;
		} break;
		case DISPLAY_Y: {
			display_y = value;
		} break;
		case DISPLAY_R: {
			display->atXY(display_x, display_y, 0) = value;
		} break;
		case DISPLAY_G: {
			display->atXY(display_x, display_y, 1) = value;
		} break;
		case DISPLAY_B: {
			display->atXY(display_x, display_y, 2) = value;
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
			return false;
		case 0x3:
			insth = r;
			return false;
		case 0x4:
			addrl = r;
			break;
		case 0x5:
			addrh = r;
			break;
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
		// Flag Operations
		case 0x0:	// Reset all flags
			zero = false;
			overflow = false;
			error = false;
			break;
		case 0x1:	// Reset alu operation flags
			zero = false;
			overflow = false;
			break;
		case 0x2:	// Reset error flag
			error = false;
			break;
		case 0x3:	// Set error flag
			error = true;
			break;
		// Arithmetic Operations
		case 0x4: {	// Add x and y
			int check = x + y;
			r = x + y;
			overflow = (r != check) ? true : false;
			zero = (r == 0) ? true : false;
		} break;
		case 0x5: {	// Add x and y with carry
			int check = x + y + 1;
			r = x + y + 1;
			overflow = (r != check) ? true : false;
			zero = (r == 0) ? true : false;
		} break;
		case 0x6: {	// Subtract x and y
			int check = x - y;
			r = x - y;
			overflow = (r != check) ? true : false;
			zero = (r == 0) ? true : false;
		} break;
		case 0x7: {	// Subtract x and y with borrow
			int check = x - y - 1;
			r = x - y - 1;
			overflow = (r != check) ? true : false;
			zero = (r == 0) ? true : false;
		} break;
		// Logic Operations
		case 0x8: {	// Or x and y
			r = x | y;
			zero = (r == 0) ? true : false;
		} break;
		case 0x9: {	// Xor x and y
			r = x ^ y;
			zero = (r == 0) ? true : false;
		} break;
		case 0xa: {	// And x and y
			r = x & y;
			zero = (r == 0) ? true : false;
		} break;
		case 0xb: {	// Nand x and y
			r = ~(x ^ y);
			zero = (r == 0) ? true : false;
		} break;
		case 0xc: {	// Rightshift x
			bool shiftbit = (x | 0b1) ? true : false;
			r = x >> 1;
			// Overflow with bit shifted out
			overflow = shiftbit;
			zero = (r == 0) ? true : false;
		} break;
		case 0xd: {	// Carry Rightshift x
			bool shiftbit = (x | 0b1) ? true : false;
			r = x >> 1 | 0b10000000;	// Set leftmost bit
			// Overflow with bit shifted out
			overflow = shiftbit;
			zero = (r == 0) ? true : false;
		} break;
		case 0xe: {	// Leftshift x
			bool shiftbit = (x | 0b1) ? true : false;
			r = x << 1;
			// Overflow with bit shifted out
			overflow = shiftbit;
			zero = (r == 0) ? true : false;
		} break;
		case 0xf: {	// Carry Leftshift x
			bool shiftbit = (x | 0b1) ? true : false;
			r = x << 1 | 0b1;
			// Overflow with bit shifted out
			overflow = shiftbit;
			zero = (r == 0) ? true : false;
		} break;
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
	// Check for "--help" Flag
	if (cmdl[{"-h","--help"}]) {
		std::cout << "ss4vm help" << std::endl;
		std::cout << "Usage: ss4vm [ROM File]" << std::endl;
		std::cout << "-h, --help: Display this Help Message" << std::endl;
		return EXIT_FAILURE;
	}
	// Check Parameters
	if (!cmdl(1)) {
		std::cout << "No ROM File Specified!" << std::endl;
		std::cout << "For help, run ss4vm --help" << std::endl;
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
	// Create Internal Display
	display = new CImg<uint8_t>(256,256,1,3,0);
	// Create Window
	window = new CImgDisplay(*display, "ss4vm");
	// Create Display Thread
	bool displaythreadrunning = true;
	std::thread displaythread(updatedisplay, &displaythreadrunning);
	// Virtual Machine Start
	bool increment = false;
	while (!window->is_closed()) {
		// Increment Instruction Register
		if (increment) {
			instl++;
			if (instl == 0) insth++;
		}
		increment = true;
		// Get Instruction
		uint8_t instruction = rom[insth][instl];
		// Execute Instruction
		increment = execute(instruction);
	}
	// Clean up Display Thread
	displaythreadrunning = false;
	displaythread.join();

	return EXIT_SUCCESS;
}
