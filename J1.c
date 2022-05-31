#include <stdio.h>
#include <stdint.h>


//BEGIN


#define uint16 uint16_t
#define int16 int16_t
#define RAM_SIZE 32768

uint16 dataStack[33];
uint16 returnStack[32];
int dataPointer = -1;
int returnPointer = -1;
int programCounter = 0;
uint16 RAM[RAM_SIZE];

enum
{
	JMP    =  0x0000,
	JZ     =  0x2000,
	CALL   =  0x4000,
	ALU    =  0x6000,
	LIT    =  0x8000,
} J1_INSTRUCTION;

enum
{
	LITERAL = 0x7fff,
	TARGET  = 0x1fff,
} J1_DATA;

void executeALU(uint16 instruction)
{
	uint16 q;
	uint16 t;
	uint16 n;
	uint16 r;

	if (dataPointer > 0)
	{
		t = dataStack[dataPointer];
	}

	if (dataPointer > 0)
	{
		n = dataStack[dataPointer - 1];
	}

	if (returnPointer > 0)
	{
		r = returnStack[returnPointer - 1];
	}

	programCounter++;
	uint16 operationCode = (instruction & 0x0f00) >> 8;

	switch (operationCode)
	{
		case 0:
			q = t;
			break;
		case 1:
			q = n;
			break;
		case 2:
			q = t + n;
			break;
		case 3:
			q = t & n;
			break;
		case 4:
			q = t | n;
			break;
		case 5:
			q = t ^ n;
			break;
		case 6:
			q = t;
			break;
		case 7:
			q = (t == n) ? 1 : 0;
			break;
		case 8:
			q = (n < t) ? 1 : 0;
			break; 
		case 9: 
			q = n >> t;
			break;
		case 10:
			q = t - 1;
			break;
		case 11:
			q = returnStack[returnPointer];
			break;
		case 12:
			q = RAM[t];
			break;
		case 13:
			q = n << t;
			break;
		case 14:
			q = dataPointer + 1;
			break;
		case 15:
			q = (n < t) ? 1 : 0; 
			break;
		default: 
			break;
	}
	
	uint16 ds = instruction & 0x0003;
	uint16 rs = (instruction & 0x000c) >> 2;

	switch (ds)
	{
		case 1:
			dataPointer++;
			break;
		case 2:
			dataPointer--;
			break;
		default:
			break;
	}

	switch (rs)
	{
		case 1:
			returnPointer++;
			break;
		case 2:
			returnPointer--;
			break;
		default:
			break;
	}

	if ((instruction & 0x0020) != 0)
	{
		RAM[t] = n;
	}

	if ((instruction & 0x0040) != 0)
	{
		returnStack[returnPointer] = t;
	}

	if ((instruction & 0x0080) != 0)
	{
		dataStack[dataPointer-1] = t;
	}

	if ((instruction & 0x1000) != 0)
	{
		programCounter = returnStack[returnPointer];
	}

	if (dataPointer >= 0)
	{
		dataStack[dataPointer] = q;
	}
}

void execute(uint16 instruction)
{
	uint16 instructionType = instruction & 0xe000;
	uint16 operand = instruction & TARGET;
	switch (instructionType)
	{
		case JMP:
			programCounter = operand;
			break;
		case JZ:
			if (dataStack[dataPointer] == 0)
			{
				programCounter = operand;
			}
			dataPointer--;
			break;
		case CALL:
			returnPointer++;
			returnStack[returnPointer] = programCounter + 1;
			programCounter = operand;
			break;
		case ALU:
			executeALU(operand);
			break;
		case LIT:
			operand = instruction & LITERAL;
			dataPointer++;
			dataStack[dataPointer] = operand;
			programCounter++;
			break;
		default:
			break;
	}
}

void executeProgram()
{
	while (RAM[programCounter] != 0)
	{
		execute(RAM[programCounter]);
		print();
	}
}

void print() 
{
	for (int i = 0; i < 33; i++) printf("%d", dataStack[i]);
	putchar('\n');
}


//END

void main(int argc, const char *argv[])
{
	RAM[0] = 0x4003;     // call 3
	RAM[1] = 0x8008;     // push 8
	RAM[2] = 0x0000;     // halt
	RAM[3] = 0x8001;     // push 1
	RAM[4] = 0x8002;     // push 2
	RAM[5] = 0x8003;     // push 3
	RAM[6] = 0x6146;     // >r
	RAM[7] = 0x6180;     // swap
	RAM[8] = 0x6b89;     // r>
	RAM[9] = 0x7180;     // swap

	executeProgram();
	print();
}