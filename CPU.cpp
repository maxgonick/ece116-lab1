#include "CPU.h"

instruction::instruction(bitset<32> fetch)
{
	// cout << fetch << endl;
	instr = fetch;
	// cout << instr.to_string().substr(25,7) << endl;
}

CPU::CPU()
{
	PC = 0;						   // set PC to 0
	for (int i = 0; i < 4096; i++) // copy instrMEM
	{
		dmemory[i] = (0);
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem)
{
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong())); // get 32 bit instruction
	PC += 4;																																								 // increment PC
	return instr;
}

bool CPU::Decode(instruction *curr)
{
	bitset<7> opcode(curr->instr.to_string().substr(25, 7));
	if (opcode == std::bitset<7>("0000000"))
		return false;
	return true;
}

unsigned long CPU::readPC()
{
	return PC;
}

// Add other functions here ...

// Flag Functions
#pragma region flag
// Constructor
flags::flags()
{
	// Initialize your flags here if needed
	memRead = false;
	MemtoReg = false;
	ALUOp = ADD;
	memWrite = false;
	regWrite = false;
	ALUSrc = false;
}
// Getter methods
bool flags::getMemRead() const { return memRead; }
bool flags::getMemtoReg() const { return MemtoReg; }
operations flags::getALUOp() const { return ALUOp; }
bool flags::getMemWrite() const { return memWrite; }
bool flags::getRegWrite() const { return regWrite; }
bool flags::getALUSrc() const { return ALUSrc; }

// Setter methods
void flags::setMemRead(bool value) { memRead = value; }
void flags::setMemtoReg(bool value) { MemtoReg = value; }
void flags::setALUOp(operations value) { ALUOp = value; }
void flags::setMemWrite(bool value) { memWrite = value; }
void flags::setRegWrite(bool value) { regWrite = value; }
void flags::setALUSrc(bool value) { ALUSrc = value; }

#pragma endregion flag
// Setup Flags from Instruction
void CPU::Controller(instruction *curr)
{
#define R std::bitset<7>("0110011")
#define I std::bitset<7>("0010011")

	// Determine type of instruction
	bitset<7> opcode(curr->instr.to_string().substr(25, 7));

	// Switch-esque statement (can't use switch with bitset)
	if (opcode == R)
	{
		cout << "it's an R type " << opcode << endl;
		// Find Registers
		reg1 = std::stoi(curr->instr.to_string().substr(12, 5), nullptr, 2);
		// set proper flags for R type
		//  CPUflags.
	}
	else if (opcode == I)
	{
		cout << "it's an I type " << opcode << endl;
		// Find Register
		reg1 = std::stoi(curr->instr.to_string().substr(12, 5), nullptr, 2);
		// Find Intermediate
		intermediate = std::stoi(curr->instr.to_string().substr(0, 12), nullptr, 2);
	}
	else
	{
		cout << "Terminate" << opcode << endl;
	}
}

int CPU::ALU()
{
	int val1 = reg_map[reg1];
	int val2;
	// Multiplexer Intermediate
	if (CPUflags.getALUSrc() == 1)
	{
		val2 = intermediate;
	}
	// Multiplexer Register
	else
	{
		val2 = reg_map[reg2];
	}

	// Choose Operation
}
