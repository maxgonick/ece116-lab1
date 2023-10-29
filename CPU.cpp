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

// Setter methods
void flags::setMemRead(bool value) { memRead = value; }
void flags::setMemtoReg(bool value) { MemtoReg = value; }
void flags::setALUOp(operations value) { ALUOp = value; }
void flags::setMemWrite(bool value) { memWrite = value; }
void flags::setRegWrite(bool value) { regWrite = value; }
void flags::setALUSrc(bool value) { ALUSrc = value; }

void CPU::resetFlags()
{
	CPUflags.setMemRead(0);
	CPUflags.setMemtoReg(0);
	CPUflags.setALUOp(ADD);
	CPUflags.setMemWrite(0);
	CPUflags.setRegWrite(0);
	CPUflags.setALUSrc(0);
}

#pragma endregion flag
// Setup Flags from Instruction
void CPU::Controller(instruction *curr)
{
#define R std::bitset<7>("0110011")
#define I std::bitset<7>("0010011")
#define STORE std::bitset<7>("0100011")
#define LOAD std::bitset<7>("0000011")
	// Determine type of instruction
	bitset<7> opcode(curr->instr.to_string().substr(25, 7));

	// Switch-esque statement (can't use switch with bitset)
	if (opcode == R)
	{
		cout << "it's an R type " << endl;
		// Find Registers
		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg2 = parseTwosComplement(curr->instr.to_string(), 7, 5);
		reg3 = parseTwosComplement(curr->instr.to_string(), 20, 5);

		cout << "reg 1: " << reg1 << " reg 2: " << reg2 << " reg 3: " << reg3 << endl;
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
		cout << "it's an I type " << endl;
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

		cout << "reg 1: " << reg1 << " intermediate: " << intermediate << endl;
	}
	else if (opcode == STORE)
	{
		cout << "it's a STORE WORD" << endl;
		// Find Register

		// offset
		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		// value stored
		reg2 = parseTwosComplement(curr->instr.to_string(), 7, 5);
		// Find Intermediate
		string intermediateString = curr->instr.to_string().substr(0, 7) + curr->instr.to_string().substr(20, 5);
		intermediate = parseTwosComplement(intermediateString, 0, 12);
		cout << "INTERMEDIATE IS " << intermediate << endl;

		// Set flags
		CPUflags.setMemWrite(1);
	}
	else if (opcode == LOAD)
	{
		cout << "it's a LOAD word" << endl;

		reg1 = parseTwosComplement(curr->instr.to_string(), 12, 5);
		reg3 = parseTwosComplement(curr->instr.to_string(), 20, 5);

		intermediate = parseTwosComplement(curr->instr.to_string(), 0, 12);

		CPUflags.setMemRead(1);
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
	cout << "val1: " << val1 << " val2: " << val2 << " operation: " << CPUflags.getALUOp() << endl;
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
	default:
		cout << "wtf happened" << endl;
	}
	return result;
}

int CPU::Memory(int result)
{
	if (!CPUflags.getMemWrite() && !CPUflags.getMemRead())
	{
		// ALU outputs to reg

		cout << "Setting Register: " << reg3 << " to " << result << endl;

		reg_map[reg3] = result;
	}
	else
	{
		// Load
		if (CPUflags.getMemRead())
		{
			cout << "START AT " << reg_map[reg1] + intermediate << endl;
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

			cout << concatenated.to_ulong() << endl;
			reg_map[reg3] = concatenated.to_ulong();
		}
		// Store
		else if (CPUflags.getMemWrite())
		{
			cout << "START AT " << reg_map[reg1] + intermediate << endl;
			for (int i = 0; i < 4; i++)
			{
				dmemory[reg_map[reg1] + intermediate + i] = (reg_map[reg2] >> (8 * i) & 0xFF);
			}

			for (int i = 0; i < 4; i++)
			{
				std::cout << "Byte " << (reg_map[reg1] + intermediate + i) << ": " << dmemory[reg_map[reg1] + intermediate + i] << std::endl;
			}
		}

		else
		{
			cout << "Messed up";
		}
	}
}