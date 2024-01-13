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
#ifndef T89C51RD2x_NAMES_H
#define T89C51RD2x_NAMES_H

namespace T89C51RD2x_NameS
{
	using namespace I8031_NameS;
	using namespace I8032_NameS;
	//SFR REKKARIT JA BITTIOSOITETTAVIEN BITIT____________________________________
	unsigned char const
		AUXR=0x8E,
		CKCON=0x8F,
		AUXR1=0xA2,
		WDTRST=0xA6,
		WDTPRG=0xA7,
		SADDR=0xA9,
		SADEN=0xB9,
		IPH=0xB7,
		FCON=0xD1,
		EECON=0xD2,
		EETIM=0xD3,
		CCON=0xD8,
		CMOD=0xD9,
		CCAPM0=0xDA,
		CCAPM1=0xDB,
		CCAPM2=0xDC,
		CCAPM3=0xDD,
		CCAPM4=0xDE,
		CL=0xE9,
		CCAP0L=0xEA,
		CCAP1L=0xEB,
		CCAP2L=0xEC,
		CCAP3L=0xED,
		CCAP4L=0xEE,
		CH=0xF9,
		CCAP0H=0xFA,
		CCAP1H=0xFB,
		CCAP2H=0xFC,
		CCAP3H=0xFD,
		CCAP4H=0xFE;

	//Rekkarien bitit
	unsigned char const
		//AUXR bitit
		AUXR_A0=BIT_0,
		AUXR_EXTRAM=BIT_1,
		AUXR_XRS0=BIT_2,
		AUXR_XRS1=BIT_3,
		AUXR_M0=BIT_5,

		//CKCON bitit
		CKCON_X2=BIT_0,
		CKCON_T0X2=BIT_1,
		CKCON_T1X2=BIT_2,
		CKCON_T2X2=BIT_3,
		CKCON_SiX2=BIT_4,
		CKCON_PcaX2=BIT_5,
		CKCON_WdX2=BIT_6,

		//AUXR1 bitit
		AUXR1_DPS=BIT_0,
		AUXR1_GF3=BIT_3,

		//WDTPRG bitit
		WDTPRG_S0=BIT_0,
		WDTPRG_S1=BIT_1,
		WDTPRG_S2=BIT_2,
		WDTPRG_T0=BIT_3,
		WDTPRG_T1=BIT_4,
		WDTPRG_T2=BIT_5,
		WDTPRG_T3=BIT_6,
		WDTPRG_T4=BIT_7,

		//IE:n tämän prosessorin bitit
		IE_EC=BIT_6,

		//IP: tämän porosessorin bitit
		IP_PPC=BIT_6,

		//IPH bitit
		IPH_PX0H=BIT_0,
		IPH_PT0H=BIT_1,
		IPH_PX1H=BIT_2,
		IPH_PT1H=BIT_3,
		IPH_PSH=BIT_4,
		IPH_PT2H=BIT_5,
		IPH_PPCH=BIT_6,

		//FCON bitit
		FCON_FBUSY=BIT_0,
		FCON_FMOD0=BIT_1,
		FCON_FMOD1=BIT_2,
		FCON_FPS=BIT_3,
		FCON_FPL0=BIT_4,
		FCON_FPL1=BIT_5,
		FCON_FPL2=BIT_6,
		FCON_FPL3=BIT_7,

		//EECON bitit
		EECON_FEBUSY=BIT_0,
		EECON_EEE=BIT_1,
		EECON_EEPL0=BIT_4,
		EECON_EEPL1=BIT_5,
		EECON_EEPL2=BIT_6,
		EECON_EEPL3=BIT_7,

		//CCON bitit
		CCON_CCF0=BIT_0,
		CCON_CCF1=BIT_1,
		CCON_CCF2=BIT_2,
		CCON_CCF3=BIT_3,
		CCON_CCF4=BIT_4,
		CCON_CR=BIT_6,
		CCON_CF=BIT_7,

		//CMOD bitit
		CMOD_ECF=BIT_0,
		CMOD_CPS0=BIT_1,
		CMOD_CPS1=BIT_2,
		CMOD_WDTE=BIT_6,
		CMOD_CIDL=BIT_7,

		//CCAM0 bitit
		CCAPM0_ECCF0=BIT_0,
		CCAPM0_PWM0=BIT_1,
		CCAPM0_TOG0=BIT_2,
		CCAPM0_MAT0=BIT_3,
		CCAPM0_CAPN0=BIT_4,
		CCAPM0_CAPP0=BIT_5,
		CCAPM0_ECOM0=BIT_6,

		//CCAPM1 bitit
		CCAPM1_ECCF1=BIT_0,
		CCAPM1_PWM1=BIT_1,
		CCAPM1_TOG1=BIT_2,
		CCAPM1_MAT1=BIT_3,
		CCAPM1_CAPN1=BIT_4,
		CCAPM1_CAPP1=BIT_5,
		CCAPM1_ECOM1=BIT_6,

		//CCAPM2 bitit
		CCAPM2_ECCF2=BIT_0,
		CCAPM2_PWM2=BIT_1,
		CCAPM2_TOG2=BIT_2,
		CCAPM2_MAT2=BIT_3,
		CCAPM2_CAPN2=BIT_4,
		CCAPM2_CAPP2=BIT_5,
		CCAPM2_ECOM2=BIT_6,

		//CCAPM3 bitit
		CCAPM3_ECCF3=BIT_0,
		CCAPM3_PWM3=BIT_1,
		CCAPM3_TOG3=BIT_2,
		CCAPM3_MAT3=BIT_3,
		CCAPM3_CAPN3=BIT_4,
		CCAPM3_CAPP3=BIT_5,
		CCAPM3_ECOM3=BIT_6,

		//CCAPM4 bitit
		CCAPM4_ECCF4=BIT_0,
		CCAPM4_PWM4=BIT_1,
		CCAPM4_TOG4=BIT_2,
		CCAPM4_MAT4=BIT_3,
		CCAPM4_CAPN4=BIT_4,
		CCAPM4_CAPP4=BIT_5,
		CCAPM4_ECOM4=BIT_6;

	//Interrupt handlays
	//Lisätty tämän prosessorin keskeytykset.
	//Jos prioriteetti 0 low niin arvo alle 0x80 jos prioriteetti 1 niin yli 0x80
	//eli MSB kertoo keskeytyksen prioriteetin kantaprosessoreissa
	//Tässä prosessorissa on 4 tasoinen keskeytysprioriteetti. Bitti 6  kertoo high tason
	//Kantaluokkien prioriteetinkorotus
	unsigned char const  IE0P2IH=0x41;   //INT 0 prioriteetti 2
	unsigned char const  TF0P2IH=0x42;   //Timer 0 prioriteetti 2
	unsigned char const  IE1P2IH=0x43;   //INT 1 prioriteetti 2
	unsigned char const  TF1P2IH=0x44;   //Timer 1 prioriteetti 2
	unsigned char const  RIP2IH=0x45;   //Serial port receive prioriteetti 2
	unsigned char const  TIP2IH=0x46;   //Serial port transmit prioriteetti 2
	unsigned char const  TF2P2IH=0x47;   //Timer 2 prioriteetti 2
	unsigned char const  IE0P3IH=0xC1;   //INT 0 prioriteetti 3
	unsigned char const  TF0P3IH=0xC2;   //Timer 0 prioriteetti 3
	unsigned char const  IE1P3IH=0xC3;   //INT 1 prioriteetti 3
	unsigned char const  TF1P3IH=0xC4;   //Timer 1 prioriteetti 3
	unsigned char const  RIP13H=0xC5;    //Serial port receive prioriteetti 3
	unsigned char const  TIP13H=0xC6;   //Serial port transmit prioriteetti 3
	unsigned char const  TF2P3IH=0xC7;   //Timer 2 prioriteetti 3
	//Tämän proskun keskeyts
	unsigned char const  PCAP0IH=0x08;    //PCA prioriteetti 0
	unsigned char const  PCAP1IH=0x88;   //PCA prioriteetti 1
	unsigned char const  PCAP2IH=0x48;    //PCA prioriteetti 2
	unsigned char const  PCAP3IH=0xC8;   //PCA prioriteetti 3

	//Tämän prosun omat keskeytysVEKTORI:
	unsigned int const  PCA_INTERRUPT_VECTOR=0x33;

	//Pinninimet
	
};

#endif
