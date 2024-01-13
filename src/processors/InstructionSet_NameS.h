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
#ifndef INSTRUCTIONSET_NAMES_H
#define INSTRUCTIONSET_NAMES_H

namespace InstructionSet_NameS
{


	//Käskyt______________________________________________________________
	//ACALL  a10 a9 a8 1 0001
	unsigned char const
		ACALL_100=0x11,
		ACALL_200=0x31,
		ACALL_300=0x51,
		ACALL_400=0x71,
		ACALL_500=0x91,
		ACALL_600=0xB1,
		ACALL_700=0xD1,
		ACALL_800=0xF1;

	//ADD A,Rn  0010  1rrr
	unsigned char const
		ADD_A_RN0=0x28,
		ADD_A_RN1=0x29,
		ADD_A_RN2=0x2A,
		ADD_A_RN3=0x2B,
		ADD_A_RN4=0x2C,
		ADD_A_RN5=0x2D,
		ADD_A_RN6=0x2E,
		ADD_A_RN7=0x2F;

	//ADD A,direct 0010 0101 + 8b direct address
	unsigned char const  ADD_A_DIRECT=0x25;

	//ADD A,@RI 0010 011i
	unsigned char const ADD_A_RI0=0x26;
	unsigned char const ADD_A_RI1=0x27;

	//ADD A,#data
	unsigned char const ADD_A_DATA=0x24;

	//ADDC A,Rn 0011 1rrr
	unsigned char const
		ADDC_A_RN0=0x38,
		ADDC_A_RN1=0x39,
		ADDC_A_RN2=0x3A,
		ADDC_A_RN3=0x3B,
		ADDC_A_RN4=0x3C,
		ADDC_A_RN5=0x3D,
		ADDC_A_RN6=0x3E,
		ADDC_A_RN7=0x3F;

	//ADDC A,direct 0010 0101
	unsigned char const  ADDC_A_DIRECT=0x35;

	//ADDC A,@RI 0010 011i
	unsigned char const ADDC_A_RI0=0x36;
	unsigned char const ADDC_A_RI1=0x37;

	//ADDC A,#data
	unsigned char const ADDC_A_DATA=0x34;

	//AJMP a10 a9 a8 0 0001
	unsigned char const
		AJMP_100=0x01,
		AJMP_200=0x21,
		AJMP_300=0x41,
		AJMP_400=0x61,
		AJMP_500=0x81,
		AJMP_600=0xA1,
		AJMP_700=0xC1,
		AJMP_800=0xE1;

	//ANL A,Rn 0101 1rrr
	unsigned char const
		ANL_A_RN0=0x58,
		ANL_A_RN1=0x59,
		ANL_A_RN2=0x5A,
		ANL_A_RN3=0x5B,
		ANL_A_RN4=0x5C,
		ANL_A_RN5=0x5D,
		ANL_A_RN6=0x5E,
		ANL_A_RN7=0x5F;

	//ANL A,direct
	unsigned char const ANL_A_DIRECT=0x55;

	//ANL A,@Ri
	unsigned char const ANL_A_RI0=0x56;
	unsigned char const ANL_A_RI1=0x57;

	//ANL A,#data
	unsigned char const ANL_A_DATA=0x54;

	//ANL direct,A
	unsigned char const ANL_DIRECT_A=0x52;

	//ANL direct,#data
	unsigned char const ANL_DIRECT_DATA=0x53;

	//ANL C,bit 1000 0010
	unsigned char const ANL_C_BIT=0x82;

	//Anl C,/bit 1011 0000
	unsigned char const ANL_C_NBIT=0xB0;

	//CJNE A,direct,rel 1011 0101
	unsigned char const CJNE_A_DIRECT_REL=0xB5;

	//CJNE A,#data,rel 1011 0100
	unsigned char const CJNE_A_DATA_REL=0xB4;

	//CJNE Rn,#data,rel 1011 1rrr
	unsigned char const
		CJNE_RN0_DATA_REL=0xB8,
		CJNE_RN1_DATA_REL=0xB9,
		CJNE_RN2_DATA_REL=0xBA,
		CJNE_RN3_DATA_REL=0xBB,
		CJNE_RN4_DATA_REL=0xBC,
		CJNE_RN5_DATA_REL=0xBD,
		CJNE_RN6_DATA_REL=0xBE,
		CJNE_RN7_DATA_REL=0xBF;

	//CJNE @Ri,#data,rel 1011 011 r
	unsigned char const CJNE_RI0_DATA_REL=0xB6;
	unsigned char const CJNE_RI1_DATA_REL=0xB7;

	//CLR A 1110 0100
	unsigned char const CLR_A=0xE4;

	//CLR C 1100 0011
	unsigned char const CLR_C=0xC3;

	//CLR bit 1100 0010
	unsigned char const CLR_BIT=0xC2;

	//CPL A 1111 0100
	unsigned char const CPL_A=0xF4;

	//CPL C 1011 0011
	unsigned char const CPL_C=0xB3;

	//CPL bit
	unsigned char const CPL_BIT=0xB2;

	//DA A 1101 0100
	unsigned char const DA_A=0xD4;

	//DEC A 0001 0100
	unsigned char const DEC_A=0x14;

	//DEC Rn 0001 1rrr
	unsigned char const
		DEC_RN0=0x18,
		DEC_RN1=0x19,
		DEC_RN2=0x1A,
		DEC_RN3=0x1B,
		DEC_RN4=0x1C,
		DEC_RN5=0x1D,
		DEC_RN6=0x1E,
		DEC_RN7=0x1F;

	//DEC direct 0001 0101
	unsigned char const DEC_DIRECT=0x15;

	//DEC @RI 0001 011 i
	unsigned char const DEC_RI0=0x16;
	unsigned char const DEC_RI1=0x17;

	//DIV AB 1000 0100
	unsigned char const DIV_AB=0x84;

	//DJNZ Rn,rel 1101 1rrr
	unsigned char const
		DJNZ_RN0_REL=0xD8,
		DJNZ_RN1_REL=0xD9,
		DJNZ_RN2_REL=0xDA,
		DJNZ_RN3_REL=0xDB,
		DJNZ_RN4_REL=0xDC,
		DJNZ_RN5_REL=0xDD,
		DJNZ_RN6_REL=0xDE,
		DJNZ_RN7_REL=0xDF;

	//DJNZ direct,rel 1101 0101
	unsigned char const DJNZ_DIRECT_REL=0xD5;

	//INC A 0000 0100
	unsigned char const INC_A=0x04;

	//INC Rn 0000 1rrr
	unsigned char const
		INC_RN0=0x08,
		INC_RN1=0x09,
		INC_RN2=0x0A,
		INC_RN3=0x0B,
		INC_RN4=0x0C,
		INC_RN5=0x0D,
		INC_RN6=0x0E,
		INC_RN7=0x0F;

	//INC direct 0000 0101
	unsigned char const INC_DIRECT=0x05;

	//INC @RI 0000 011i
	unsigned char const INC_RI0=0x06;
	unsigned char const INC_RI1=0x07;

	//INC DPTR 1010 0011
	unsigned char const INC_DPTR=0xA3;

	//JB bit,rel 0010 0000
	unsigned char const JB_BIT_REL=0x20;

	//JBC bit,rel 0001 0000
	unsigned char const JBC_BIT_REL=0x10;

	//JC rel 0100 0000
	unsigned char const JC_REL=0x40;

	//JMP @A+DPTR 0111 0011
	unsigned char const JMP_A_DPTR=0x73;

	//JNB bit,rel 0011 0000
	unsigned char const JNB_BIT_REL=0x30;

	//JNC rel 0101 0000
	unsigned char const JNC_REL=0x50;

	//JNZ rel 0111 0000
	unsigned char const JNZ_REL=0x70;

	//JZ rel 0110 0000
	unsigned char const JZ_REL=0x60;

	//LCALL 0001 0010
	unsigned char const LCALL=0x12;

	//LJMP
	unsigned char const LJMP=0x02;

	//MOV A,Rn 1110 1rrr
	unsigned char const
		MOV_A_RN0=0xE8,
		MOV_A_RN1=0xE9,
		MOV_A_RN2=0xEA,
		MOV_A_RN3=0xEB,
		MOV_A_RN4=0xEC,
		MOV_A_RN5=0xED,
		MOV_A_RN6=0xEE,
		MOV_A_RN7=0xEF;

	//MOV A,direct 1110 0101
	unsigned char const MOV_A_DIRECT=0xE5;

	//MOV A,@Ri 1110 011i
	unsigned char const MOV_A_RI0=0xE6;
	unsigned char const MOV_A_RI1=0xE7;

	//MOV A,#data 0111 0100
	unsigned char const MOV_A_DATA=0x74;

	//MOV Rn,A 1111 1rrr
	unsigned char const
		MOV_RN0_A=0xF8,
		MOV_RN1_A=0xF9,
		MOV_RN2_A=0xFA,
		MOV_RN3_A=0xFB,
		MOV_RN4_A=0xFC,
		MOV_RN5_A=0xFD,
		MOV_RN6_A=0xFE,
		MOV_RN7_A=0xFF;

	//Rn,direct 1010 1rrr
	unsigned char const 
		MOV_RN0_DIRECT=0xA8,
		MOV_RN1_DIRECT=0xA9,
		MOV_RN2_DIRECT=0xAA,
		MOV_RN3_DIRECT=0xAB,
		MOV_RN4_DIRECT=0xAC,
		MOV_RN5_DIRECT=0xAD,
		MOV_RN6_DIRECT=0xAE,
		MOV_RN7_DIRECT=0xAF;

	//MOV Rn,#data 0111 1rrr
	unsigned char const
		MOV_RN0_DATA=0x78,
		MOV_RN1_DATA=0x79,
		MOV_RN2_DATA=0x7A,
		MOV_RN3_DATA=0x7B,
		MOV_RN4_DATA=0x7C,
		MOV_RN5_DATA=0x7D,
		MOV_RN6_DATA=0x7E,
		MOV_RN7_DATA=0x7F;

	//MOV direct,A 1111 0101
	unsigned char const MOV_DIRECT_A=0xF5;

	//MOV direct,Rn 1000 1rrr
	unsigned char const
		MOV_DIRECT_RN0=0x88,
		MOV_DIRECT_RN1=0x89,
		MOV_DIRECT_RN2=0x8A,
		MOV_DIRECT_RN3=0x8B,
		MOV_DIRECT_RN4=0x8C,
		MOV_DIRECT_RN5=0x8D,
		MOV_DIRECT_RN6=0x8E,
		MOV_DIRECT_RN7=0x8F;

	//MOV direct,direct 1000 0101
	unsigned char const MOV_DIRECT_DIRECT=0x85;

	//MOV direct,@Ri 1000 011i
	unsigned char const MOV_DIRECT_RI0=0x86;
	unsigned char const MOV_DIRECT_RI1=0x87;

	//MOV direct,#data 0111 0101
	unsigned char const MOV_DIRECT_DATA=0x75;

	//MOV @Ri,A 1111 011i
	unsigned char const MOV_RI0_A=0xF6;
	unsigned char const MOV_RI1_A=0xF7;

	//MOV @Ri,direct 1010 011i
	unsigned char const MOV_RI0_DIRECT=0xA6;
	unsigned char const MOV_RI1_DIRECT=0xA7;

	//MOV @Ri,#data 0111 011i
	unsigned char const MOV_RI0_DATA=0x76;
	unsigned char const MOV_RI1_DATA=0x77;

	//MOV C,bit 1010 0010
		unsigned char const MOV_C_BIT=0xA2;

	//MOV bit,C 1001 0010
	unsigned char const MOV_BIT_C=0x92;

	//MOV DPTR,#DATA16 1001 0000
	unsigned char const MOV_DPTR_DATA16=0x90;

	//MOVC A,@A+DPTR 1001 0011
	unsigned char const MOVC_A_A_DPTR=0x93;

	//MOVC A,@A+PC 1000 0011
	unsigned char const MOVC_A_A_PC=0x83;

	//MOVX A,@Ri 1110 001i
	unsigned char const MOVX_A_RI0=0xE2;
	unsigned char const MOVX_A_RI1=0xE3;

	//MOVX @Ri,A 1111 001i
	unsigned char const MOVX_RI0_A=0xF2;
	unsigned char const MOVX_RI1_A=0xF3;

	//MOVX A,@DPTR 1110 0000
	unsigned char const MOVX_A_DPTR=0xE0;

	//MOVX @DPTR,A 1111 0000
	unsigned char const MOVX_DPTR_A=0xF0;

	//NOP 0000 0000
	unsigned char const NOP=0x00;

	//MUL AB 1010 0100
	unsigned char const MUL_AB=0xA4;

	//ORL A,Rn 0100 1rrr
 unsigned char const 
		ORL_A_RN0=0x48,
		ORL_A_RN1=0x49,
		ORL_A_RN2=0x4A,
		ORL_A_RN3=0x4B,
		ORL_A_RN4=0x4C,
		ORL_A_RN5=0x4D,
		ORL_A_RN6=0x4E,
		ORL_A_RN7=0x4F;

	//ORL A,direct 0100 0101
	unsigned char const ORL_A_DIRECT=0x45;

	//ORl A,@Ri 0100 011i
	unsigned char const ORL_A_RI0=0x46;
	unsigned char const ORL_A_RI1=0x47;

	//ORL A,#data 0100 0100
	unsigned char const ORL_A_DATA=0x44;

	//ORL direct,A 0100 0010
	unsigned char const ORL_DIRECT_A=0x42;

	//ORL direct,#data 0100 0011
	unsigned char const ORL_DIRECT_DATA=0x43;

	//ORL C,bit 0111 0010
	unsigned char const ORL_C_BIT=0x72;

	//ORL C,/bit 1010 0000
	unsigned char const ORL_C_NBIT=0xA0;

	//POP direct 1101 0000
	unsigned char const POP_DIRECT=0xD0;

	//PUSH direct 1100 0000
	unsigned char const PUSH_DIRECT=0xC0;

	//RET 0010 0010
	unsigned char const RET=0x22;

	//RETI 0011 0010
	unsigned char const RETI=0x32;

	//RL A 0010 0011
	unsigned char const RL_A=0x23;

	//RLC A 0011 0011
	unsigned char const RLC_A=0x33;

	//RR A 0000 0011
	unsigned char const RR_A=0x03;

	//RRC A 0001 0011
	unsigned char const RRC_A=0x13;

	//SETB C 1101 0011
	unsigned char const SETB_C=0xD3;

	//SETB bit 1101 0010
	unsigned char const SETB_BIT=0xD2;

	//SJMP rel 1000 0000
	unsigned char const SJMP=0x80;

	//SUBB A,Rn 1001 1rrr
 unsigned char const 
		SUBB_A_RN0=0x98,
		SUBB_A_RN1=0x99,
		SUBB_A_RN2=0x9A,
		SUBB_A_RN3=0x9B,
		SUBB_A_RN4=0x9C,
		SUBB_A_RN5=0x9D,
		SUBB_A_RN6=0x9E,
		SUBB_A_RN7=0x9F;

	//SUBB A,direct 1001 0101
	unsigned char const SUBB_A_DIRECT=0x95;

	//SUBB A,@Ri 1001 011i
	unsigned char const SUBB_A_RI0=0x96;
	unsigned char const SUBB_A_RI1=0x97;

	//SUBB A,#data 1001 0100
	unsigned char const SUBB_A_DATA=0x94;

	//SWAP A 1100 0100
	unsigned char const SWAP_A=0xC4;

	//XCH A,Rn 1100 1rrr
 unsigned char const 
		XCH_A_RN0=0xC8,
		XCH_A_RN1=0xC9,
		XCH_A_RN2=0xCA,
		XCH_A_RN3=0xCB,
		XCH_A_RN4=0xCC,
		XCH_A_RN5=0xCD,
		XCH_A_RN6=0xCE,
		XCH_A_RN7=0xCF;

	//XCH A,direct 1100 0101
	unsigned char const XCH_A_DIRECT=0xC5;

	//XCH A,@Ri 1100 011i
	unsigned char const XCH_A_RI0=0xC6;
	unsigned char const XCH_A_RI1=0xC7;

	//XCHD A,@Ri 1101 011i
	unsigned char const XCHD_A_RI0=0xD6;
	unsigned char const XCHD_A_RI1=0xD7;

	//XRL A,Rn 0110 1rrr
 unsigned char const 
		XRL_A_RN0=0x68,
		XRL_A_RN1=0x69,
		XRL_A_RN2=0x6A,
		XRL_A_RN3=0x6B,
		XRL_A_RN4=0x6C,
		XRL_A_RN5=0x6D,
		XRL_A_RN6=0x6E,
		XRL_A_RN7=0x6F;

	//XRL A,direct 0110 0101
	unsigned char const XRL_A_DIRECT=0x65;

	//XRL A,@Ri 0110 011i
	unsigned char const XRL_A_RI0=0x66;
	unsigned char const XRL_A_RI1=0x67;

	//XRL A,#data 0110 0100
	unsigned char const XRL_A_DATA=0x64;

	//XRL direct,A 0110 0010
	unsigned char const XRL_DIRECT_A=0x62;

	//XRL direct,#data 0110 0011
	unsigned char const XRL_DIRECT_DATA=0x63;

	//Instruction Names
	#define STR_ADD 			"add"
	#define STR_ADDC 			"addc"
	#define STR_SUBB 			"subb"
	#define STR_INC 			"inc"
	#define STR_DEC 			"dec"
	#define STR_MUL 			"mul"
	#define STR_DIV 			"div"
	#define STR_DA 			"da"
	#define STR_ANL 			"anl"
	#define STR_ORL 			"orl"
	#define STR_XRL 			"xrl"
	#define STR_CLR 			"clr"
	#define STR_CPL 			"cpl"
	#define STR_RL 			"rl"
	#define STR_RLC 			"rlc"
	#define STR_RR 			"rr"
	#define STR_RRC 			"rrc"
	#define STR_SWAP 			"swap"
	#define STR_MOV 			"mov"
	#define STR_MOVC 			"movc"
	#define STR_MOVX 			"movx"
	#define STR_POP 			"pop"
	#define STR_PUSH 			"push"
	#define STR_XCH 			"xch"
	#define STR_XCHD 			"xchd"
	#define STR_SETB 			"setb"
	#define STR_JC 			"jc"
	#define STR_JNC 			"jnc"
	#define STR_JB 			"jb"
	#define STR_JNB 			"jnb"
	#define STR_JBC 			"jbc"
	#define STR_ACALL 		"acall"
	#define STR_LCALL 		"lcall"
	#define STR_RET 			"ret"
	#define STR_RETI 			"reti"
	#define STR_AJMP 			"ajmp"
	#define STR_LJMP 			"ljmp"
	#define STR_SJMP 			"sjmp"
	#define STR_JMP 			"jmp"
	#define STR_JZ 			"jz"
	#define STR_JNZ 			"jnz"
	#define STR_CJNE 			"cjne"
	#define STR_DJNZ 			"djnz"
	#define STR_NOP 			"nop"

//Instruction Mnemonics
	#define STR_add_A_RNx "add_A_RNx"
	#define STR_add_A_DIRECT "add_A_DIRECT"
	#define STR_add_A_RIx 				"add_A_RIx"
	#define STR_add_A_DATA 				"add_A_DATA"
	#define STR_addc_A_RNx 				"addc_A_RNx"
	#define STR_addc_A_DIRECT 			"addc_A_DIRECT"
	#define STR_addc_A_RIx 				"addc_A_RIx"
	#define STR_addc_A_DATA 			"addc_A_DATA"
	#define STR_subb_A_RNx 				"subb_A_RNx"
	#define STR_subb_A_DIRECT 			"subb_A_DIRECT"
	#define STR_subb_A_RIx 				"subb_A_RIx"
	#define STR_subb_A_DATA 			"subb_A_DATA"
	#define STR_inc_A 					"inc_A"
	#define STR_inc_RNx 					"inc_RNx"
	#define STR_inc_DIRECT 				"inc_DIRECT"
	#define STR_inc_RIx 					"inc_RIx"
	#define STR_inc_DPTR 				"inc_DPTR"
	#define STR_dec_A 					"dec_A"
	#define STR_dec_RNx 					"dec_RNx"
	#define STR_dec_DIRECT 				"dec_DIRECT"
	#define STR_dec_RIx 					"dec_RIx"
	#define STR_mul_AB 					"mul_AB"
	#define STR_div_AB 					"div_AB"
	#define STR_da_A 						"da_A"
	#define STR_anl_A_RNx 				"anl_A_RNx"
	#define STR_anl_A_DIRECT 			"anl_A_DIRECT"
	#define STR_anl_A_RIx 				"anl_A_RIx"
	#define STR_anl_A_DATA 				"anl_A_DATA"
	#define STR_anl_DIRECT_A 			"anl_DIRECT_A"
	#define STR_anl_DIRECT_DATA 		"anl_DIRECT_DATA"
	#define STR_anl_C_BIT 				"anl_C_BIT"
	#define STR_anl_C_NBIT 				"anl_C_NBIT"
	#define STR_orl_A_RNx 				"orl_A_RNx"
	#define STR_orl_A_DIRECT 			"orl_A_DIRECT"
	#define STR_orl_A_RIx 				"orl_A_RIx"
	#define STR_orl_A_DATA 				"orl_A_DATA"
	#define STR_orl_DIRECT_A 			"orl_DIRECT_A"
	#define STR_orl_DIRECT_DATA 		"orl_DIRECT_DATA"
	#define STR_orl_C_BIT 				"orl_C_BIT"
	#define STR_orl_C_NBIT 				"orl_C_NBIT"
	#define STR_xrl_A_RNx 				"xrl_A_RNx"
	#define STR_xrl_A_DIRECT 			"xrl_A_DIRECT"
	#define STR_xrl_A_RIx 				"xrl_A_RIx"
	#define STR_xrl_A_DATA 				"xrl_A_DATA"
	#define STR_xrl_DIRECT_A 			"xrl_DIRECT_A"
	#define STR_xrl_DIRECT_DATA 		"xrl_DIRECT_DATA"
	#define STR_clr_A 					"clr_A"
	#define STR_clr_C 					"clr_C"
	#define STR_clr_BIT 					"clr_BIT"
	#define STR_cpl_A 					"cpl_A"
	#define STR_cpl_C 					"cpl_C"
	#define STR_cpl_BIT 					"cpl_BIT"
	#define STR_rl_A 						"rl_A"
	#define STR_rlc_A 					"rlc_A"
	#define STR_rr_A 						"rr_A"
	#define STR_rrc_A 					"rrc_A"
	#define STR_swap_A 					"swap_A"
	#define STR_mov_A_RNx 				"mov_A_RNx"
	#define STR_mov_A_DIRECT 			"mov_A_DIRECT"
	#define STR_mov_A_RIx 				"mov_A_RIx"
	#define STR_mov_A_DATA 				"mov_A_DATA"
	#define STR_mov_RNx_A 				"mov_RNx_A"
	#define STR_mov_RNx_DIRECT 		"mov_RNx_DIRECT"
	#define STR_mov_RNx_DATA 			"mov_RNx_DATA"
	#define STR_mov_DIRECT_A 			"mov_DIRECT_A"
	#define STR_mov_DIRECT_RNx 		"mov_DIRECT_RNx"
	#define STR_mov_DIRECT_DIRECT 	"mov_DIRECT_DIRECT"
	#define STR_mov_DIRECT_RIx 		"mov_DIRECT_RIx"
	#define STR_mov_DIRECT_DATA 		"mov_DIRECT_DATA"
	#define STR_mov_RIx_A 				"mov_RIx_A"
	#define STR_mov_RIx_DIRECT 		"mov_RIx_DIRECT"
	#define STR_mov_RIx_DATA 			"mov_RIx_DATA"
	#define STR_mov_DPTR_DATA16 		"mov_DPTR_DATA16"
	#define STR_mov_C_BIT 				"mov_C_BIT"
	#define STR_mov_BIT_C 				"mov_BIT_C"
	#define STR_movc_A_A_DPTR 			"movc_A_A_DPTR"
	#define STR_movc_A_A_PC 			"movc_A_A_PC"
	#define STR_movx_A_RIx 				"movx_A_RIx"
	#define STR_movx_RIx_A 				"movx_RIx_A"
	#define STR_movx_A_DPTR 			"movx_A_DPTR"
	#define STR_movx_DPTR_A 			"movx_DPTR_A"
	#define STR_pop_DIRECT 				"pop_DIRECT"
	#define STR_push_DIRECT 			"push_DIRECT"
	#define STR_xch_A_RNx 				"xch_A_RNx"
	#define STR_xch_A_DIRECT 			"xch_A_DIRECT"
	#define STR_xch_A_RIx 				"xch_A_RIx"
	#define STR_xchd_A_RIx 				"xchd_A_RIx"
	#define STR_setb_C 					"setb_C"
	#define STR_setb_BIT 				"setb_BIT"
	#define STR_jc_REL 					"jc_REL"
	#define STR_jnc_REL 					"jnc_REL"
	#define STR_jb_BIT_REL 				"jb_BIT_REL"
	#define STR_jnb_BIT_REL 			"jnb_BIT_REL"
	#define STR_jbc_BIT_REL 			"jbc_BIT_REL"
	#define STR_acall_x00 				"acall_x00"
	#define STR_lcall 					"lcall"
	#define STR_ret 						"ret"
	#define STR_reti 						"reti"
	#define STR_ajmp_x00 				"ajmp_x00"
	#define STR_ljmp 						"ljmp"
	#define STR_sjmp 						"sjmp"
	#define STR_jmp_A_DPTR 				"jmp_A_DPTR"
	#define STR_jz_REL 					"jz_REL"
	#define STR_jnz_REL 					"jnz_REL"
	#define STR_cjne_A_DIRECT_REL 	"cjne_A_DIRECT_REL"
	#define STR_cjne_A_DATA_REL 		"cjne_A_DATA_REL"
	#define STR_cjne_RNx_DATA_REL 	"cjne_RNx_DATA_REL"
	#define STR_cjne_RIx_DATA_REL 	"cjne_RIx_DATA_REL"
	#define STR_djnz_RNx_REL 			"djnz_RNx_REL"
	#define STR_djnz_DIRECT_REL 		"djnz_DIRECT_REL"
	#define STR_nop 						"nop"
};

#endif
