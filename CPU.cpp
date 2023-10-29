#include "CPU.h"

instruction::instruction(bitset<32> fetch)
{
	instr = fetch;
}

CPU::CPU()
{
	PC = 0;						   // set PC to 0
	for (int i = 0; i < 4096; i++) // copy instrMEM
	{
		dmemory[i] = std::bitset<8>("00000000");
	}
	// Hardcode 0
	reg_map[0] = 0;
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

int parseTwosComplement(const std::string &binaryString, int startBit, int numBits)
{
	if (binaryString[startBit] == '1')
	{
		// It's a negative number
		int value = 0;
		for (int i = 0; i < numBits; i++)
		{
			if (binaryString[startBit + i] == '0')
			{
				value |= (1 << (numBits - 1 - i));
			}
		}
		// Convert to negative by taking the two's complement
		return -(value + 1);
	}
	else
	{
		// It's a positive number
		return std::stoi(binaryString.substr(startBit, numBits), nullptr, 2);
	}
}

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
bool flags::getBranch() const { return branch; }
bool flags::getSaveRegister() const { return saveRegister; };

// Setter methods
void flags::setMemRead(bool value) { memRead = value; }
void flags::setMemtoReg(bool value) { MemtoReg = value; }
void flags::setALUOp(operations value) { ALUOp = value; }
void flags::setMemWrite(bool value) { memWrite = value; }
void flags::setRegWrite(bool value) { regWrite = value; }
void flags::setALUSrc(bool value) { ALUSrc = value; }
void flags::setBranch(bool value) { branch = value; }
void flags::setSaveRegister(bool value) { saveRegister = value; }

void CPU::resetFlags()
{
	CPUflags.setMemRead(0);
	CPUflags.setMemtoReg(0);
	CPUflags.setALUOp(ADD);
	CPUflags.setMemWrite(0);
	CPUflags.setRegWrite(0);
	CPUflags.setALUSrc(0);
	CPUflags.setBranch(0);
	CPUflags.setSaveRegister(0);
}

#pragma endregion flag
// Setup Flags from Instruction
void CPU::Controller(instruction *curr)
{
#define R std::bitset<7>("0110011")
#define I std::bitset<7>("0010011")
#define STORE std::bitset<7>("0100011")
#define LOAD std::bitset<7>("0000011")
#define JALR std::bitset<7>("1100111")
#define BLT std::bitset<7>("1100011")
	// Determine type of instruction
	bitset<7> opcode(curr->instr.to_string().substr(25, 7));

	// Switch-esque statement (can't use switch with bitset)
	if (opcode == R)
	{
		// cout << "it's an R type " << endl;
		// Find Registers
		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg2 = parseTwosComplement(curr->instr.to_string(), 7, 5);
		reg3 = parseTwosComplement(curr->instr.to_string(), 20, 5);

		// set proper flags for R type
		bitset<7> func7(curr->instr.to_string().substr(0, 7));
		bitset<3> func3(curr->instr.to_string().substr(17, 3));

		if (func7 == std::bitset<7>("0000000"))
		{
			if (func3 == std::bitset<3>("100"))
			{
				CPUflags.setALUOp(XOR);
			}
		}
		else if (func7 == std::bitset<7>("0100000"))
		{
			if (func3 == std::bitset<3>("000"))
			{
				CPUflags.setALUOp(SUB);
			}
			else
			{
				CPUflags.setALUOp(SRA);
			}
		}
	}
	else if (opcode == I)
	{
		// Find Register
		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg3 = parseTwosComplement(curr->instr.to_string(), 20, 5);

		// Find Intermediate
		intermediate = parseTwosComplement(curr->instr.to_string(), 0, 12);
		// Setup Proper flags for I type
		CPUflags.setALUSrc(1);
		bitset<3> func3(curr->instr.to_string().substr(17, 3));
		if (func3 == std::bitset<3>("111"))
		{
			CPUflags.setALUOp(AND);
		}
	}
	else if (opcode == STORE)
	{
		// Find Register

		// offset
		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		// value stored
		reg2 = parseTwosComplement(curr->instr.to_string(), 7, 5);
		// Find Intermediate
		string intermediateString = curr->instr.to_string().substr(0, 7) + curr->instr.to_string().substr(20, 5);
		intermediate = parseTwosComplement(intermediateString, 0, 12);

		// Set flags
		CPUflags.setMemWrite(1);
	}
	else if (opcode == LOAD)
	{

		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg3 = parseTwosComplement(curr->instr.to_string(), 20, 5);

		intermediate = parseTwosComplement(curr->instr.to_string(), 0, 12);

		CPUflags.setMemRead(1);
	}
	else if (opcode == JALR)
	{

		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg3 = parseTwosComplement(curr->instr.to_string(), 20, 5);

		intermediate = parseTwosComplement(curr->instr.to_string(), 0, 12);

		CPUflags.setBranch(1);
		CPUflags.setALUSrc(1);
		CPUflags.setSaveRegister(1);
	}
	else if (opcode == BLT)
	{
		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg2 = parseTwosComplement(curr->instr.to_string(), 7, 5);

		string intermediateSplice = curr->instr.to_string().substr(0, 1) + curr->instr.to_string().substr(24, 1) + curr->instr.to_string().substr(1, 6) + curr->instr.to_string().substr(20, 4) + curr->instr.to_string().substr(7, 1);

		intermediate = parseTwosComplement(intermediateSplice, 0, 12) << 1;

		CPUflags.setBranch(1);
	}
}

int CPU::ALU()
{
	int val1 = reg_map[reg1];
	int val2;
	int result;
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
	operations op = CPUflags.getALUOp();

	switch (op)
	{
	case ADD:
		result = val1 + val2;
		break;
	case SUB:
		result = val1 - val2;
		break;
	case XOR:
		result = val1 ^ val2;
		break;
	case SRA:
		result = val1 >> val2;
		break;
	case AND:
		result = val1 & val2;
		break;
	default:
		break;
	}
	return result;
}

void CPU::Memory(int result)
{
	if (!CPUflags.getMemWrite() && !CPUflags.getMemRead() && !CPUflags.getBranch())
	{
		// ALU outputs to reg

		reg_map[reg3] = result;
	}
	else
	{
		// Load
		if (CPUflags.getMemRead())
		{
			std::bitset<8> bytes[4];
			for (int i = 0; i < 4; i++)
			{
				bytes[i] = dmemory[reg_map[reg1] + intermediate + i];
			}

			std::bitset<32> concatenated(0);
			for (int i = 0; i < 4; i++)
			{
				concatenated |= (std::bitset<32>(bytes[i].to_ulong()) << (i * 8));
			}

			reg_map[reg3] = concatenated.to_ulong();
		}
		// Store
		else if (CPUflags.getMemWrite())
		{
			for (int i = 0; i < 4; i++)
			{
				dmemory[reg_map[reg1] + intermediate + i] = (reg_map[reg2] >> (8 * i) & 0xFF);
			}

		}
		else if (CPUflags.getBranch() && CPUflags.getSaveRegister())
		{
			reg_map[reg3] = PC;
			PC = reg_map[reg1] + intermediate;
		}
		else if (CPUflags.getBranch() && !CPUflags.getSaveRegister())
		{
			if (reg_map[reg1] < reg_map[reg3])
			{
				PC = PC + intermediate - 4;
			}
		}
	}
}