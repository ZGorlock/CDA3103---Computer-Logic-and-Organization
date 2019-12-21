//CDA3103-Summer C001
//MySPIM
//Group 10
//Matthew Garcia
//Zachary Gill
//Steven Lukander
//2015/08/07

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
  //perform ALU operation on A and B based on ALUControl
  switch (ALUControl) {
  case 0: //Add - 000
    *ALUresult = A + B;
    break;
  case 1: //Subtract - 001
    *ALUresult = A - B;
    break;
  case 2: case 3: //Set on Less Than / Set on Less Than Unsigned - 010 / 011
    *ALUresult = (A < B);
    break;
  case 4: //Bitwise AND - 100
    *ALUresult = (A & B);
    break;
  case 5: //Bitwise OR - 101
    *ALUresult = (A | B);
    break;
  case 6: //Shift left 16 bits - 110
    *ALUresult = B << 0x10;
    break;
  case 7: //Bitwise NOT - 111
    *ALUresult = ~A;
    break;
  }

  //set Zero output parameter based on value of ALUresult after operations
  if (*ALUresult == 0)
    *Zero = 1;
  else
    *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
  //check for word alignment
  if (PC % 4 != 0) {
    return 1; //halt
  }

  unsigned index = PC >> 2; //get word address instead of byte address

  *instruction = Mem[index]; //write memory to instruction
  return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
  //1111 1100 0000 0000 0000 0000 0000 0000
  *op = (instruction & 0xFC000000) >> 26;   //should shift the first 6 bits to the right most position.

  //0000 0011 1110 0000 0000 0000 0000 0000
  *r1 = (instruction & 0x03E00000) >> 21;

  //0000 0000 0001 1111 0000 0000 0000 0000
  *r2 = (instruction & 0x001F0000) >> 16;

  //0000 0000 0000 0000 1111 1000 0000 0000
  *r3 = (instruction & 0x0000F800) >> 11;

  //0000 0000 0000 0000 0000 0000 0011 1111
  *funct = instruction & 0x0000003F;  //no shift as the number we want is already in the lower order bits

  //0000 0000 0000 0000 1111 1111 1111 1111
  *offset = instruction & 0x0000FFFF;

  //0000 0011 1111 1111 1111 1111 1111 1111
  *jsec = instruction & 0x03FFFFFF;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls)
{
  //if the signal is set to 2 it is a dont care value, the ALUOp signals are from the table in the project guidelines.
  //r-type functions
  if (op == 0){ //000000
    (*controls).RegDst = 1;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 0;
    (*controls).ALUOp = 7; //111
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 0;
    (*controls).RegWrite = 1;
  }
  //Add immediate
  else if (op == 8){ //001000
    (*controls).RegDst = 0;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 0;
    (*controls).ALUOp = 0; //000
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 1;
    (*controls).RegWrite = 1;
  }
  //Load Word
  else if (op == 35){ //100011
    (*controls).RegDst = 0;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 1;
    (*controls).MemtoReg = 1;
    (*controls).ALUOp = 0; //000
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 1;
    (*controls).RegWrite = 1;
  }
  //Store Word
  else if (op == 43){ //101011
    (*controls).RegDst = 2;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 2;
    (*controls).ALUOp = 0; //000
    (*controls).MemWrite = 1;
    (*controls).ALUSrc = 1;
    (*controls).RegWrite = 0;
  }
  //Load Upper immediate
  else if (op == 15){ //001111
    (*controls).RegDst = 0;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 0;
    (*controls).ALUOp = 6; //110
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 1;
    (*controls).RegWrite = 1;
  }
  //Branch on equal
  else if (op == 4){ //000100
    (*controls).RegDst = 0;
    (*controls).Jump = 0;
    (*controls).Branch = 1;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 0;
    (*controls).ALUOp = 1; //001
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 0;
    (*controls).RegWrite = 0;
  }
  // Set less than immdeiate
  else if (op == 10){ //001010    
    (*controls).RegDst = 0;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 0;
    (*controls).ALUOp = 2; //010    
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 1;
    (*controls).RegWrite = 1;
  }
  //Set less than immediate unsigned  
  else if (op == 11){ //001011    
    (*controls).RegDst = 0;
    (*controls).Jump = 0;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 0;
    (*controls).ALUOp = 3; //011    
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 1;
    (*controls).RegWrite = 1;
  }
  //Jump
  else if (op == 2){ //000010
    (*controls).RegDst = 2;
    (*controls).Jump = 1;
    (*controls).Branch = 0;
    (*controls).MemRead = 0;
    (*controls).MemtoReg = 2;
    (*controls).ALUOp = 0; //000
    (*controls).MemWrite = 0;
    (*controls).ALUSrc = 2;
    (*controls).RegWrite = 0;
  }
  else{
    return 1;
  }

  return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
  //store data from Reg at location r1 to data1, same logic for data2
  //r1 = read bits 27-23 / r2 = read bits 22-18
  //r2 only applies if R-type is given but if any other type is given data2 won't be used
  //(Jump/Branch/I Type don't use the rt register for data).
  //Because of that I don't see a reason to add any conditional statements and just let
  // data2 store Reg[r2] no matter the type.
  *data1 = Reg[r1];
  *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
  //offset is a number with the sign bit on the 16th bit
  if (offset >> 15 == 1) //test if the 16th bit is 1
    *extended_value = 0xFFFF0000; //if it is, then the number is negative and needs to be prepended with more 1s to maintain its sign
  else
    *extended_value = 0x00000000; //if not, then the number needs to prepended with 0s

  *extended_value = (*extended_value | offset); //combine the prepended value with the offset value
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
  //Check ALUSrc, if asserted
  //The second ALU operand is the sign-extended lower 16 bits of the instruction
  //if deassrted the second ALU operand comes from second register value
  //return 0 because it shouldn't halt due to any other factors now
  if (ALUSrc == 1)
  {
    data2 = extended_value;
  }

  /* According to the instructions and the table provided we will have 8 operations
  Add/Subtract/Set On Less Than (signed)/ Set On Less Than (unsigned)/ Bitwise And / 		   Bitwise OR/ Shift left logical/ Bitwise Not
  All that needs to be done is determine if we are dealing with an R-type instruction
  and if we are the program will check func to determine the ALU operation code we need to
  send to the ALU function*/

  if (ALUOp == 7) //111 //111 is a place holder based on the table in the instructions. It may
  {		//change when I get to look at the decode function as it will be whatever it
    //is there (Possible 7 but I'm not sure without seeing the code.
    //If you are looking at this and want to change it feel free.
    //funct comes from the instruction_partition function and is the last 6 bits
    //of the instruction.
    // Only applies to R-Type functions
    //Per slide 45 on  Processor pp
    // add 100000/sub 100010 /and 100100/
    //or 100101/slt(signed 101010
    // in hex:  add 0x20/sub 0x22/and 0x24/or 0x25/
    // set on less than signed 0x2A/
    //Also found hex codes for unsigned slt, bitwise NOT (NOR), and shift
    //left logical which are: 0x2B, 0x27, and 0x00 from a chart in the text.
    //Add
    if (funct == 0x20)
    {
      ALUOp = 0; //000 //The ALUOp codes here are per the table on the instructions
    }
    //Subtract
    else if (funct == 0x22)
    {
      ALUOp = 1; //001
    }
    //Set on Less Than
    else if (funct == 0x2A)
    {
      ALUOp = 2; //010
    }
    //Set on Less Than Unsigned
    else if (funct == 0x2B)
    {
      ALUOp = 3; //011
    }
    //Bitwise AND
    else if (funct == 0x24)
    {
      ALUOp = 4; //100
    }
    //Bitwise OR
    else if (funct == 0x25)
    {
      ALUOp = 5; //101
    }
    //Shift left 16 bits
    else if (funct == 0x00)
    {
      ALUOp = 6; //110
    }
    //Bitwise NOT (1s complement)
    else if (funct == 0x27)
    {
      ALUOp = 7; //111
    }
    //Any other values will result in a halt
    else
    {
      return 1;
    }
  }

  //Perform ALU Operation by passing the results results
  ALU(data1, data2, ALUOp, ALUresult, Zero);

  //return 0 to ensure program is not halted
  return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
  if (MemWrite == 1){
    if (ALUresult % 4 == 0) { //test for word alignment
      if ((ALUresult >> 2) < 0 || (ALUresult >> 2) > 16384)
        return 1; //index outside available memory, halt
      else
        Mem[ALUresult >> 2] = data2; //store data2 in Mem at the index specified by ALUresult
    }
    else {
      return 1; //word unalignment, halt
    }
  }

  if (MemRead == 1){
    if (ALUresult % 4 == 0) {//test for word alignment
      if ((ALUresult >> 2) < 0 || (ALUresult >> 2) > 16384)
        return 1; //index outside available memory, halt
      else
        *memdata = Mem[ALUresult >> 2]; //read Mem at the index specified by ALUresult and store in memdata
    }
    else {
      return 1; //word unalignment, halt
    }
  }

  return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
  //Only continue if Regwrite is asserted (referenced slide 14)
  //Also by the same slide MemtoReg determines where the data is coming from
  //MemtoReg = 0 / grab ALUResult
  //MemtoReg = 1 / grab memdata

  if (RegWrite == 1)
  {
    //Essentially if RegDst = 0 we are using r2, if RegDst = 1 we are using r3
    //This is used to determine what type of instruction we are writing for
    //if RegDst is asserted then we are dealing with an r type instruction that utilizes the rd
    //register.  If RegDst is deasserted then we have an i type/jump/branch instruction that
    //will be using rt as a write register or will not have a write register function here.
    if (MemtoReg == 1 && RegDst == 0)
    {
      Reg[r2] = memdata;
    }

    else if (MemtoReg == 0 && RegDst == 0)
    {
      Reg[r2] = ALUresult;
    }

    else if (MemtoReg == 1 && RegDst == 1)
    {
      Reg[r3] = memdata;
    }

    else if (MemtoReg == 0 && RegDst == 1)
    {
      Reg[r3] = ALUresult;
    }
  }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
  //Updates counter by word(words are 4 bits long)
  *PC += 4;

  //Handle Zero and branch instructions
  //Zero and Branch asserted required to branch per data path example
  if (Zero == 1 && Branch == 1)
  {
    //if zero and branch are asserted PC counter updated by
    // current PC + extended_value shifted left 8 bits
    /*EX: Just a quick example of the arithmatic involved, not an accurate instruction
    current PC = 00004004 = 0000 0000 0000 0000 0100 0000 0000 0100
    extended_value = 0000 0000 0000 0000 1111 1111 1111 1111
    extended_value <<2 = 0000 0000 1111 1111 1111 1111 0000 0000
    *PC += extended_value << 2
    0000 0000 0000 0000 0100 0000 0000 0100
    +0000 0000 1111 1111 1111 1111 0000 0000
    0000 0001 0000 0000 0011 1111 0000 0100
    ^^assuming this is the value of PC we have branched out too*/

    *PC += extended_value << 2;
  }

  //Handle Jump
  if (Jump == 1)
  {
    //If Jump is asserted then update PC counter
    // shift jsec 2 bytes and mask the current PC Counter so that
    //the most significant 4 bits are preserved then bitwise OR
    // leaving us with the resulting PC counter as
    // the target jump pc value
    /*EX: just another quick example of the arithmatic involved, not an actual instruction
    *PC = 11111111111111111111111111111111
    jsec = 11110101010101010101010101010101
    shift jsec by 8 = 01010101010101010101010100000000
    *PC & 0xf0000000 = 11110000000000000000000000000000 <- Bitwise AND*/

    /**PC = (jsec << 2) | (*PC & 0xf0000000) =
    01010101010101010101010100000000
    11110000000000000000000000000000
    Final result = 11110101010101010101010100000000*/

    *PC = (jsec << 2) | (*PC & 0xf0000000);
  }
}
