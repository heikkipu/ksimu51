/**************************************************************************
*   Copyright (C) 2005 - ... Heikki Pulkkinen                             *
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
#ifndef MAX765X_H
#define MAX765X_H

#include "../I80C52.h"
#include "../I8031_ADConverter.h"
#include "../I8051_Flash.h"
#include "../AnalogInputPinList.h"
#include "../InstructionSet.h"
#include "MAX765x_ExternalCycle.h"
#include "MAX765x_NameS.h"
#include "MAX765x_TimersCounters.h"

#include<cmath>

class MAX765x:  public I80C52, public I8031_ADConverter, public I8051_Flash
{
	public:
		MAX765x(unsigned long const flashSize);
		virtual ~MAX765x(void);

		unsigned int readDPTRValue(void) override;

		I8031_NameS::machineCycles_Type calculateNextStepMachineCycles(void) override;

		//Uudelleenmääritelty FLASHIN alue 0x3FC0 - 0x3FFF ei käytössä.
		unsigned char readInstructionDoCycle(bool const readOnlyInstruction) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;


	protected:
		void reset_VirtualPart(void) override;

		//Uudelleenmääritellyt käskyjen käsittelyt: 
		void add_A_DIRECT(unsigned int const directAddress) override;
		void addc_A_DIRECT(unsigned int const directAddress) override;
		void subb_A_DIRECT(unsigned int const directAddress) override;
		void inc_DIRECT(unsigned int const directAddress) override;
		void dec_DIRECT(unsigned int const directAddress) override;
		void inc_DPTR(void) override;
		void anl_A_DIRECT(unsigned int const directAddress) override;
		void anl_DIRECT_A(unsigned int const directAddress) override;
		void mov_DIRECT_RNx(unsigned char const instruction, unsigned int const directAddress) override;
		void anl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data) override;
		void orl_A_DIRECT(unsigned int const directAddress) override;
		void orl_DIRECT_A(unsigned int const directAddress) override;
		void orl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data) override;
		void xrl_A_DIRECT(unsigned int const directAddress) override;
		void xrl_DIRECT_A(unsigned int const directAddress) override;
		void xrl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data) override;
		void mov_A_DIRECT(unsigned int const directAddress) override;
		void mov_RNx_DIRECT(unsigned char const instruction, unsigned int const directAddress) override;
		void mov_DIRECT_A(unsigned int const directAddress) override;
		void mov_DIRECT_DIRECT(unsigned int const srcDirectAddress, unsigned int const dstDirectAddress) override;
		void mov_DIRECT_RIx(unsigned char const instruction, unsigned int const directAddress) override;
		void mov_DIRECT_DATA(unsigned int const directAddress, unsigned int const data) override;
		void mov_RIx_DIRECT(unsigned char const instruction, unsigned int const directAddress) override;
		void mov_DPTR_DATA16(unsigned char const dph, unsigned char const dpl) override;
		void pop_DIRECT(unsigned int const directAddress) override;
		void push_DIRECT(unsigned int const directAddress) override;
		void xch_A_DIRECT(unsigned int const directAddress) override;
		void cjne_A_DIRECT_REL(unsigned int const directAddress, char const relAddress) override;
		void djnz_DIRECT_REL(unsigned int const directAddress, char const relAddress) override;

		void handlePeripherals_VirtualPart(void) override;
		//Sarjaportti uudelleenmääritys: Lisätty ajastuksen muutokset ja lisätty PORTTI 1.
		SerialPort* uart1_;
		virtual void serialPortRxTxHandling(void) override;
		//External cyclen stepin loppuhommat uart1 lisäys
		virtual void externalCycleAddsEndOfStep(void) override;
		virtual void timersAndCountersHandling(void) override;

		//Uudellen määritetyt keskeytyskäsittelijät. Lisätty:
		//-Serial port 1
		//-ADC
		//-FLASH
		//-WDT
		//Uudellen määritetyt keskeytyskäsittelijät
		unsigned int priority0IntCheck(void) override;
		unsigned int priority1IntCheck(void) override;
		bool putInterruptServiceOn(unsigned int const interruptToPutOn) override;
		//Sekä keskytyksistä paluut
		bool priority0IntReturn(void) override;
		bool priority1IntReturn(void) override;
		//Ulkoisten keskytysten uudelleenmääritys, koska eri pinnit kuin kantaproskussa.
		bool readExternalIntPin(char const exInterrupt) override;

		IOPinList::PinIndex pinINT0_Index_;
		IOPinList::PinIndex pinINT1_Index_;
		IOPinList::PinIndex pinPWMA_Index_;
		IOPinList::PinIndex pinPWMB_Index_;
		int aPinAIN0_Index_;
		int aPinAIN1_Index_;
		int aPinAIN2_Index_;
		int aPinAIN3_Index_;
		int aPinAIN4_Index_;
		int aPinAIN5_Index_;
		int aPinAIN6_Index_;
		int aPinAIN7_Index_;
		int aPinACOM_Index_;
		int aPinVREFP_Index_;
		int aPinVREFN_Index_;

	private:
		MAX765x();

		void createSFRBitNameTable(void); //SFR bittinimien asetus/enablointi
		void createSFRNameTable(void); //SFR rekkarien nimien asetus/enablointi

		void createPins(void);//Luodaan yksittäiset IO pinnit

		void changeSomeInstructionsMachineCycles(void); //Muutetaan konstruktorissa joidenkin käskyjen machineCycle, jotka eri kuin kanatprosussa

		//Oheislaitteiden handlays:
		//Yhteiset handlayksen jutut
		I8031_NameS::clockTicks_Type lastPeriodClockTicksInHandlingPeripherals_;
		//WATCHDOG
		unsigned long watchdogCounter_;
		unsigned long counterValueAtPeriodReached_;
		bool watchdogPeriodReached_;
		bool prieviousWTRF_;
		void handlingWatchdog(void);
		//AD
		bool aDConversionIsOn_; //Merkki AD-muunnoksen käynnistymisestä
		bool aDConversionIsOnPrevValue_;   //Edellisen stepin arvo voidaan katsoa päälle meno
		char inputSelect_;   //Inputin valinta tapahtuu AD/ muunnoksen käynnistyksen yhteydessä
		int conversionCounter_;
		void handlingADC(void);
		//PWM
		void handlingPWM(void);
		unsigned int pwmCounter_;
		unsigned int modulo255Counter_;
		//Flash read/write/erase
		void handlingFlashOperations(void);
		unsigned char flashCommandOfEESTCMD_; //CPU lukee commandin, joka on kirjoitettavissa EESTCMD rekkarin kautta.
		unsigned char flashStatusOfEESTCMD_;   //CPU kirjottaa tähän Flashin statuksen joka on luettavissa EESTCMD:n kautta.
		int flashTimeCounter_;

};

#endif
