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

unsigned int pc = 0x0;	//Program counter

//Array of the 32 available registers stored in order. For example, if we need to get register ra (x1), it is stored in reg[1].
string reg[32] = { "zero", "ra", "sp", "gp", "tp","t0", "t1", "t2", "s0", "s1",
					"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7","s2", "s3",
					"s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };

char memory[8 * 1024];	// only 8KB of memory located at address 0

void emitError(const char* s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec(unsigned int instWord, bool flag)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;								//Used in the 32-bit instructions
	unsigned int rd_dash, rs1_dash, rs2_dash, funct2_1, funct2_2, funct4, funct6;	//Used in the 16-bit (C) instructions
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;

	if (flag) //32-bit instructions.
	{
		unsigned int instPC = pc - 4;
		opcode = instWord & 0x0000007F;
		rd = (instWord >> 7) & 0x0000001F;
		funct3 = (instWord >> 12) & 0x00000007;
		rs1 = (instWord >> 15) & 0x0000001F;
		rs2 = (instWord >> 20) & 0x0000001F;
		funct7 = (instWord >> 25) & 0x0000007F;

		// — inst[31] — inst[30:25] inst[24:21] inst[20]
		I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));	//We take the rightmost 11 bits and then we check the 12th bit for the sign

		S_imm = ((instWord >> 7) & 0x1F) | ((instWord >> 25) & 0x3F << 5) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
				//imm[10:5]						//imm[4:0]					//imm[11] sign bit
		B_imm = ((instWord >> 7 & 0x1) << 12) | ((instWord >> 25 & 0x3F) << 5) | (instWord >> 8 & 0xF) | ((instWord >> 31) ? 0xFFFFF800 : 0x0);
					//imm[11]						//imm[10:5]						//imm[4:1]					//imm[12], sign bit

		J_imm = ((instWord & 0x7FE00000) >> 20) | ((instWord >> 20 & 0x1) << 11) | ((instWord >> 12 & 0xFF) << 12) | ((instWord >> 31) ? 0xFFFFF800 : 0x0);
						//imm[10:1]							//imm[11]						//imm[19:12]					//imm[20], sign bit

		U_imm = ((instWord & 0xFFFFF00) >> 12);

		printPrefix(instPC, instWord);

		if (opcode == 0x33) // R Instructions
		{
			switch (funct3) {
			case 0:
				if (funct7 == 32)
					cout << "\tSUB\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				else
					cout << "\tADD\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 1:
				cout << "\tSLL\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 2:
				cout << "\tSLT\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 3:
				cout << "\tSLTU\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 4:
				cout << "\tXOR\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 5:
				if (funct7 == 32)
					cout << "\tSRA\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				else
					cout << "\tSRL\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";

				break;
			case 6:
				cout << "\tOR\tx" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;

			case 7:
				cout << "\tAND\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			default:
				cout << "\tUnkown R Instruction \n";
			}
		}

		else if (opcode == 0x13) // I-Instructions
		{
			switch (funct3) {
			case 0:	cout << "\tADDI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 1: cout << "\tSLLI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 2: cout << "\tSLTI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 3: cout << "\tSLTIU\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 4: cout << "\tXORI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 5: if (funct7 == 0)
				cout << "\tSRLI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				  else
				cout << "\tSRAI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 6:  cout << "\tORI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			case 7: cout << "\tANDI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
				break;
			default:
				cout << "\tUnkown I Instruction \n";
			}
		}
		else if (opcode == 0x3) // Type- I Load instructions
		{
			switch (funct3)
			{
			case 0: cout << "\tLB\t" << reg[rd] << ", " << hex << "0x" << (int)I_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			case 1: cout << "\tLH\t" << reg[rd] << ", " << hex << "0x" << (int)I_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			case 2: cout << "\tLW\t" << reg[rd] << ", " << hex << "0x" << (int)I_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			case 4: cout << "\tLBU\t" << reg[rd] << ", " << hex << "0x" << (int)I_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			case 5: cout << "\tLHU\t" << reg[rd] << ", " << hex << "0x" << (int)I_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			default: cout << "Unkown Type-I Load instruction \n";
			}
		}
		else if (opcode == 0x23) //S-Type
		{
			switch (funct3)
			{
			case 0:  cout << "\tSH\t" << reg[rs2] << ", " << hex << "0x" << (int)S_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			case 1:  cout << "\tSB\t" << reg[rs2] << ", " << hex << "0x" << (int)S_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			case 2:  cout << "\tSW\t" << reg[rs2] << ", " << hex << "0x" << (int)S_imm << "(" << reg[rs1] << ")" << "\n";
				break;
			default:
				cout << "\tUnknown Type-S Instruction \n";
			}
		}
		else if (opcode == 0x63) //B-Type
		{
			switch (funct3)
			{
			case 0:
				cout << "\tBEQ\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 1:
				cout << "\tBNE\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 4:
				cout << "\tBLT\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 5:
				cout << "\tBGE\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 6:
				cout << "\tBLTU\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 7:
				cout << "\tBGEU\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			default:
				cout << " Unkown Type-B Instruction " << "\n";
			}
		}
		else if (opcode == 0x37) //LUI
		{
			cout << "\tLUI\t" << reg[rd] << ", " << hex << "0x" << (int)U_imm << endl;
		}

		else if (opcode == 0x17) //AUIPC
		{
			cout << "\tAUIPC\t" << reg[rd] << ", " << hex << "0x" << (int)U_imm << endl;
		}
		else if (opcode == 0x67) //JALR
		{
			cout << "\tJALR\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_imm << "\n";
		}
		else if (opcode == 0x6F) //J-Type
		{
			cout << "\tJAL\t" << reg[rd] << " , " << hex << "0x" << (int)J_imm << endl;
		}
		else if (opcode == 0x73) //Ecall (added to avoid having an unknown instruction when trying the test cases.
		{
		cout << "\tECALL\t" << endl;
		}

		else
		{
			cout << "\tUnkown Instruction \n";
		}
	}

	else //16-bit instructions
	{
		unsigned int instPC = pc - 2;	//-2 and not -4 because it is only 16 bits and not 32.


		opcode = instWord & 0x00000003;		//same spot same number of bits in all of them.
		rd = (instWord >> 7) & 0x0000001F;	//uncompressed rd

		//There is 2 funct2 variables because funct2 is used in different locations in different instruction words.
		funct2_1 = (instWord >> 10) & 0x00000003;
		funct2_2 = (instWord >> 5) & 0x00000003;

		funct3 = (instWord >> 13) & 0x00000007;
		funct4 = (instWord >> 12) & 0x0000000F;
		funct6 = (instWord >> 10) & 0x0000003F;

		rs1 = (instWord >> 7) & 0x0000001F; //uncompressed rs1
		rs2 = (instWord >> 2) & 0x0000001F; //uncompressed rs2

	
		rd_dash = (instWord >> 2) & 0x00000007; //rd'
		rs1_dash = (instWord >> 7) & 0x00000007; //rs1'
		rs2_dash = (instWord >> 2) & 0x00000007; //rs2'


		printPrefix(instPC, instWord);

		if (opcode == 0x0) //opcode = 00
		{
			unsigned int I_LS;	//Immediate for LW and SW instructions.
			I_LS = (instWord & 0x0010) | ((instWord >> 9) & 0x000E) | ((instWord >> 6) & 0x1);

			if (funct3 == 2)
				cout << "\tC.LW\t" << reg[rd] << ", " << hex << "0x" << (int)I_LS << "(" << reg[rs1] << ")" << "\n";
			else if (funct3 == 6)
				cout << "\tC.SW\t" << reg[rs2] << ", " << hex << "0x" << (int)I_LS << "(" << reg[rs1] << ")" << "\n";
		}
		else if (opcode == 0x1) //opcode = 01
		{
			int I_ADDI = ((instWord >> 7) & 0x0020) | ((instWord >> 2) & 0x001F) | ((instWord >> 12) ? 0xFFFFFF0 : 0x0);

			int I_JAL = ((instWord << 2) & 0x200) | ((instWord >> 1) & 0x180) | ((instWord << 1) & 0x40) | ((instWord >> 1) & 0x20)
				| ((instWord << 3) & 0x10) | ((instWord >> 7) & 0x8) | ((instWord >> 2) & 0xE) | ((instWord >> 12) ? 0xFFFFFF0 : 0x0);

			int I_LUI = ((instWord << 5) & 0x10000) | ((instWord >> 2) & 0xF800) | ((instWord >> 12) ? 0xFFE0000 : 0x0);		//Double Check

			unsigned int I_Shift = ((instWord >> 7) & 0x0020) | ((instWord >> 2) & 0x001F);	//Used in SRAI, SRLI, SLLI

			int I_ANDI = ((instWord >> 7) & 0x0020) | ((instWord << 10) & 0x001F) | ((instWord >> 12) ? 0xFFFFFF0 : 0x0);

			if (funct3 == 0)
				cout << "\tC.ADDI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_ADDI << "\n";
			else if (funct3 == 1)
				cout << "\tC.JALR\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_JAL << "\n";
			else if (funct3 == 3)
				cout << "\tC.LUI\t" << reg[rd] << ", " << hex << "0x" << (int)I_LUI << endl;
			else if (funct3 == 4)
			{
				if (funct2_1 == 0)
					cout << "\tC.SRLI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_Shift << "\n";
				else if (funct2_1 == 1)
					cout << "\tC.SRAI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_Shift << "\n";
				else if (funct2_1 == 2)
					cout << "\tC.ANDI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_ANDI << "\n";
				else if (funct2_1 == 3)
				{
					if (funct2_2 == 0)
						cout << "\tC.SUB\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
					else if (funct2_2 == 1)
						cout << "\tC.XOR\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
					else if (funct2_2 == 2)
						cout << "\tC.OR\tx" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
					else if (funct2_2 == 3)
						cout << "\tC.AND\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				}
			}
			else if (opcode == 0x2)//opcode = 10
			{
				if (funct3 == 0)
					cout << "\tC.SLLI\t" << reg[rd] << ", " << reg[rs1] << ", " << hex << "0x" << (int)I_Shift << "\n";
				else if (funct3 == 4)
				{
					if (rs2 == 0)
						cout << "\tC.JALR\t" << reg[1] << ", 0(" << reg[rs1] << ")" << "\n";
					else
						cout << "\tC.ADD\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				}
			}
			else
				cout << "\tUnknown Compressed Instruction \n";
		}
	}
}

int main(int argc, char* argv[]) 
{

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

		while (true) {	// 
			

			int temp = memory[pc] & 0x00000003;	//Checking the 1st and 2nd bit to determine the size of the instruction.
			bool flag = true;

			if (temp == 3)		//If the last 2 bits checked in temp are 1s, it is a 32-bit instruction
			{
				instWord = (unsigned char)memory[pc] | (((unsigned char)memory[pc + 1]) << 8) | (((unsigned char)memory[pc + 2]) << 16) | (((unsigned char)memory[pc + 3]) << 24);
				pc += 4;
			}

			else			   //16-bit instructions.
			{
				instWord = (unsigned char)memory[pc] | (((unsigned char)memory[pc + 1]) << 8);
				flag = false;
				pc += 2;
			}

			instDecExec(instWord, flag);
			if (!memory[pc]) break;	//break when we have went over all the elements (instructions) in the memory array.
		}
	}
	else emitError("Cannot access input file\n");
}