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
//KANTALUOKKA:::

#ifndef I8031_H
#define I8031_H

#include "../share/conversium.h"
#include "InstructionSet.h"
#include "InternalRAM.h"
#include "IOPinList.h"
#include "ExternalCycle.h"
#include "SerialPort.h"
#include "TimersCounters.h"
#include "I8031_NameS.h"

#include <cmath>
#include <string>
#include <cctype>

#include <qdatastream.h>

class ExternalCycle;

class I8031
{
	public:
		I8031();
		virtual ~I8031();

		static I8031_NameS::clockTicks_Type const DEFAULT_CLOCKSPEED=11059200; //Oletuskello

		//unsigned char numberOfPorts(void){return portsNumber;};

		//Toiminnan ohjaus
		void powerOnReset(void);      //Tehdään syväresetti, clearaa kaikki alueet
		bool isAtReset(void){return resetState;};

		//Vaihdetaan portin pinnin tila
		virtual void togglePortPin(unsigned int const portAddress, unsigned int const pinNumber);
		virtual int readPortPin(unsigned char const portAddress, unsigned char const pinNumber);//-1 ei portti
		virtual void writePortPin(unsigned char const portAddress, unsigned char const pinNumber, bool const value);

		//Luetaan erikoispinnin arvo
		inline IOPinList::PinIndex readPinIndex(string name){return pinList_.readIndex(name);};
		inline bool readPinValue(IOPinList::PinIndex const index){return pinList_.readValue(index, true);};
		inline IOPin::PinID readPinID(string name){return pinList_.readID(name);};
		inline bool readPinValue(IOPin::PinID const id){return pinList_.readValue(id, true);};
		//Luetaan koko erikoispinnilista
		IOPinList* readPins(void);
		//Kirjoitetaan inputti erikoispinniin
		void writeInputPin(string name, bool pinValue);
		//Vaihdetaan erikoispinnin tila jos on inputti
		void toggleInputPin(string name);


		//Kaikki ohjaus tapahtuu askelissa.
		//Kirjoittaa käskyjonon,jos dissassemble päällä.
		//Ei suorita käskyjä, vaan disassembloi ohjelman jos run ei ole päällä
		//Ohjelman suorituksessa paluttaa virheilmoituksen käskyjonolla , jos on erroreita, eihän?.
		virtual void step(bool const disassemble, bool const run);

		//Laskee seuraavana olevan käskyn koejaksot.
		//Saadaan myös seuraava käsky code bufferista, jos pistetään disassemble päälle.
		virtual I8031_NameS::machineCycles_Type calculateNextStepMachineCycles(void);

		//Ohjelmamuisti
		virtual void writeProgramMemory(unsigned int const address, unsigned char const value);
		virtual void clearProgramMemory(void);
		virtual unsigned char readExternalProgramMemory(unsigned int const address) const;
		//Ohjelmamuistista haku: Hakee PC:n osoittamasta paikasta ja lisää PC:a jos readOnlyInstruction=false. Tekee myös external cyclen ja oheislaithandlayksen.
		virtual unsigned char readInstructionDoCycle(bool const readOnlyInstruction);
		//PC:n arvo
		I8031_NameS::programCounter_Type readProgramCounterValue(void){return programCounter_;};
		I8031_NameS::programCounter_Type readLastExecutionProgramCounterValue(void){return lastExecutionProgramCounterValue_;};
		void setProgramCounterValue(I8031_NameS::programCounter_Type value){programCounter_=value;}; //Varo käyttöä

		
		//Ulkoinen datamuistialue
		void clearExternalDataMemory(void);
		void writeExternalDataMemory(unsigned int const address, unsigned char const value);
		unsigned int readExternalDataMemory(unsigned int const address);
		bool isExternalDataMemoryChanged(unsigned int const address);
		bool isExternalDataMemoryChangedAfterClear(unsigned int const address);
		//Voidaan katsoa oliko ulkoiseen muistiinkirjoitustoiminto edellisessä stepissä.
		bool is16bAddressExternalDataMemoryWriteCycleLastStep(void){return movx16bAWriteCycle_;};
		bool is8bAddressExternalDataMemoryWriteCycleLastStep(void){return movx8bAWriteCycle_;};
		unsigned char readAddressInP0AfterALE(void){return addressInP0AfterALE_;};
		unsigned char readAddressInP2AfterALE(void){return addressInP2AfterALE_;};

		void writeInternalDataMemory(unsigned int const address, unsigned char const value);
		unsigned char readInternalDataMemory(unsigned int const address);
		bool isInternalDataMemoryLocationChanged(unsigned int const address);
		bool isInternalDataMemoryLocationChangedAfterClear(unsigned int const address);

		//Onko jokin sisäisen rammin muistipaikka käytössä
		bool isInternalDataMemoryLocationInUse(unsigned int const address);
		//Onko SFR:n muistipaikan bittinimet käytössä
		bool isSFRBitNamesInUse(unsigned char const sfrAddress);
		//Onko SFR rekkari portti
		bool isSFRRegisterIOPort(unsigned char const sfrAddress);

		//Käytetty rekisteripankki palauttaa 0, 1, 2 tai 3
		unsigned char registerBankInUse(void);

		//Paluttaa DPTR:n 16 bittisenä. Uudelleenmääriteltävänä 2 DPTR:n processoreissa
		virtual unsigned int readDPTRValue(void);
		//Luetaan Timerin arvo 16-bittisenä
		virtual unsigned int readTimerValue16(char const timerNumber);

		//Sarjaportti. Otettu huomioon perivien luokkien useammat sarjaportit
		virtual bool isSerialPortTaransmitOn(unsigned char const port);
		virtual int readSerialPortTransmittedValue(unsigned char const port);
		virtual void charToSerialPortReceiveBuff(unsigned char const port, int const data);
		virtual int readSerialPortRxBits(unsigned char const port);
		virtual int readSerialPortTxBits(unsigned char const port);

		//Muita sallittuja metodeja
		//Lukee codeBuffer merkkijonon, paluttaa sen osoitteen
		//Muotoa "AAAA:VV[VVVV] INSTRUCTION[:ERCODE]"
		string readCodeBufferStr(void) {return codeBufferStr_;};
		//Ladatun ohjelman koko tavuissa.
		unsigned int readProgramSize(void) const {return programSize_;};
		//Processorin nimi
		string readProcessorName(void){return processorName_;};
		//Kellotaajuus
		I8031_NameS::clockTicks_Type readClockSpeed(void){return clockSpeed_;};
		//Clock cycles per machine cycle
		I8031_NameS::clockTicks_Type readClockCyclesPerMachineCycle(void){return clockCyclesPerMachineCycle_;};
		//Machine cycles
		I8031_NameS::machineCycles_Type readMachineCycles(void){return machineCycleCounter_;};
		//ClockTicks
		I8031_NameS::clockTicks_Type readClockTicks(void){return clockTicksCalculated_;};
		//Viimeisn external code tai data Memorycycle jos oli konejaksossa. Nollautuu aina joka stepin alussa.
		ExternalCycle *readLastExternalCycle(void){return externalCycleList_;};

		string readSFRName(unsigned char sfrAddress);
		string readSFRBitName(unsigned char sfrAddress, unsigned char bitNum);
		int readSFRAddress(string const name);//Voidaan lukea nimen perusteella osoite

		void changeClockSpeed(I8031_NameS::clockTicks_Type speed);

		bool isInstruction(char const *instStr)
		{
			return instructions_->isInstruction(instStr);
		};

		//Tietojen tallennus. Ei ANSI mukainen vaan QT
		//Pitää määritellä perivissä, jos tallenetaan niiden ominaisuuksien tiloja, mutta kutsuttava myös tätä.
		virtual void saveWorkDataToFile(QDataStream& saveFileStream);
		virtual void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);


	//------------------------------ OBJEKTIT, METODIT ---------------------------------
	protected:

		string processorName_;
		I8031_NameS::clockTicks_Type maxClockSpeed_;
		I8031_NameS::clockTicks_Type clockSpeed_;      //Hz
		I8031_NameS::clockTicks_Type clockCyclesPerMachineCycle_;   //Yhden konejakson kellojaksojen määrä.
		I8031_NameS::clockTicks_Type clockCyclesClockTicksCounted_; //Kellojaksojen määrä laskettaessa kellopulsseja

		static I8031_NameS::clockTicks_Type const MAX_CLOCKSPEED=12000000;

		ExternalCycle* externalCycleList_;   //Viimeisin ulkoisen osoituksen jakso.
		virtual void externalCycleAddsEndOfStep(void);
		
		//Erikoispinnilistat
		IOPinList pinList_;
		IOPinList::PinIndex pinALE_Index_;
		IOPinList::PinIndex pinPSEN_Index_;
		IOPinList::PinIndex pinRESET_Index_;
		IOPinList::PinIndex pinEA_Index_;

		InstructionSet *instructions_; //

		//8031:n PC ,long koska tutkitaan ylivuotoa
		I8031_NameS::programCounter_Type programCounter_;
		I8031_NameS::clockTicks_Type clockTicksCalculated_; //Kellojen tikitysmäärä yhteensä
		I8031_NameS::clockTicks_Type clockTicksCounted_;
		
		I8031_NameS::alePulses_Type oneStepAlePulses_; //Lasketaan ALE pulssit. 2 pulssia = MachineCycle tällä prskulla.
		I8031_NameS::machineCycles_Type oneStepMachineCycles_; //Yhteen Käskyyn käytetyt konejaksot stepin aikana

		//Ajettavan ohjelman koko.
		//Tallettuu suurimman ohjelmamuistiin kirjoitetun osoitteen perusteella.
		unsigned int programSize_;

		//Ulkoinen ohjelmamuisti:
		unsigned char externalProgramMemory_[I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE];

		//Sisäinen RAM alue jossa SFR, Rekisteripankit ja Bit alue + Stack
		InternalRAM internalDataMemory_[I8031_NameS::INTERNAL_DATA_MEMORY_SIZE];

		//Ulkoinen RAM
		Memory externalDataMemory_[I8031_NameS::EXTERNAL_DATA_MEMORY_SIZE];

		//Asetetaan SFR rekisteri käyttöön
		void setSFRInUse(unsigned char const reg, char const* name, bool const isPort);
		//Asetetaan SFR rekisterin bittinimi käyttöön
		void setSFRBitNameInUse(unsigned char const reg, unsigned char const bit, char const* bitname);

		//Merkki ulkoiseen muitiin kirjoituksesta. Päällä vain sen Stepin jälkeen.
		//P0 ja/tai P2 ilmaisevat osoitteen:
		bool movx16bAWriteCycle_;
		bool movx8bAWriteCycle_;

		//Interruptien käsittely.
		//Keskeytysten käsittely päällä
		unsigned int priority0IntON_;
		unsigned int priority1IntON_;
		virtual unsigned int priority0IntCheck(void);
		virtual unsigned int priority1IntCheck(void);
		virtual bool putInterruptServiceOn(unsigned int const interruptToPutOn);//Param: can use inherit
		virtual bool priority0IntReturn(void);
		virtual bool priority1IntReturn(void);
		//Ulkoisen keskytyksen tarkistus
		virtual bool readExternalIntPin(char const exInterrupt); //(UM) koska perivillä voi olla eri pinnit
		virtual void lookIfExternalInterrupts(void); //(UM) koska perivillä voi olla eri määrä ExInttejä
		bool previousExternalInt0Pin_;
		bool previousExternalInt1Pin_;

		//Keskeytys käynnistetty merkki, yhden stepin päällä.
		//Voidaan käyttää perivissä luokissa IDLE moden poistoon.
		bool interruptStarted_;
		//Merkki keskytyksestä paluusta: Käytetään single step operaatiossa
		bool interruptDone_;

		//Ajoon liittyvää
		bool disassembleIsOn_;

		//Uudelleenmääritettävät epäsuorat sisäisen muisttin haut
		virtual unsigned int readDataFromInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r);
		virtual void writeDataToInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r, unsigned int val);

		virtual void reset_VirtualPart(void){};
		virtual void powerOnReset_VirtualPart(void){};
		void reset_protected(void){reset();}; //Perovät prosut voi resetoida tällä

		//8051/31Käskyjen käsittely. Voidaan uudelleenmäärittää perivässä luokassa.
		//Viimeisenä parametrinä viedään tieto jos lasketaan vain konejaksoja. Silloin ei käskyä suoriteta.
		//Edellinen otettava huomioon perivässä luokassa uudelleenmääritettäessä käskyn suoritusta.
		//Arithmetic operations
		virtual void add_A_RNx(unsigned char const instruction);
		virtual void add_A_DIRECT(unsigned int const directAddress);
		virtual void add_A_RIx(unsigned char const instruction);
		virtual void add_A_DATA(unsigned int const data);
		virtual void addc_A_RNx(unsigned char const instruction);
		virtual void addc_A_DIRECT(unsigned int const directAddress);
		virtual void addc_A_RIx(unsigned char const instruction);
		virtual void addc_A_DATA(unsigned int const data);
		virtual void subb_A_RNx(unsigned char const instruction);
		virtual void subb_A_DIRECT(unsigned int const directAddress);
		virtual void subb_A_RIx(unsigned char const instruction);
		virtual void subb_A_DATA(unsigned int const data);
		virtual void inc_A(void);
		virtual void inc_RNx(unsigned char const instruction);
		virtual void inc_DIRECT(unsigned int const directAddress);
		virtual void inc_RIx(unsigned char const instruction);
		virtual void inc_DPTR(void);
		virtual void dec_A(void);
		virtual void dec_RNx(unsigned char const instruction);
		virtual void dec_DIRECT(unsigned int const directAddress);
		virtual void dec_RIx(unsigned char const instruction);
		virtual void mul_AB(void);
		virtual void div_AB(void);
		virtual void da_A(void);
		//Logical operations
		virtual void anl_A_RNx(unsigned char const instruction);
		virtual void anl_A_DIRECT(unsigned int const directAddress);
		virtual void anl_A_RIx(unsigned char const instruction);
		virtual void anl_A_DATA(unsigned int const data);
		virtual void anl_DIRECT_A(unsigned int const directAddress);
		virtual void anl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data);
		virtual void orl_A_RNx(unsigned char const instruction);
		virtual void orl_A_DIRECT(unsigned int const directAddress);
		virtual void orl_A_RIx(unsigned char const instruction);
		virtual void orl_A_DATA(unsigned int const data);
		virtual void orl_DIRECT_A(unsigned int const directAddress);
		virtual void orl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data);
		virtual void xrl_A_RNx(unsigned char const instruction);
		virtual void xrl_A_DIRECT(unsigned int const directAddress);
		virtual void xrl_A_RIx(unsigned char const instruction);
		virtual void xrl_A_DATA(unsigned int const data);
		virtual void xrl_DIRECT_A(unsigned int const directAddress);
		virtual void xrl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data);
		virtual void clr_A(void);
		virtual void cpl_A(void);
		virtual void rl_A(void);
		virtual void rlc_A(void);
		virtual void rr_A(void);
		virtual void rrc_A(void);
		virtual void swap_A(void);
		//Data transfer
		virtual void mov_A_RNx(unsigned char const instruction);
		virtual void mov_A_DIRECT(unsigned int const directAddress);
		virtual void mov_A_RIx(unsigned char const instruction);
		virtual void mov_A_DATA(unsigned int const data);
		virtual void mov_RNx_A(unsigned char const instruction);
		virtual void mov_RNx_DIRECT(unsigned char const instruction, unsigned int const directAddress);
		virtual void mov_RNx_DATA(unsigned char const instruction, unsigned int const data);
		virtual void mov_DIRECT_A(unsigned int const directAddress);
		virtual void mov_DIRECT_RNx(unsigned char const instruction, unsigned int const directAddress);
		virtual void mov_DIRECT_DIRECT(unsigned int const srcDirectAddress, unsigned int const dstDirectAddress);
		virtual void mov_DIRECT_RIx(unsigned char const instruction, unsigned int const directAddress);
		virtual void mov_DIRECT_DATA(unsigned int const directAddress, unsigned int const data);
		virtual void mov_RIx_A(unsigned char const instruction);
		virtual void mov_RIx_DIRECT(unsigned char const instruction, unsigned int const directAddress);
		virtual void mov_RIx_DATA(unsigned char const instruction, unsigned int const data);
		virtual void mov_DPTR_DATA16(unsigned char const dph, unsigned char const dpl);
		virtual void movc_A_A_DPTR(void);
		virtual void movc_A_A_PC(void);
		virtual void movx_A_RIx(unsigned char const instruction);
		virtual void movx_RIx_A(unsigned char const instruction);
		virtual void movx_A_DPTR(void);
		virtual void movx_DPTR_A(void);
		virtual void pop_DIRECT(unsigned int const directAddress);
		virtual void push_DIRECT(unsigned int const directAddress);
		virtual void xch_A_RNx(unsigned char const instruction);
		virtual void xch_A_DIRECT(unsigned int const directAddress);
		virtual void xch_A_RIx(unsigned char const instruction);
		virtual void xchd_A_RIx(unsigned char const instruction);
		//Boolean variable manipulation
		virtual void clr_C(void);
		virtual void clr_BIT(unsigned char const bitAddress);
		virtual void setb_C(void);
		virtual void setb_BIT(unsigned char const bitAddress);
		virtual void cpl_C(void);
		virtual void cpl_BIT(unsigned char const bitAddress);
		virtual void anl_C_BIT(unsigned char const bitAddress);
		virtual void anl_C_NBIT(unsigned char const bitAddress);
		virtual void orl_C_BIT(unsigned char const bitAddress);
		virtual void orl_C_NBIT(unsigned char const bitAddress);
		virtual void mov_C_BIT(unsigned char const bitAddress);
		virtual void mov_BIT_C(unsigned char const bitAddress);
		virtual void jc_REL(char const relAddress);
		virtual void jnc_REL(char const relAddress);
		virtual void jb_BIT_REL(unsigned char const bitAddress, char const relAddress);
		virtual void jnb_BIT_REL(unsigned char const bitAddress, char const relAddress);
		virtual void jbc_BIT_REL(unsigned char const bitAddress, char const relAddress);
		//Program branching
		virtual void acall_x00(unsigned char const instruction, unsigned int const addressLow);
		virtual void lcall(unsigned char const addressHigh, unsigned char const addressLow);
		virtual void ret(void);
		virtual void reti(void);
		virtual void ajmp_x00(unsigned char const instruction, unsigned int const addressLow);
		virtual void ljmp(unsigned char const addressHigh, unsigned char const addressLow);
		virtual void sjmp(char const relAddress);
		virtual void jmp_A_DPTR(void);
		virtual void jz_REL(char const relAddress);
		virtual void jnz_REL(char const relAddress);
		virtual void cjne_A_DIRECT_REL(unsigned int const directAddress, char const relAddress);
		virtual void cjne_A_DATA_REL(unsigned char const data, char const relAddress);
		virtual void cjne_RNx_DATA_REL(unsigned char const instruction, unsigned char const data, char const relAddress);
		virtual void cjne_RIx_DATA_REL(unsigned char const instruction, unsigned char const data, char const relAddress);
		virtual void djnz_RNx_REL(unsigned char const instruction, char const relAddress);
		virtual void djnz_DIRECT_REL(unsigned int const directAddress, char const relAddress);

		virtual void pop_pc(void); //Tarvitaan keskeytysten käsittelyssä
		virtual void push_pc(void);//Ja aliohjelmissa

		void addClockTicksAndHandlePeripherals(I8031_NameS::alePulses_Type ales); //Kaikkien oheispiirien handlays. Lisää myös clockTicksCounted_ arvoa. Tehdään jokaisella ale pulssilla.
		virtual void handlePeripherals_VirtualPart(void){};//Virtuaaliosa periviä varten
		//Timerit Counterit
		TimersCounters *timersCounters_;
		virtual void timersAndCountersHandling(void);
		//Sarjaportti
		SerialPort* uart_;
		virtual void serialPortRxTxHandling(void);

		//8051 käskyjen käsittely joka on määritettävä uudestaan ominaisuuksien mukkan
		virtual void push(unsigned char dpush);
		virtual void pop(unsigned char dpop);

		//Lisätään perivistä luokista koodimerkkijonoon virheilmoituksen
		void addErrorToCodeBufferStr(string strr)
		{if(disassembleIsOn_) codeBufferStr_+=strr;};

		//Mahdollistaa perivien luokkien oheispiiritoimintojen käsittelyn samalla konejaksolla
		//Timereitten yms. kanssa:
		//On mahdollista uudellenmäärittää timerien rutiinit, mutta tämä on selvempi
		//Ei tee tässä proskussa mitään:
//      virtual void handlingRoutinesBeginningOfStep(void){};
	//   virtual void handlingRoutinesEndOfStep(void){};

	private:

		//Nimitaulukot. Ei ole laitettu internaRAM luokkaan tilan säästämiseksi.
		//Bittiosoitettavien bittien nimet.
		//Käytetään disassemblessa.
		//Bittiosoitettavan alueen bittien nimet:
		//Pitää ladata ohjelmakohtaisesti mnemonikeista jos haluaa käyttää. Muutoin käytössä oletusnimet, eli bittien luvut:
		static unsigned int const BITAREANAMETABLESIZE=128;
		string bitAreaNameTable_[BITAREANAMETABLESIZE];
		//SFR alueen rekistereiden bittien nimitaulukko.
		static unsigned int const SFRBITNAMETABLESIZE=1024;
		string sfrBitNameTable_[SFRBITNAMETABLESIZE];
		//SFR rekisterien nimet.
		static unsigned int const SFRNAMETABLESIZE=128;
		string sfrNameTable_[SFRNAMETABLESIZE];

		//Yksittäisten pinnien listarakenteen ekapinnin merkki.
		void createPins(void);

		//Jos processori resettitilassa
		bool resetState;
		void reset(void);

		//Processorin käyttämä aika:
		//clock_t machineTime;
		//clock_t lastMachineTime;

		//jokaisen stepin virhe tai dissassemble moodissa dissassemble koodi
		string codeBufferStr_;

		//Askelluksen private osa.
		//disassemble= tekee käskyjonon. Yksistään ei suorita käskyä.
		//run = suorittaa käskyn.
		//machineCycleCalculation = laskee seuraavan käskyn konejaksot. Sulkee pois runin: ei muuta mitään. Käytetään sisäisesti.
		//Palauttaa käytetyt konejaksot
		unsigned int doStep(bool const disassemble, bool const run);

		I8031_NameS::machineCycles_Type machineCycleCounter_;       //Tällä lasketaan käytettyjen konejaksojen määrä.

		unsigned char addressInP0AfterALE_; //portin 0 ale jälkeinen tila ulkoisen datamuistin osoituksessa.
		unsigned char addressInP2AfterALE_; //portin 2 ale jälkeinen tila ulkoisen datamuistin osoituksessa.

		void formatInternalDataMemory(void);
		void createSFRBitNameTable(void);
		void createSFRNameTable(void);

		//Interruptien käsittely.
		void handlingInterrupts(void); //Joka stepillä tutkitaan
		void handlingInterruptReturn(void); //reti käskyllä
		
		//Käskyjen käsittelyjä
		void setParityFlag(void);      //PSW:n pariteettibitin asetus joka kierrolla
		void addSetFlags(unsigned int a, unsigned int b, bool on_c);
		void subbSetFlags(unsigned int a, unsigned int b);
		unsigned int readDataFromRBRegisterDirectly(unsigned int const r);
		void writeDataToRBRegisterDirectly(unsigned int const r, unsigned int const val);
		unsigned char bitAddressToSFRAddress(unsigned char const bitAddress); //Antaa bittiosoitteella SFR osoitteen
		void manipulateDirectBitWithBitAddress(unsigned char const bitAddress, unsigned char const manipulation);
		void manipulateCarryWithBitAddress(unsigned char const bitAddress, unsigned char const manipulation);
		void jumpBitManipulation(unsigned char const bitAddress, char const relAddress, unsigned char const manip);

		//Disassemblea varten luetaan bittiosoitettavat nimet.
		string readBitAddressableBitName(unsigned char bitAddress);//Bit area ja SFR

		//Tarkistuksia
		inline void isSFROn(unsigned char regAddress);
		inline void isSFRBitOn(unsigned char bit);

		void clearBitAddressableArea(void);
		void clearRegisterBanks(void);
		void clearScratchPadArea(void);

		I8031_NameS::programCounter_Type lastExecutionProgramCounterValue_;
		I8031_NameS::machineCycles_Type lastExecutionMachineCycleCounterValue_;

};

#endif
