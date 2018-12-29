/*
 * MIPS_pipeline.cpp
 *
 *  Created on: Oct 21, 2018
 *      Author: paras
 */




/*
 * MIPS_pipeline.cpp
 *
 *  Created on: Oct 21, 2018
 *      Author: paras
 */

#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <cstdlib>
using namespace std;
#define MemSize 256 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public:
        bitset<32> Reg_data;
     	RF()
    	{
			Registers.resize(32);
			Registers[0] = bitset<32> (0);
        }

        bitset<32> readRF(bitset<5> Reg_addr)
        {
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }

        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }

		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();
		}

	private:
		vector<bitset<32> >Registers;
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {
			IMem.resize(MemSize);
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{
					IMem[i] = bitset<8>(line);
					i++;
				}
			}
            else cout<<"Unable to open file";
			imem.close();
		}

		bitset<32> readInstr(bitset<32> ReadAddress)
		{
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;
		}

    private:
        vector<bitset<8> > IMem;
};

class DataMem
{
    public:
        bitset<32> ReadData;
        DataMem()
        {
            DMem.resize(MemSize);
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();
        }

        bitset<32> readDataMem(bitset<32> Address)
        {
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;
		}

        void writeDataMem(bitset<32> Address, bitset<32> WriteData)
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));
        }

        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {
                    dmemout << DMem[j]<<endl;
                }

            }
            else cout<<"Unable to open file";
            dmemout.close();
        }

    private:
		vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl;

        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl;

        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;

        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl;
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl;
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl;
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl;
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl;
    }
    else cout<<"Unable to open file";
    printstate.close();
}

unsigned long shiftbits(bitset<32> inst, int start)
{
    return ((inst.to_ulong())>>start);

}

bitset<32> signextend (bitset<16> imm)
{
    string sestring;
    if (imm[15]==0){
        sestring = "0000000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    else{
        sestring = "1111111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    return (bitset<32> (sestring));

}

int main()
{

    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;

    bitset<32> operand1;
    bitset<32> operand2;
    bitset<1> IsBranch =1;
    bitset<1> isStall = 0;
    bitset<32> branchAddress = 0;
	stateStruct state, newState;
    state.IF.PC = bitset<32> (0);
    int cycle = 0;

    newState.IF.nop = 0;
    newState.ID.nop = 1;

    newState.EX.nop = 1;
    newState.EX.alu_op = 1;
    newState.EX.is_I_type = 0;
    newState.EX.rd_mem = 0;
    newState.EX.wrt_mem = 0;
    newState.EX.wrt_enable= 0;
    newState.MEM.nop = 1;
    newState.MEM.rd_mem = 0;
    newState.WB.nop = 1;
    newState.WB.wrt_enable = 0;

    state.IF.nop = 0;
    state.ID.nop = 1;
    state.EX.nop = 1;
    state.MEM.nop = 1;
    state.WB.nop = 1;

    bitset<1> fwdRs, fwdRt;
    while (1) {

        /* --------------------- WB stage --------------------- */
        if(state.WB.nop)
        {

        }
        else
        {

            if(state.WB.wrt_enable)
                {
            		myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);

                }
        }
        /* --------------------- MEM stage --------------------- */

        newState.WB.nop = state.MEM.nop;
        if(state.MEM.nop)
            {

            }
        else
        {
            if(state.MEM.rd_mem)
            {

                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
            }
            else
            {
            	newState.WB.Wrt_data = state.MEM.ALUresult;
            }

            if (state.MEM.wrt_mem)
            {

                if(state.MEM.Rt == state.WB.Wrt_reg_addr)
                {
                	myDataMem.writeDataMem(state.MEM.ALUresult, state.WB.Wrt_data);
                }
                else
                myDataMem.writeDataMem(state.MEM.ALUresult, state.EX.Read_data2);
            }

            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = state.MEM.wrt_enable;
        }


        /* --------------------- EX stage --------------------- */
        newState.MEM.nop = state.EX.nop ;
        if(state.EX.nop)
            {
            }
        else
        {
            // check for forwarding for Rs
            if(state.EX.Rs == state.MEM.Wrt_reg_addr && cycle > 2 && state.MEM.wrt_mem == false)
            {
            	fwdRs = 1;
            	if(state.MEM.rd_mem)
            		{
            			operand1 = newState.WB.Wrt_data;
            		}
            	else if(state.MEM.wrt_mem)
            	{
            		operand1 = state.EX.Read_data1;
            	}
            	else
            		{
            			operand1 = state.MEM.ALUresult;
            		}
            }
            else if(state.EX.Rs == state.WB.Wrt_reg_addr && cycle > 3)
            {
            	fwdRs = 1;
            	operand1 = state.WB.Wrt_data;
            }
            else
            	{
            		operand1 = state.EX.Read_data1;
            	}

            // check for forwarding for Rt
            if(state.EX.Rt == state.MEM.Wrt_reg_addr && cycle > 2)
            	{
            		fwdRt = 1;

           			if(state.MEM.rd_mem)
           			{
           				if(state.MEM.nop)
           					operand2 = state.WB.Wrt_data; //in case of stalling only
           				else
           					operand2 = newState.WB.Wrt_data; // otherwise

           			}
            	}
            else if(state.EX.Rt == state.WB.Wrt_reg_addr && cycle > 3 && !state.WB.nop)
            {
            	fwdRt = 1;
       			operand2 = state.WB.Wrt_data;
            }
            else
            	{
            		operand2 = state.EX.Read_data2;
            	}
            // compute ALU result for I type
            if(state.EX.is_I_type)
            {

            		newState.MEM.ALUresult = bitset<32> (operand1.to_ulong() + signextend(state.EX.Imm).to_ulong());
            }
            else
            {
            	if(state.EX.alu_op)
            	{
            		newState.MEM.ALUresult = bitset<32> (operand1.to_ulong() + operand2.to_ulong());
            	}
            	else
            	{
            		newState.MEM.ALUresult = bitset<32> (operand1.to_ulong() - operand2.to_ulong());
            	}
            }
            //to be defined for sw only
            newState.MEM.Store_data = state.EX.Read_data2 ;

            newState.MEM.Rs  = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.rd_mem = state.EX.rd_mem;

            newState.MEM.wrt_mem = state.EX.wrt_mem;
            newState.MEM.wrt_enable = state.EX.wrt_enable ;
        }
        /* --------------------- ID stage --------------------- */
        newState.EX.nop = state.ID.nop;
        if(state.ID.nop)
            {

            }
        else
        {
            bitset<6> opcode = bitset<6> (shiftbits(state.ID.Instr, 26));
            newState.EX.Rs = bitset<5> (shiftbits(state.ID.Instr, 21));
            newState.EX.Rt = bitset<5> (shiftbits(state.ID.Instr, 16));
            newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
            newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
            newState.EX.Imm = bitset<16> (shiftbits(state.ID.Instr, 0));

            if(newState.EX.Read_data1 == newState.EX.Read_data2 && opcode.to_ulong() == 4)
            {
            	state.ID.nop = 1;
            	newState.EX.nop = 1;
            	IsBranch = 0;

            	branchAddress = signextend(newState.EX.Imm);
            	branchAddress[0] = 0;
            	branchAddress[1] = 0;
            }

            newState.EX.is_I_type = (opcode.to_ulong()!=0 && opcode.to_ulong()!=2)?1:0;

            newState.EX.rd_mem = (opcode.to_ulong() == 35)?1:0;

            newState.EX.wrt_mem = (opcode.to_ulong() == 43)?1:0;

            newState.EX.Wrt_reg_addr = (newState.EX.is_I_type)? newState.EX.Rt : bitset<5> (shiftbits(state.ID.Instr, 11));
            newState.EX.alu_op = (opcode.to_ulong() == 35 || opcode.to_ulong() == 43 || (bitset<3> (shiftbits(state.ID.Instr, 0)).to_ulong() == 1 )) ? 1 : 0;
            newState.EX.wrt_enable = ( !newState.EX.wrt_mem || opcode.to_ulong() ==0 || opcode.to_ulong() == 4)?1:0; // if store word or R type instruction(addu and subu) or branch
            if((state.EX.Wrt_reg_addr == newState.EX.Rs || state.EX.Wrt_reg_addr == newState.EX.Rt) && cycle >2)
            {
            	if(state.EX.rd_mem && !newState.EX.is_I_type)
            		{
            			isStall = 1;
            			newState.ID = state.ID;
            			newState.IF = state.IF;
            			newState.EX.nop = 1;
            		}
            }
        }
        /* --------------------- IF stage --------------------- */
        if(isStall.to_ulong())
        {
        	isStall = 0;
        }
        else
        {
        newState.ID.nop = state.IF.nop;
        if(state.IF.nop)
            {

            }
        else
        {
        	newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
        	if (newState.ID.Instr.to_string<char,std::string::traits_type,std::string::allocator_type>()=="11111111111111111111111111111111")
        	{
        	            newState.IF.nop = 1;
        	            newState.ID.nop = 1;
           	}
        	else if(IsBranch.to_ulong())
        		newState.IF.PC = bitset<32> (state.IF.PC.to_ulong() + 4);
        	else
        		//effective branch address
        		newState.IF.PC = bitset<32> (state.IF.PC.to_ulong() + 4 + branchAddress.to_ulong());
        }
        }
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            {
                break;
            }
        printState(newState, cycle++); //print states after executing cycle 0, cycle 1, cycle 2 ...
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */
    }

    myRF.outputRF(); // dump RF;
	myDataMem.outputDataMem(); // dump data mem

	return 0;
}
