#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <type_traits>

//https://en.wikibooks.org/wiki/MIPS_Assembly/Instruction_Formats

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define NUM_INSTRUCTIONS 32
#define NUM_REGISTERS 32

struct ICstats {
	int ops;
	int loads;
	int jumps;
};

struct memAccess {
	int fetches;
	int loads;
	int stores;
};

struct xferControl {
	int jumps;
	int jumpsLinks;
	int takenBranches;
	int untakenBranches;
};

std::vector<int> instructionList;
int branch = 0;
bool halt = false;
int usedWordNumber = 0;
int wordNumber = 0;
int rNumber = 0;
int word[NUM_REGISTERS];
int r[NUM_REGISTERS];
int pc = 0;
ICstats ic;
memAccess mem;
xferControl xfer;

//i
void addiu(const unsigned int passedValue) {
	ic.ops++;
	int maskimm = 65535;
	int imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	

	r[rt] = r[rs] + imm;

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	std::cout << "addiu	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	pc++;
}

void sw(const unsigned int passedValue) {
	mem.stores++;
	ic.loads++;
	
	int maskimm = 65535;
	int imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	int test = r[rs] + imm;
	instructionList[/*r[rs] +*/ imm] = r[rt];

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	std::cout << "sw	- register r[" << rt << "] value stored in memory\n";
	pc++;
}

void beq(const unsigned int passedValue) {
	//test if taken
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;


	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	std::cout << "beq	- ";

	if (r[rs] == r[rt]) {
		pc += imm;
		xfer.takenBranches++;
		std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << imm << "\n";
	}
	else {
		xfer.untakenBranches++;
		std::cout << std::setw(8) << "branch untaken\n";
	}
}

void bgtz(const unsigned int passedValue) {
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	pc++;
	ic.jumps++;
	std::cout << "blgtz	- ";
	if (r[rs] > 0) {
		pc += imm;
		branch = pc - 1;
		xfer.takenBranches++;
		std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << pc << "\n";
	}
	else {
		xfer.untakenBranches++;
		std::cout << std::setw(8) << "branch untaken\n";
	}
}
//i
void blez(const unsigned int passedValue) {
	//test if taken
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	
	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	pc++;
	ic.jumps++;
	std::cout << "blez	- ";
	if (r[rs] <= 0) {
		pc += imm;
		branch = pc - 1;
		xfer.takenBranches++;
		std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << pc << "\n";
	}
	else {
		xfer.untakenBranches++;
		std::cout << std::setw(8) << "branch untaken\n";
	}
}

void bne(const unsigned int passedValue) {
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;


	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	std::cout << "beq	- ";

	if (r[rs] != r[rt]) {
		pc += imm;
		xfer.takenBranches++;
		std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << imm << "\n";
	}
	else {
		xfer.untakenBranches++;
		std::cout << std::setw(8) << "branch untaken\n";
	}
}
//j
void j(const unsigned int passedValue) {
	xfer.jumps++;
	ic.jumps++;
	int jumpVal = passedValue;
	//get rid of opcode
	jumpVal = jumpVal << 6;
	jumpVal = jumpVal >> 6;

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
			  << std::dec
			  << std::showbase
			  << std::internal;

	std::cout << std::setw(8) << "j	- jump to " << std::setfill('0') << std::setw(8) << jumpVal << "\n";
	pc+=jumpVal;
	branch = jumpVal-1;
}

void jal(const unsigned int passedValue) {
	xfer.jumpsLinks++;
	xfer.jumps++;
	ic.jumps++;
}

void lui(const unsigned int passedValue) {
	ic.ops++;
}

void lw(const unsigned int passedValue) {
	ic.loads++;
	mem.loads++;
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	r[rt] = instructionList[r[rs] + imm];

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;

	std::cout << "lw	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	pc++;
}

void mul(const unsigned int passedValue) {
	ic.ops++;
}

void slti(const unsigned int passedValue) {
	ic.ops++;
}

void xori(const unsigned int passedValue) {
	ic.ops++;
}

/* ALU FUNCTIONS */
void addu(const unsigned int passedValue) {
	
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] + r[rt];

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	std::cout << "addu	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	pc++;

}

void and(const unsigned int passedValue) {
	
}

void jalr(const unsigned int passedValue) {
	
}

void jr(const unsigned int passedValue) {
	
}

void nor(const unsigned int passedValue) {
	
}

void or(const unsigned int passedValue) {
	
}

void sll(const unsigned int passedValue) {
	
}

void sra(const unsigned int passedValue) {

}

void srl(const unsigned int passedValue) {
	
}

//r
void subu(const unsigned int passedValue) {

	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] - r[rt];

	std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
	std::cout << std::setfill('0')
		<< std::dec
		<< std::showbase
		<< std::internal;
	std::cout << "subu	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	pc++;
}

void xor(const unsigned int passedValue) {
	ic.ops++;
}

/****************************************************/

void ALUfunction(const unsigned int passedValue) {
	//decide if op or constant
	//deal with constants later
	int test = passedValue;
	test = test >> 6;
	if (test == 0) {
		word[wordNumber] = passedValue;
		wordNumber++;
		return;
	}
	ic.ops++;
	unsigned int mask = 63;
	unsigned int funct = passedValue & mask;

	switch (funct) {
	case 0x21: addu(passedValue); break;
	case 0x24: and(passedValue); break;
	case 0x09: jalr(passedValue); break;
	case 0x08: jr(passedValue); break;
	case 0x27: nor(passedValue); break;
	case 0x25: or(passedValue); break;
	case 0x00: sll(passedValue); break;
	case 0x03: sra(passedValue); break;
	case 0x02: srl(passedValue); break;
	case 0x23: subu(passedValue); break;
	case 0x26: xor(passedValue); break;
	default: break;
	}

}

void hlt() {
	std::cout << std::setfill('0') << std::setw(3) << pc << ": hlt\n";
}

void functionCall(const unsigned int passedValue) {
	//figure out what kind of function it is
	//get the function opcode
	//send the int to the function's function
	if (passedValue == 0) {
		hlt();
		return;
	}
	unsigned int mask = 63;
	mask = mask << 26;

	unsigned int opcode = passedValue & mask;
	opcode = opcode >> 26;
	mem.fetches++;
	switch(opcode) {
	case 0: ALUfunction(passedValue); break;
	case 0x09: addiu(passedValue); break;
	case 0x04: beq(passedValue); break;
	case 0x07: bgtz(passedValue); break;
	case 0x06: blez(passedValue); break;
	case 0x05: bne(passedValue); break;
	case 0x02: j(passedValue); break;
	case 0x03: jal(passedValue); break;
	case 0x0f: lui(passedValue); break;
	case 0x23: lw(passedValue); break;
	case 0x1c: mul(passedValue); break;
	case 0x0a: slti(passedValue); break;
	case 0x2b: sw(passedValue); break;
	case 0x0e: xori(passedValue); break;
		default: break;
	}
}

int main(void) {

	std::string inputCommand;
	int i = 0;

#ifdef _MSC_VER
	//FILE* inputFile;
	std::ifstream inputFile;
	unsigned int input;
	//inputFile = fopen("input.txt", "r");
	inputFile.open("input.txt");
	//while (fscanf(inputFile, "%s", &inputCommand) != EOF)
	while (inputFile >> std::hex >> input) {
		std::string::size_type sz;
		instructionList.push_back(input);

	}

#else
	//school computers
	while (std::cin >> inputString)
		input += inputString + "\n";
#endif
	//before
	std::cout << "contents of memory\n";
	std::cout << "addr value\n";
	for (int i = 0; i < instructionList.size(); ++i) {
		//printf("%x\n", instructionList[i]);
		std::cout << std::setfill('0') << std::setw(3) << i << ": ";
		std::cout << std::hex << std::setfill('0') << std::setw(8) << instructionList[i] << "\n";
	}

	//go through list of instructions ... perform each action
	std::cout << "behavioral simulation of simple MIPS-like machine\n";
	std::cout << "(all values are shown in hexadecimal)\n";
	std::cout << "pc   result of instruction at that location\n";
	for (int i = 0; i < instructionList.size(); ++i) {
		functionCall(instructionList[i]);
		if (halt) return 0;
		if (branch) {
			i = branch;
			branch = 0;
		}
		
	}

	//after
	std::cout << "contents of memory\n";
	std::cout << "addr value\n";
	for (int i = 0; i < instructionList.size(); ++i) {
		//printf("%x\n", instructionList[i]);
		std::cout << std::setfill('0') << std::setw(3) << i << ": ";
		std::cout << std::hex << std::setfill('0') << std::setw(8) << instructionList[i] << "\n";
	}


	/* Report Results */
	std::cout << std::dec << "instruction class counts(omits hlt instruction)\n";
		std::cout << "ops " << ic.ops << "\n";
		std::cout << "loads / stores " << ic.loads << "\n";
		std::cout << "jumps/ branches " << ic.jumps << "\n";
		std::cout << "total " << ic.ops + ic.loads + ic.jumps << "\n";

	std::cout << "memory access counts(omits hlt instruction)\n";
		std::cout << "inst.fetches " << mem.fetches << "\n";
		std::cout << "loads " << mem.loads << "\n";
		std::cout << "stores " << mem.stores << "\n";;
		std::cout << "total " << mem.fetches + mem.loads + mem.stores << "\n";

	std::cout << "transfer of control counts\n";
		std::cout << "jumps " << xfer.jumps << "\n";
		std::cout << "jump - and-links " << xfer.jumpsLinks << "\n";;
		std::cout << "taken branches " << xfer.takenBranches << "\n";;
		std::cout << "untaken branches " << xfer.untakenBranches << "\n";;
		std::cout << "total " << xfer.jumps + xfer.jumpsLinks + xfer.takenBranches + xfer.untakenBranches << "\n";

		int x;
		std::cin >> x;
}