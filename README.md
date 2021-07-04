# DisassemblySummer21
A disassembler which supports RV32IC instructions.
==================================================

Implementation Details:
---------------------------------
*Instruction Word:
The program accepts input machine code from a binary file into memory[8*1024] array (8kb of memory), then takes one instruction word to decode it. The disassembler supports RV32IC instructions, which means that compressed instructions could be mixed with 32-bit instructions. This is done by checking for the least significant 2 bits of each instruction word. If they are both ones it means that the word is 32-bits, so we take 32 bits from the binary file by concatenating 4 8-bits from the memory file, then the program counter is incremented by 4. Otherwise, if the two least significant bits are not one, we concatunate only 2 bytes from the memory using the program counter location. The program counter is incremented by two in this case.
--------------------------------
*Immediates Decoding:
To decode the Immediated from the instruction word, we used the shifting operator as well as some masking to get the correct number of bits of immediated together and get the correct number.
For example, the instruction word 0000000 01110 0001 0010 0100 0010 0011 which is an S-Type would give us 000000001000 as the offset. The immediate in this instruction word is split into two positions in the word. Bits [4:0] are located at bit [11:7] of the instruction word and bits [11:5] are located at bit [31:25] of the instruction word. Therfore, we shifted bits 31:25 25 bits to the right and masked these 7 bits with 0x3F and then shifted to the left 5 bits leaving the offset like 000000000000. After that we concatunate with the instruction word shifted right 7 bits and masked with 0x1F which gives us the offset 000000001000. We check for the sign bit to check whether we should sign extend or zero extend the offset.
--------------------------------
*ABI names:
In order to change the registers numbers to their ABI names, we made 2 arrays, one for the 32 bit regs and the other for the 16 bit regs. We put the ABI names in the order of the registers according to their binary values and when we needed to print a certain register, we pass that number to the array and the ABI name is printed. Since the compressed instructions only use specific registers s0-1 and a1-5, we made an array for them and we pass the register binary value ass well to fetch the name.
---------------------------------
*instructions opcode:
The program separates instructions by checking for the opcode of the instruction word. When the opcodes are the same, i.e, the instructions are of the same type. in this case we check for the instruction word distinguishing functions and according to that we output the corresponding instructions with their addresses and registers used.

Limitations:
---------------------------------
The labels/functions are not printed with the instructions.
We were not able to test all of the compressed instructions due to the limitations of the venus simulator.

Known Issues:
---------------------------------
There was no issues in the instructions tested. However, some compressed instructions were not tested, and mixing 16-bit and 32-bit instructions was not feasible.

Contribution of Each Team Member:
---------------------------------
Abdelrahman Said: R-type and U-type instructions

Martina Muawad: I-type and J-type instructions.

Mostafa Ibrahim: B-type and S-type instructions.

After writing the 32-bit instructions, the rest of the work was done collectively through zoom.
Implementing the 16-bit instructions and writing the test cases was more convenient to be done together to get the input of all the members of the team.

