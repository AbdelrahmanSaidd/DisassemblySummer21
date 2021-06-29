/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only.
	It prints "Unkown Instruction" for all other instructions!

	Usage example:
		$ rvcdiss t1.bin
	should print out:
		0x00000000	0x00100013	ADDI	x0, x0, 0x1
		0x00000004	0x00100093	ADDI	x1, x0, 0x1
		0x00000008	0x00100113	ADDI	x2, x0, 0x1
		0x0000000c	0x001001b3	ADD		x3, x0, x1
		0x00000010	0x00208233	ADD		x4, x1, x2
		0x00000014	0x004182b3	ADD		x5, x3, x4
		0x00000018	0x00100893	ADDI	x11, x0, 0x1
		0x0000001c	0x00028513	ADDI	xa, x5, 0x0
		0x00000020	0x00000073	Unkown Instruction

	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0

void emitError(const char* s)	//OUR TOUCH (const)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	
	funct7 = (instWord >> 25) & 0x0000007F; //OUR TOUCH

	// — inst[31] — inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	printPrefix(instPC, instWord);

	if (opcode == 0x33) {		// R Instructions
		switch (funct3) {
		case 0:
			if (funct7 == 32)
				cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			else
				cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";

			break;
		case 1:
			cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			break;
		case 2:
			cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			break;
		case 3:
			cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			break;
		case 4:
			cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			break;
		case 5:
			if (funct7 == 32)
				cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			else
				cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";

			break;
		case 6:
			cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			break;

		case 7:
			cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			break;
		default:
			cout << "\tUnkown R Instruction \n";
		}
	}
	else if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else {
		cout << "\tUnkown Instruction \n";
	}

}

int main(int argc, char* argv[]) {

	unsigned int instWord = 0;
	ifstream inFile;
	ofstream outFile;

	if (argc < 2) emitError("use: rvcdiss <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg(0, inFile.beg);
		if (!inFile.read((char*)memory, fsize)) emitError("Cannot read from input file\n");

		while (true) {
			instWord = (unsigned char)memory[pc] |
				(((unsigned char)memory[pc + 1]) << 8) |
				(((unsigned char)memory[pc + 2]) << 16) |
				(((unsigned char)memory[pc + 3]) << 24);
			pc += 4;
			// remove the following line once you have a complete simulator
			if (pc == 40) break;			// stop when PC reached address 32
			instDecExec(instWord);
		}
	}
	else emitError("Cannot access input file\n");
}