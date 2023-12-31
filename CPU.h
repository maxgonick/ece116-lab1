#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
using namespace std;

class instruction
{
public:
	bitset<32> instr;			   // instruction
	instruction(bitset<32> fetch); // constructor
};

enum operations
{
	ADD,
	SUB,
	XOR,
	SRA,
	AND
};

class flags
{

	bool memRead;
	bool MemtoReg;
	operations ALUOp;
	bool memWrite;
	bool regWrite;
	bool ALUSrc;
	bool branch;
	bool saveRegister;

public:
	flags(); // Constructor
	bool getMemRead() const;
	bool getMemtoReg() const;
	operations getALUOp() const;
	bool getMemWrite() const;
	bool getRegWrite() const;
	bool getALUSrc() const;
	bool getBranch() const;
	bool getSaveRegister() const;
	void setMemRead(bool value);
	void setMemtoReg(bool value);
	void setALUOp(operations value);
	void setMemWrite(bool value);
	void setRegWrite(bool value);
	void setALUSrc(bool value);
	void setBranch(bool value);
	void setSaveRegister(bool value);
};

class CPU
{
private:
	std::bitset<8> dmemory[4096];
	unsigned long PC;
	flags CPUflags;
	int reg1;
	int reg2;
	int reg3;
	int intermediate;

public:
	CPU();
	unsigned long readPC();
	bitset<32> Fetch(bitset<8> *instmem);
	bool Decode(instruction *instr);
	void Controller(instruction *instr);
	int ALU();
	std::map<int, int> reg_map;
	void Memory(int result);
	void resetFlags();

	// Function to print the values of all flags
	void printFlags()
	{
		cout << "memRead: " << CPUflags.getMemRead() << endl;
		cout << "MemtoReg: " << CPUflags.getMemtoReg() << endl;
		cout << "ALUOp: " << CPUflags.getALUOp() << endl;
		cout << "memWrite: " << CPUflags.getMemWrite() << endl;
		cout << "regWrite: " << CPUflags.getRegWrite() << endl;
		cout << "ALUSrc: " << CPUflags.getALUSrc() << endl;
		cout << "Branch: " << CPUflags.getBranch() << endl;
	}
};
