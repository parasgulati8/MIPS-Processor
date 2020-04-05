<h1>Pipeline MIPS Processor</h1>
In this project, we  implemented an cycle-accurate simulator for a 5-stage pipelined MIPS processor in C++. The simulator supports a subset of the MIPS instruction set and models the execution of each instruction with cycle accuracy.
The MIPS program is provided to the simulator as a text file “imem.txt” file which is used to initialize the Instruction Memory. Each line of the file corresponds to a Byte stored in the Instruction Memory in binary format, with the first line at address 0, the next line at address 1 and so on. Four contiguous lines correspond to a whole instruction. Note that the words stored in memory are in “Big-Endian” format, meaning that the most significant byte is stored first. 
The Data Memory is initialized using the “dmem.txt” file. The format of the stored words is the same as the Instruction Memory. As with the instruction memory, the data memory addresses also begin at 0 and increment by one in each line. 

Instructions.txt is just to understand the sequence of instruction in MIPS program contained in imem.txt.
The instructions that the simulator supports and their encodings are shown in Table 1. Note that all instructions, except for “halt”, exist in the MIPS ISA. The MIPS Green Sheet defines the semantics of each instruction. 

Instruction | Format | OpCode (hex) | Funct. (hex) 
------------|--------|--------------|-------------
addu | R-Type (ALU) | 00 | 21 
subu | R-Type (ALU) | 00 | 23 
lw | I-Type (Memory) | 23 | - 
sw | I-Type (Memory) | 2B | - 
beq | I-Type (Control) | 04 | - 
halt | Custom instruction | FF | 

<h2>Pipeline Structure</h2>
MIPS pipeline has the following 5 stages: 
 
1. Fetch (IF): fetches an instruction from instruction memory. Updates PC.  
2. Decode (ID/RF): reads from the register RF and generates control signals required in subsequent stages.
3. Execute (EX): performs an ALU operation.  
4. Memory (MEM): loads or stores a 32-bit word from data memory. 
5. Writeback (WB): writes back data to the RF.

<h2>Dealing with Hazards</h2>
RAW hazards are dealt with using either only forwarding (if possible) or, if not, using stalling + forwarding. You must follow the mechanisms described in Lecture 6 to deal RAW hazards. 

<h2>Simulator Output</h2>
This simulator outputs the values of all flip-flops at the end of each clock cycle. Further, when the simulation terminates, the simulator also outputs the state of the RF and Dmem.
