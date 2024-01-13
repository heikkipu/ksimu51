/**************************************************************************
*   Copyright (C) 2008 - ... Heikki Pulkkinen                             *
*   heikki@CompaQ7113EA                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "InstructionSet.h"

Instruction::Instruction()
{
	name_="noName";
	mnemonic_="noMnemo";
	machineCycles_=-1;
}

Instruction::~Instruction()
{
}

void Instruction::setInstruction(char const *instName, char const *instMnemonic, I8031_NameS::machineCycles_Type const numCycles)
{
	name_=instName;
	mnemonic_=instMnemonic;
	machineCycles_=numCycles;
}

I8031_NameS::machineCycles_Type Instruction::readMachineCycles(void)
{
	return machineCycles_;
}

void Instruction::setMachineCycles(I8031_NameS::machineCycles_Type const mCycles)
{
	machineCycles_=mCycles;
}

string Instruction::readName(void)
{
	return name_;
}

string Instruction::readMnemonic(void)
{
	return mnemonic_;
}

//Main
InstructionSet::InstructionSet()
{
	createInstructions();
}

InstructionSet::~InstructionSet()
{
}

bool InstructionSet::isInstruction(char const *instStr)
{
	if(instStr!=0x0)
		for(unsigned int n=0; n<INSTRUCTION_TABLE_SIZE; n++)
			if(instructionTable_[n].readName()==instStr)
				return true;
	return false;
}

void InstructionSet::setNewMachineCycles(char const *mnemonicStr, int const numCycles)
{
	if(mnemonicStr!=0x0)
		for(unsigned int n=0; n<INSTRUCTION_TABLE_SIZE; n++)
			if(instructionTable_[n].readMnemonic()==mnemonicStr)
				instructionTable_[n].setMachineCycles(numCycles);
}

I8031_NameS::machineCycles_Type InstructionSet::readMachineCycle(unsigned int const instructionValue)
{
	if(instructionValue<INSTRUCTION_TABLE_SIZE)
		return instructionTable_[instructionValue].readMachineCycles();
	return -1;
}

void InstructionSet::createInstructions(void)
{
	using namespace InstructionSet_NameS;
	//Luodaan käskyt
	//ADD A,Rn  0010  1rrr
	instructionTable_[ADD_A_RN0].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN1].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN2].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN3].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN4].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN5].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN6].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	instructionTable_[ADD_A_RN7].setInstruction(STR_ADD, STR_add_A_RNx, 1);
	//ADD A,direct 0010 0101 + 8b direct address
	instructionTable_[ADD_A_DIRECT].setInstruction(STR_ADD, STR_add_A_DIRECT, 1);
	//ADD A,@RI 0010 011i
	instructionTable_[ADD_A_RI0].setInstruction(STR_ADD, STR_add_A_RIx, 1);
	instructionTable_[ADD_A_RI1].setInstruction(STR_ADD, STR_add_A_RIx, 1);
	//ADD A,#data
	instructionTable_[ADD_A_DATA].setInstruction(STR_ADD, STR_add_A_DATA,1);
	//ADDC A,Rn 0011 1rrr
	instructionTable_[ADDC_A_RN0].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN1].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN2].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN3].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN4].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN5].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN6].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	instructionTable_[ADDC_A_RN7].setInstruction(STR_ADDC, STR_addc_A_RNx, 1);
	//ADDC A,direct 0010 0101
	instructionTable_[ADDC_A_DIRECT].setInstruction(STR_ADDC, STR_addc_A_DIRECT, 1);
	//ADDC A,@RI 0010 011i
	instructionTable_[ADDC_A_RI0].setInstruction(STR_ADDC, STR_addc_A_RIx, 1);
	instructionTable_[ADDC_A_RI1].setInstruction(STR_ADDC, STR_addc_A_RIx, 1);
	//ADDC A,#data
	instructionTable_[ADDC_A_DATA].setInstruction(STR_ADDC, STR_addc_A_DATA, 1);
	//SUBB A,Rn 1001 1rrr
	instructionTable_[SUBB_A_RN0].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN1].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN2].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN3].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN4].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN5].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN6].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	instructionTable_[SUBB_A_RN7].setInstruction(STR_SUBB, STR_subb_A_RNx, 1);
	//SUBB A,direct 1001 0101
	instructionTable_[SUBB_A_DIRECT].setInstruction(STR_SUBB, STR_subb_A_DIRECT, 1);
	//SUBB A,@Ri 1001 011i
	instructionTable_[SUBB_A_RI0].setInstruction(STR_SUBB, STR_subb_A_RIx, 1);
	instructionTable_[SUBB_A_RI1].setInstruction(STR_SUBB, STR_subb_A_RIx, 1);
	//SUBB A,#data 1001 0100
	instructionTable_[SUBB_A_DATA].setInstruction(STR_SUBB, STR_subb_A_DATA, 1);
	//INC A 0000 0100
	instructionTable_[INC_A].setInstruction(STR_INC, STR_inc_A, 1);
	//INC Rn 0000 1rrr
	instructionTable_[INC_RN0].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN1].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN2].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN3].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN4].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN5].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN6].setInstruction(STR_INC, STR_inc_RNx, 1);
	instructionTable_[INC_RN7].setInstruction(STR_INC, STR_inc_RNx, 1);
	//INC direct 0000 0101
	instructionTable_[INC_DIRECT].setInstruction(STR_INC, STR_inc_DIRECT, 1);
	//INC @RI 0000 011i
	instructionTable_[INC_RI0].setInstruction(STR_INC, STR_inc_RIx, 1);
	instructionTable_[INC_RI1].setInstruction(STR_INC, STR_inc_RIx, 1);
	//INC DPTR 1010 0011
	instructionTable_[INC_DPTR].setInstruction(STR_INC, STR_inc_DPTR, 2);
	//DEC A 0001 0100
	instructionTable_[DEC_A].setInstruction(STR_DEC, STR_dec_A, 1);
	//DEC Rn 0001 1rrr
	instructionTable_[DEC_RN0].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN1].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN2].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN3].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN4].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN5].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN6].setInstruction(STR_DEC, STR_dec_RNx, 1);
	instructionTable_[DEC_RN7].setInstruction(STR_DEC, STR_dec_RNx, 1);
	//DEC direct 0001 0101
	instructionTable_[DEC_DIRECT].setInstruction(STR_DEC, STR_dec_DIRECT, 1);
	//DEC @RI 0001 011 i
	instructionTable_[DEC_RI0].setInstruction(STR_DEC, STR_dec_RIx, 1);
	instructionTable_[DEC_RI1].setInstruction(STR_DEC, STR_dec_RIx, 1);
	//MUL AB 1010 0100
	instructionTable_[MUL_AB].setInstruction(STR_MUL, STR_mul_AB, 4);
	//DIV AB 1000 0100
	instructionTable_[DIV_AB].setInstruction(STR_DIV, STR_div_AB, 4);
	//DA A 1101 0100
	instructionTable_[DA_A].setInstruction(STR_DA, STR_da_A, 1);
	//ANL A,Rn 0101 1rrr
	instructionTable_[ANL_A_RN0].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN1].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN2].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN3].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN4].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN5].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN6].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	instructionTable_[ANL_A_RN7].setInstruction(STR_ANL, STR_anl_A_RNx, 1);
	//ANL A,direct
	instructionTable_[ANL_A_DIRECT].setInstruction(STR_ANL, STR_anl_A_DIRECT, 1);
	//ANL A,@Ri
	instructionTable_[ANL_A_RI0].setInstruction(STR_ANL, STR_anl_A_RIx, 1);
	instructionTable_[ANL_A_RI1].setInstruction(STR_ANL, STR_anl_A_RIx, 1);
	//ANL A,#data
	instructionTable_[ANL_A_DATA].setInstruction(STR_ANL, STR_anl_A_DATA, 1);
	//ANL direct,A
	instructionTable_[ANL_DIRECT_A].setInstruction(STR_ANL, STR_anl_DIRECT_A, 1);
	//ANL direct,#data
	instructionTable_[ANL_DIRECT_DATA].setInstruction(STR_ANL, STR_anl_DIRECT_DATA, 2);
	//ANL C,bit 1000 0010
	instructionTable_[ANL_C_BIT].setInstruction(STR_ANL, STR_anl_C_BIT, 2);
	//Anl C,/bit 1011 0000
	instructionTable_[ANL_C_NBIT].setInstruction(STR_ANL, STR_anl_C_NBIT, 2);
	//ORL A,Rn 0100 1rrr
	instructionTable_[ORL_A_RN0].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN1].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN2].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN3].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN4].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN5].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN6].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	instructionTable_[ORL_A_RN7].setInstruction(STR_ORL, STR_orl_A_RNx, 1);
	//ORL A,direct 0100 0101
	instructionTable_[ORL_A_DIRECT].setInstruction(STR_ORL, STR_orl_A_DIRECT, 1);
	//ORl A,@Ri 0100 011i
	instructionTable_[ORL_A_RI0].setInstruction(STR_ORL, STR_orl_A_RIx, 1);
	instructionTable_[ORL_A_RI1].setInstruction(STR_ORL, STR_orl_A_RIx, 1);
	//ORL A,#data 0100 0100
	instructionTable_[ORL_A_DATA].setInstruction(STR_ORL, STR_orl_A_DATA, 1);
	//ORL direct,A 0100 0010
	instructionTable_[ORL_DIRECT_A].setInstruction(STR_ORL, STR_orl_DIRECT_A, 1);
	//ORL direct,#data 0100 0011
	instructionTable_[ORL_DIRECT_DATA].setInstruction(STR_ORL, STR_orl_DIRECT_DATA, 2);
	//ORL C,bit 0111 0010
	instructionTable_[ORL_C_BIT].setInstruction(STR_ORL, STR_orl_C_BIT, 2);
	//ORL C,/bit 1010 0000
	instructionTable_[ORL_C_NBIT].setInstruction(STR_ORL, STR_orl_C_NBIT, 2);
	//XRL A,Rn 0110 1rrr
	instructionTable_[XRL_A_RN0].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN1].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN2].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN3].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN4].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN5].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN6].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	instructionTable_[XRL_A_RN7].setInstruction(STR_XRL, STR_xrl_A_RNx, 1);
	//XRL A,direct 0110 0101
	instructionTable_[XRL_A_DIRECT].setInstruction(STR_XRL, STR_xrl_A_DIRECT, 1);
	//XRL A,@Ri 0110 011i
	instructionTable_[XRL_A_RI0].setInstruction(STR_XRL, STR_xrl_A_RIx, 1);
	instructionTable_[XRL_A_RI1].setInstruction(STR_XRL, STR_xrl_A_RIx, 1);
	//XRL A,#data 0110 0100
	instructionTable_[XRL_A_DATA].setInstruction(STR_XRL, STR_xrl_A_DATA, 1);
	//XRL direct,A 0110 0010
	instructionTable_[XRL_DIRECT_A].setInstruction(STR_XRL, STR_xrl_DIRECT_A, 1);
	//XRL direct,#data 0110 0011
	instructionTable_[XRL_DIRECT_DATA].setInstruction(STR_XRL, STR_xrl_DIRECT_DATA, 2);
	//CLR A 1110 0100
	instructionTable_[CLR_A].setInstruction(STR_CLR, STR_clr_A, 1);
	//CLR C 1100 0011
	instructionTable_[CLR_C].setInstruction(STR_CLR, STR_clr_C, 1);
	//CLR bit 1100 0010
	instructionTable_[CLR_BIT].setInstruction(STR_CLR, STR_clr_BIT, 1);
	//CPL A 1111 0100
	instructionTable_[CPL_A].setInstruction(STR_CPL, STR_cpl_A, 1);
	//CPL C 1011 0011
	instructionTable_[CPL_C].setInstruction(STR_CPL, STR_cpl_C, 1);
	//CPL bit
	instructionTable_[CPL_BIT].setInstruction(STR_CPL, STR_cpl_BIT, 1);
	//RL A 0010 0011
	instructionTable_[RL_A].setInstruction(STR_RL, STR_rl_A, 1);
	//RLC A 0011 0011
	instructionTable_[RLC_A].setInstruction(STR_RLC, STR_rlc_A, 1);
	//RR A 0000 0011
	instructionTable_[RR_A].setInstruction(STR_RR, STR_rr_A, 1);
	//RRC A 0001 0011
	instructionTable_[RRC_A].setInstruction(STR_RRC, STR_rrc_A, 1);
	//SWAP A 1100 0100
	instructionTable_[SWAP_A].setInstruction(STR_SWAP, STR_swap_A, 1);
	//MOV A,Rn 1110 1rrr
	instructionTable_[MOV_A_RN0].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN1].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN2].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN3].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN4].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN5].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN6].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	instructionTable_[MOV_A_RN7].setInstruction(STR_MOV, STR_mov_A_RNx, 1);
	//MOV A,direct 1110 0101
	instructionTable_[MOV_A_DIRECT].setInstruction(STR_MOV, STR_mov_A_DIRECT, 1);
	//MOV A,@Ri 1110 011i
	instructionTable_[MOV_A_RI0].setInstruction(STR_MOV, STR_mov_A_RIx, 1);
	instructionTable_[MOV_A_RI1].setInstruction(STR_MOV, STR_mov_A_RIx, 1);
	//MOV A,#data 0111 0100
	instructionTable_[MOV_A_DATA].setInstruction(STR_MOV, STR_mov_A_DATA, 1);
	//MOV Rn,A 1111 1rrr
	instructionTable_[MOV_RN0_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN1_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN2_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN3_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN4_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN5_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN6_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	instructionTable_[MOV_RN7_A].setInstruction(STR_MOV, STR_mov_RNx_A, 1);
	//MOV Rn,direct 1010 1rrr
	instructionTable_[MOV_RN0_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN1_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN2_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN3_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN4_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN5_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN6_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	instructionTable_[MOV_RN7_DIRECT].setInstruction(STR_MOV, STR_mov_RNx_DIRECT, 1);
	//MOV Rn,#data 0111 1rrr
	instructionTable_[MOV_RN0_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN1_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN2_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN3_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN4_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN5_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN6_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	instructionTable_[MOV_RN7_DATA].setInstruction(STR_MOV, STR_mov_RNx_DATA, 1);
	//MOV direct,A 1111 0101
	instructionTable_[MOV_DIRECT_A].setInstruction(STR_MOV, STR_mov_DIRECT_A, 1);
	//MOV direct,Rn 1000 1rrr
	instructionTable_[MOV_DIRECT_RN0].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN1].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN2].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN3].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN4].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN5].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN6].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	instructionTable_[MOV_DIRECT_RN7].setInstruction(STR_MOV, STR_mov_DIRECT_RNx, 2);
	//MOV direct,direct 1000 0101
	instructionTable_[MOV_DIRECT_DIRECT].setInstruction(STR_MOV, STR_mov_DIRECT_DIRECT, 2);
	//MOV direct,@Ri 1000 011i
	instructionTable_[MOV_DIRECT_RI0].setInstruction(STR_MOV, STR_mov_DIRECT_RIx, 2);
	instructionTable_[MOV_DIRECT_RI1].setInstruction(STR_MOV, STR_mov_DIRECT_RIx, 2);
	//MOV direct,#data 0111 0101
	instructionTable_[MOV_DIRECT_DATA].setInstruction(STR_MOV, STR_mov_DIRECT_DATA, 2);
	//MOV @Ri,A 1111 011i
	instructionTable_[MOV_RI0_A].setInstruction(STR_MOV, STR_mov_RIx_A, 1);
	instructionTable_[MOV_RI1_A].setInstruction(STR_MOV, STR_mov_RIx_A, 1);
	//MOV @Ri,direct 1010 011i
	instructionTable_[MOV_RI0_DIRECT].setInstruction(STR_MOV, STR_mov_RIx_DIRECT, 2);
	instructionTable_[MOV_RI1_DIRECT].setInstruction(STR_MOV, STR_mov_RIx_DIRECT, 2);
	//MOV @Ri,#data 0111 011i
	instructionTable_[MOV_RI0_DATA].setInstruction(STR_MOV, STR_mov_RIx_DATA, 1);
	instructionTable_[MOV_RI1_DATA].setInstruction(STR_MOV, STR_mov_RIx_DATA, 1);
	//MOV DPTR,#DATA16 1001 0000
	instructionTable_[MOV_DPTR_DATA16].setInstruction(STR_MOV, STR_mov_DPTR_DATA16, 2);
	//MOV C,bit 1010 0010
	instructionTable_[MOV_C_BIT].setInstruction(STR_MOV, STR_mov_C_BIT, 2);
	//MOV bit,C 1001 0010
	instructionTable_[MOV_BIT_C].setInstruction(STR_MOV, STR_mov_BIT_C, 2);
	//MOVC A,@A+DPTR 1001 0011
	instructionTable_[MOVC_A_A_DPTR].setInstruction(STR_MOVC, STR_movc_A_A_DPTR, 2);
	//MOVC A,@A+PC 1000 0011
	instructionTable_[MOVC_A_A_PC].setInstruction(STR_MOVC, STR_movc_A_A_PC, 2);
	//MOVX A,@Ri 1110 001i
	instructionTable_[MOVX_A_RI0].setInstruction(STR_MOVX, STR_movx_A_RIx, 2);
	instructionTable_[MOVX_A_RI1].setInstruction(STR_MOVX, STR_movx_A_RIx, 2);
	//MOVX @Ri,A 1111 001i
	instructionTable_[MOVX_RI0_A].setInstruction(STR_MOVX, STR_movx_RIx_A, 2);
	instructionTable_[MOVX_RI1_A].setInstruction(STR_MOVX, STR_movx_RIx_A, 2);
	//MOVX A,@DPTR 1110 0000
	instructionTable_[MOVX_A_DPTR].setInstruction(STR_MOVX, STR_movx_A_DPTR, 2);
	//MOVX @DPTR,A 1111 0000
	instructionTable_[MOVX_DPTR_A].setInstruction(STR_MOVX, STR_movx_DPTR_A, 2);
	//POP direct 1101 0000
	instructionTable_[POP_DIRECT].setInstruction(STR_POP, STR_pop_DIRECT, 2);
	//PUSH direct 1100 0000
	instructionTable_[PUSH_DIRECT].setInstruction(STR_PUSH, STR_push_DIRECT, 2);
	//XCH A,Rn 1100 1rrr
	instructionTable_[XCH_A_RN0].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN1].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN2].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN3].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN4].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN5].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN6].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	instructionTable_[XCH_A_RN7].setInstruction(STR_XCH, STR_xch_A_RNx, 1);
	//XCH A,direct 1100 0101
	instructionTable_[XCH_A_DIRECT].setInstruction(STR_XCH, STR_xch_A_DIRECT, 1);
	//XCH A,@Ri 1100 011i
	instructionTable_[XCH_A_RI0].setInstruction(STR_XCH, STR_xch_A_RIx, 1);
	instructionTable_[XCH_A_RI1].setInstruction(STR_XCH, STR_xch_A_RIx, 1);
	//XCHD A,@Ri 1101 011i
	instructionTable_[XCHD_A_RI0].setInstruction(STR_XCHD, STR_xchd_A_RIx, 1);
	instructionTable_[XCHD_A_RI1].setInstruction(STR_XCHD, STR_xchd_A_RIx, 1);
	//SETB C 1101 0011
	instructionTable_[SETB_C].setInstruction(STR_SETB, STR_setb_C, 1);
	//SETB bit 1101 0010
	instructionTable_[SETB_BIT].setInstruction(STR_SETB, STR_setb_BIT, 1);
	//JC rel 0100 0000
	instructionTable_[JC_REL].setInstruction(STR_JC, STR_jc_REL, 2);
	//JNC rel 0101 0000
	instructionTable_[JNC_REL].setInstruction(STR_JNC, STR_jnc_REL, 2);
	//JB bit,rel 0010 0000
	instructionTable_[JB_BIT_REL].setInstruction(STR_JB, STR_jb_BIT_REL, 2);
	//JNB bit,rel 0011 0000
	instructionTable_[JNB_BIT_REL].setInstruction(STR_JNB, STR_jnb_BIT_REL, 2);
	//JBC bit,rel 0001 0000
	instructionTable_[JBC_BIT_REL].setInstruction(STR_JBC, STR_jbc_BIT_REL, 2);
	//ACALL  a10 a9 a8 1 0001
	instructionTable_[ACALL_100].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_200].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_300].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_400].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_500].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_600].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_700].setInstruction(STR_ACALL, STR_acall_x00, 2);
	instructionTable_[ACALL_800].setInstruction(STR_ACALL, STR_acall_x00, 2);
	//LCALL 0001 0010
	instructionTable_[LCALL].setInstruction(STR_LCALL, STR_lcall , 2);
	//RET 0010 0010
	instructionTable_[RET].setInstruction(STR_RET, STR_ret, 2);
	//RETI 0011 0010
	instructionTable_[RETI].setInstruction(STR_RETI, STR_reti, 2);
	//AJMP a10 a9 a8 0 0001
	instructionTable_[AJMP_100].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_200].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_300].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_400].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_500].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_600].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_700].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	instructionTable_[AJMP_800].setInstruction(STR_AJMP, STR_ajmp_x00, 2);
	//LJMP
	instructionTable_[LJMP].setInstruction(STR_LJMP, STR_ljmp, 2);
	//SJMP rel 1000 0000
	instructionTable_[SJMP].setInstruction(STR_SJMP, STR_sjmp, 2);
	//JMP @A+DPTR 0111 0011
	instructionTable_[JMP_A_DPTR].setInstruction(STR_JMP, STR_jmp_A_DPTR, 2);
	//JZ rel 0110 0000
	instructionTable_[JZ_REL].setInstruction(STR_JZ, STR_jz_REL, 2);
	//JNZ rel 0111 0000
	instructionTable_[JNZ_REL].setInstruction(STR_JNZ, STR_jnz_REL, 2);
	//CJNE A,direct,rel 1011 0101
	instructionTable_[CJNE_A_DIRECT_REL].setInstruction(STR_CJNE, STR_cjne_A_DIRECT_REL, 2);
	//CJNE A,#data,rel 1011 0100
	instructionTable_[CJNE_A_DATA_REL].setInstruction(STR_CJNE, STR_cjne_A_DATA_REL, 2);
	//CJNE Rn,#data,rel 1011 1rrr
	instructionTable_[CJNE_RN0_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN1_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN2_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN3_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN4_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN5_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN6_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	instructionTable_[CJNE_RN7_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RNx_DATA_REL, 2);
	//CJNE @Ri,#data,rel 1011 011 r
	instructionTable_[CJNE_RI0_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RIx_DATA_REL, 2);
	instructionTable_[CJNE_RI1_DATA_REL].setInstruction(STR_CJNE, STR_cjne_RIx_DATA_REL, 2);
	//DJNZ Rn,rel 1101 1rrr
	instructionTable_[DJNZ_RN0_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN1_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN2_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN3_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN4_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN5_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN6_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	instructionTable_[DJNZ_RN7_REL].setInstruction(STR_DJNZ, STR_djnz_RNx_REL, 2);
	//DJNZ direct,rel 1101 0101
	instructionTable_[DJNZ_DIRECT_REL].setInstruction(STR_DJNZ, STR_djnz_DIRECT_REL, 2);
	//NOP 0000 0000
	instructionTable_[NOP].setInstruction(STR_NOP, STR_nop, 1);
}

