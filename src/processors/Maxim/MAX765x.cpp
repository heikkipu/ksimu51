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
#include "MAX765x.h"
#include "../../share/conversium.h"
#include "../../ksimu51.h"


//Muodostin
MAX765x::MAX765x(unsigned long const flashSize) : I80C52(), I8031_ADConverter(), I8051_Flash(flashSize)
{
	using namespace MAX765x_NameS;
	//Changes to base 8031
	processorName_="MAX765x";
	clockCyclesPerMachineCycle_=4;
	clockCyclesClockTicksCounted_=4; //Tässä prosessorissa sama kuin konejakson kellot
	createSFRBitNameTable();
	createSFRNameTable();
	createPins();
	changeSomeInstructionsMachineCycles();
	delete externalCycleList_;
	pinINT0_Index_=pinList_.readIndex(PINNAME_INT0);
	pinINT1_Index_=pinList_.readIndex(PINNAME_INT1);
	pinPWMA_Index_=pinList_.readIndex(PINNAME_PWMA);
	pinPWMB_Index_=pinList_.readIndex(PINNAME_PWMB);
	externalCycleList_=new MAX765x_ExternalCycle(pinALE_Index_, pinPSEN_Index_, pinRESET_Index_, pinEA_Index_, pinINT0_Index_, pinINT1_Index_, pinPWMA_Index_, pinPWMB_Index_, P0, P1, P2, P3, &internalDataMemory_[CKCON]);
	
	SerialPort::RegSCONBits bitsToSCON;
	bitsToSCON.bitRI=SCON1_RI;
	bitsToSCON.bitTI=SCON1_TI;
	bitsToSCON.bitRB8=SCON1_RB8;
	bitsToSCON.bitTB8=SCON1_TB8;
	bitsToSCON.bitREN=SCON1_REN;
	bitsToSCON.bitSM2=SCON1_SM2;
	bitsToSCON.bitSM1=SCON1_SM1;
	bitsToSCON.bitSM0=SCON1_SM0;
	SerialPort::RxTxPins pinsRxTx;
	pinsRxTx.pinTx=P1_TXD1;
	pinsRxTx.pinRx=P1_RXD1;
	uart1_=new SerialPort(&internalDataMemory_[SCON1], bitsToSCON, &internalDataMemory_[SBUF1], &internalDataMemory_[P1], pinsRxTx);
	internalDataMemory_[SBUF1]=0x0;    //Alustetaan tässä, resetissä Indeterminate

	delete timersCounters_;
	timersCounters_=new MAX765x_TimersCounters(&internalDataMemory_[TL0], &internalDataMemory_[TH0], &internalDataMemory_[TL1], &internalDataMemory_[TH1], &internalDataMemory_[TMOD], &internalDataMemory_[TCON], &internalDataMemory_[IE], &internalDataMemory_[P3], &internalDataMemory_[TL2], &internalDataMemory_[TH2], &internalDataMemory_[RCAP2L], &internalDataMemory_[RCAP2H], &internalDataMemory_[T2CON], &internalDataMemory_[P1], &internalDataMemory_[CKCON]);

	clearProgramMemory();
}

//Hajotin
MAX765x::~MAX765x()
{
}

void MAX765x::reset_VirtualPart(void)
{
	using namespace MAX765x_NameS;
	//Kutsutaan kantaluokan resettiä
	I80C52::reset_VirtualPart();
	I8031_ADConverter::reset_VirtualPart();
	I8051_Flash::reset_VirtualPart();
	//SFR reset values
	internalDataMemory_[DPL1]=0x0;
	internalDataMemory_[DPH1]=0x0;
	internalDataMemory_[DPS]=0x0;
	internalDataMemory_[PCON]=0x30; //00110000b Eri arvo kuin kantaproskussa
	internalDataMemory_[CKCON]=0x01; //00000001b Yksi lisäjakso Stretc memorycyclessä resetin jälkeen.
	internalDataMemory_[EXIF]=0x08; //00001000b
	internalDataMemory_[IP]=0x80;   //10000000b Eri kuin kantaproskussa
	internalDataMemory_[SCON1]=0x0;
	//internalDataMemory_[SBUF1]=0x0;Indeterminate??
	internalDataMemory_[ADDAT0]=0x0;
	internalDataMemory_[ADDAT1]=0x0;
	internalDataMemory_[ADCON]=0x0;
	internalDataMemory_[EICON]=0x40;//01000000b
	internalDataMemory_[PWPS]=0x0;
	internalDataMemory_[PWDA]=0x0;
	internalDataMemory_[PWDB]=0x0;
	internalDataMemory_[WDT]&=(unsigned char)WDT_WTRF;   //Vahtikoiran resetti jättää päälle
	internalDataMemory_[EIE]=0xE0;   //11100000b
	internalDataMemory_[EEAL]=0x0;
	internalDataMemory_[EEAH]=0x0;
	internalDataMemory_[EEDAT]=0x0;
	internalDataMemory_[EESTCMD]=0x0;
	internalDataMemory_[EIP]=0xE0; //11100000b
	internalDataMemory_[PWMC]=0x0;
	//Int
	//Watchdog
	watchdogCounter_=0;
	watchdogPeriodReached_=false;
	counterValueAtPeriodReached_=0;
	//AD
	aDConversionIsOn_=false;
	conversionCounter_=0;
	//PWM
	pwmCounter_=0;
	modulo255Counter_=0;
	//Flash
	flashCommandOfEESTCMD_=0;
	flashStatusOfEESTCMD_=0xC0;   //Kaksi ylintä bittiä ykköseksi, jotta Flash operaatiot sallittuja

	lastPeriodClockTicksInHandlingPeripherals_=clockTicksCounted_;
	
}

//Luodaan yksittäiset pinnit mitä ei löydy kantaluokista.
void MAX765x::createPins(void)
{
	//Yksittäise I/O pinnit
	//Lisätään ensin INTIT
	//Asetetaan  ylös alussa koska erilliset inputpinnit. open drain.
	//Muutoin näitten tila rippuu ohjauksesta. Molemmat vain inputteja
	pinList_.addPin(PINNAME_INT0, true, IOPin::PINTYPE_INPUT);
	pinList_.addPin(PINNAME_INT1, true, IOPin::PINTYPE_INPUT);
	//PWM lähdöt, siis vain outputteja. Asetetaan nollaan aluksi
	pinList_.addPin(PINNAME_PWMA, false, IOPin::PINTYPE_OUTPUT);
	pinList_.addPin(PINNAME_PWMB, false, IOPin::PINTYPE_OUTPUT);
}

//Määritellään uudestaan koska FLASH alueella 0x3FC0 - 0x3FFF ei käytettävissä
//Aiheuttaa virheilmoituksen ja paluun noppina.
unsigned char MAX765x::readInstructionDoCycle(bool const readOnlyInstruction)
{
	//Jos sisäinen muisti käytössä katsotaan kielletty alue
	if((pinList_.readValue(pinEA_Index_, true)) && ((programCounter_>=0x3FC0) && (programCounter_<0x4000)))
	{
		string ercode("FLASH address: ");
		ercode+=hexToString(programCounter_, 4);
		ercode+="H not CPU accessable! ";
		addErrorToCodeBufferStr(ercode);
		//return 0; Aiheuttaa external cycleen ongelmia
	}
	return I8052::readInstructionDoCycle(readOnlyInstruction); //Kantaluokasta
}

I8031_NameS::machineCycles_Type MAX765x::calculateNextStepMachineCycles(void)
{
	using namespace MAX765x_NameS;
	int rv;
	rv=I80C52::calculateNextStepMachineCycles();


	//Konejakson Stretch toiminto movx käskyssä.
	//MAXISSA 2-9 konejaksoa riippuen CKCON rekkarin 3:sta alimmasta bitistä. 8031:ssä aina 2 konejaksoa.
	//Tässä 0-7 lisäjaksoa.
	//-movx_A_RIx
	//-movx_RIx_A
	//-movx_A_DPTR
	//-movx_DPTR_A
	switch(readInstructionDoCycle(READ_ONLY_INSTRUCTION))
	{
		case InstructionSet_NameS::MOVX_A_RI0:
		case InstructionSet_NameS::MOVX_A_RI1:
		case InstructionSet_NameS::MOVX_RI0_A:
		case InstructionSet_NameS::MOVX_RI1_A:
		case InstructionSet_NameS::MOVX_A_DPTR:
		case InstructionSet_NameS::MOVX_DPTR_A:
			rv+=(internalDataMemory_[CKCON].readValue()&0x07);
	}
	//programCounter_--; //Korjataan PC kohallen codemuistista luvun jälkeen

	return rv;
}

//Uudelleenmääritellyt käskyt:
//Muutokset:
//Serial port 1
//AD-konversio
//Flash command/status?
void MAX765x::add_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.??
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::add_A_DIRECT(directAddress); //Kutsuaan kantaluokan ohjelma

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//AD- konversio
//Serial port 1
//Flash command/status?
void MAX765x::addc_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());
	//Jos Flash status rekkari haetaan tieto statuksesta.??
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::addc_A_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial port 1.
//AD- konversio
//Flash command/status?
void MAX765x::subb_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::subb_A_DIRECT(directAddress);
	
	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial port 1
//AD- konversion käynnistys
//Flash command/status?
void MAX765x::inc_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.??
	//if(directAddress==EESTCMD)
		//internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;


	//Jos luettiin ADDAT1 rekkarista ???????????????????????
	//if(directAddress==ADDAT1)
	//{
	//   internalDataMemory_[ADCON]&=~ADCON_CC;   //Nollataan Converison Complete
	//   internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	//}

	I80C52::inc_DIRECT(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	//Toimiikohan oikea prosu näin: en ole koskaan kokeillut
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oikea prosku näin!!!
	//if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		//aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
		//flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
}

//Muutokset:
//AD- konversion käynnistys
//Flash command/status?
void MAX765x::dec_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.??
	//if(directAddress==EESTCMD)
		//internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	//Jos luettiin ADDAT1 rekkarista
	//if(directAddress==ADDAT1)
	//{
	//   internalDataMemory_[ADCON]&=~ADCON_CC;   //Nollataan Converison Complete
	//   internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	//}

	I80C52::dec_DIRECT(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	//Toimiikohan oikea prosu näin: en ole koskaan kokeillut
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oikea prosku näin!!!
	//if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
	//   aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
		//flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
}

//Muutokset:
//Dual DPTR.
void MAX765x::inc_DPTR(void)
{
	using namespace MAX765x_NameS;
	//Katsotaan kumpi DPTR käytössä.
	if(internalDataMemory_[DPS].readValue()&0x01) //Jos DPTR 1
	{
		unsigned int apu=internalDataMemory_[DPL1].readValue();
		apu++;
		if(apu>0xFF)                           //Jos ylivuoto lisätään myös yläosaa DPTR:a
			++internalDataMemory_[DPH1];
		internalDataMemory_[DPL1]=apu&0x00FF;
	}
	else
		I80C52::inc_DPTR(); //DPTR 0 käytössä. Kantaluokan ohjelma
}


//Muutokset:
//AD- konversio
//Serial port 1
//Flash command/status?
void MAX765x::anl_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.??
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::anl_A_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial PORT1. Toiminta kyseenalainen pitäisi testata oikeella proskulla. Myös kantaluokassa.
//AD- konversion käynnistys?
//Flash command/status?
void MAX765x::anl_DIRECT_A(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	I80C52::anl_DIRECT_A(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	//Toimiikohan oikea prosu näin: en ole koskaan kokeillut
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oikee prosku näin!!!
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
	//   flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//Serial PORT1. Toiminta kyseenalainen pitäisi testata oikeella proskulla. Myös kantaluokassa.
//AD- konversion käynnistys. Samat sanat edellisen kanssa
//Flash command/status?
void MAX765x::anl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace MAX765x_NameS;
	
	I80C52::anl_DIRECT_DATA(directAddress, data);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto myös sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oike prosku näin!!!
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
	//   flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//AD-Konversio
//Flash command/status?
void MAX765x::orl_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.??
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::orl_A_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial PORT1. Toiminta kyseenalainen pitäisi testata oikeella proskulla. Myös kantaluokassa.
//AD- konversion käynnistys
//Flash command
void MAX765x::orl_DIRECT_A(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	I80C52::orl_DIRECT_A(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oike prosku näin!!!
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
	//   flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//Serial PORT1. Toiminta kyseenalainen pitäisi testata oikeella proskulla. Myös kantaluokassa.
//AD- konversion käynnistys ??
//Flash command/status?
void MAX765x::orl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace MAX765x_NameS;
	
	I80C52::orl_DIRECT_DATA(directAddress, data);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oike prosku näin!!!
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
	//   flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//AD- konversio
//Serial port 1
//Flash command/status?
void MAX765x::xrl_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.??
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::xrl_A_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial PORT1. Toiminta kyseenalainen pitäisi testata oikeella proskulla. Myös kantaluokassa.
//AD- konversin käynnistys.??
//Flash command/status?
void MAX765x::xrl_DIRECT_A(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	I80C52::xrl_DIRECT_A(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oike prosku näin!!!
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
	//   flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//Serial PORT1. Toiminta kyseenalainen pitäisi testata oikeella proskulla. Myös kantaluokassa.
//AD- konversion käynnistys
//Flash command/status?
void MAX765x::xrl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace MAX765x_NameS;
	
	I80C52::xrl_DIRECT_DATA(directAddress, data);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	//Ei ihan täyttä varmuutta toimiiko oike prosku näin!!!
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	//if(directAddress==EESTCMD)
	//   flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//Serial Port 1
//AD- konversio
//Flash command/status?
void MAX765x::mov_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::mov_A_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial port 1
//AD- konversio
//Flash command/status?
void MAX765x::mov_RNx_DIRECT(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::mov_RNx_DIRECT(instruction, directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial PORT1.
//AD- konversion käynnistys
//Flash command/status?
void MAX765x::mov_DIRECT_A(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	I80C52::mov_DIRECT_A(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio
	//Jos Flash command rekkari tieto commandiksi.
	if(directAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
}

//Muutokset:
//Serial PORT1.
//AD- konversion käynnistys
//Flash command/status?
void MAX765x::mov_DIRECT_RNx(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	I80C52::mov_DIRECT_RNx(instruction, directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	if(directAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
}

//Muutokset:
//Serial PORT1.
//AD- konversion käynnistys ja lippujen nollaus
//Flash command/status?
void MAX765x::mov_DIRECT_DIRECT(unsigned int const srcDirectAddress, unsigned int const dstDirectAddress)
{
	using namespace MAX765x_NameS;
	
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(srcDirectAddress==SBUF1)
		internalDataMemory_[srcDirectAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(srcDirectAddress==EESTCMD)
		internalDataMemory_[srcDirectAddress]=flashStatusOfEESTCMD_;

	I80C52::mov_DIRECT_DIRECT(srcDirectAddress, dstDirectAddress); //Tehdään hommat kantaluokassa

	//Jos luettiin ADDAT1 rekkarista
	if(srcDirectAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC); //Nollataan Converison Complete
		internalDataMemory_[ADCON] &= ~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}

	//pitää tarkistaa onko kohde direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(dstDirectAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[dstDirectAddress].readValue(), clockTicksCounted_);

	if(dstDirectAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	if(dstDirectAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[dstDirectAddress].readValue();
}

//Muutokset:
//Serial PORT1.
//AD- muunnoksen käynnistys
//Flash command/status?
void MAX765x::mov_DIRECT_RIx(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	I80C52::mov_DIRECT_RIx(instruction, directAddress); //Kantaluokassa hommailu ekaksi

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	if(directAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
}

//Muutokset:
//Serial PORT1.
//AD- muunnoksen käynnistys
//Flash command/status?
void MAX765x::mov_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace MAX765x_NameS;
	
	I80C52::mov_DIRECT_DATA(directAddress, data);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	if(directAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
}

//Muutokset
//Serial port 1
//AD- konversio
//Flash command/status?
void MAX765x::mov_RIx_DIRECT(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::mov_RIx_DIRECT(instruction, directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC); //Nollataan Converison Complete
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_OVRN); //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Dual DPTR.
void MAX765x::mov_DPTR_DATA16(unsigned char const dph, unsigned char const dpl)
{
	using namespace MAX765x_NameS;
	
	if(internalDataMemory_[DPS].readValue()&0x01) //Jos DPTR 1
	{
			internalDataMemory_[DPH1]=dph;
			internalDataMemory_[DPL1]=dpl;
	}
	else   //DPTR0
		I80C52::mov_DPTR_DATA16(dph, dpl);

}

//Muutokset:
//Serial port1
//AD- konversio
//Flash command/status?
void MAX765x::pop_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;

	I80C52::pop_DIRECT(directAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio

	//Jos Flash command rekkari tieto commandiksi.
	if(directAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//Serial port 1
//AD- konversio
//Flash command/status?
void MAX765x::push_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	
	//Jos luetaan sarjaporin1 rekisteristä haetaan tieto SBUF1
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::push_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC);   //Nollataan Converison Complete
		internalDataMemory_[ADCON]&=~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}

//Muutokset:
//Serial port 1
//AD- konversio
//Flash command/status?
void MAX765x::xch_A_DIRECT(unsigned int const directAddress)
{
	using namespace MAX765x_NameS;
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::xch_A_DIRECT(directAddress);

	//Jos luettiin ADDAT1 rekkarista
	if(directAddress==ADDAT1)
	{
		internalDataMemory_[ADCON] &= static_cast<unsigned char>(~ADCON_CC); //Nollataan Converison Complete
		internalDataMemory_[ADCON] &= ~ADCON_OVRN; //Nollataan ylikirjoitusbitti
	}
}


//Muutokset:
//Serial port 1
//Flash command/status?
void MAX765x::cjne_A_DIRECT_REL(unsigned int const directAddress, char const relAddress)
{
	using namespace MAX765x_NameS;
	
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());
		//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;

	I80C52::cjne_A_DIRECT_REL(directAddress, relAddress);

}

//Muutokset:
//Serial PORT1 tutkailu??
//AD- muunnoksen käynnistys???
//Flash command/status?
void MAX765x::djnz_DIRECT_REL(unsigned int const directAddress, char const relAddress)
{
	using namespace MAX765x_NameS;
	
	//Toimiikohen SBUFFAUS 1 tässä näin???
	//pitää tarkistaa onko direct src osoite SBUF1, jos on haetaan tieto sbuf1Receive:stä SBUF1:iin
	if(directAddress==SBUF1)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart1_->readReceiveSbuf());

	//Jos Flash status rekkari haetaan tieto statuksesta.
	if(directAddress==EESTCMD)
		internalDataMemory_[directAddress]=flashStatusOfEESTCMD_;
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa

	//Tehdään hommat kantaluokassa
	I80C52::djnz_DIRECT_REL(directAddress, relAddress);

	//pitää tarkistaa onko direct osoite SBUF1, jos on kirjotetaan tieto sbuf1Transmittiin
	//Käynnistää lähetyksen, koska kirjotus tapahtuu SBUF1 rekkariin, vai käynnistääkö???
	if(directAddress==SBUF1)
		uart1_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
	
	//Toimiikohan tämä tässä???
	if(directAddress==ADCON) //Jos kirjoitus tapahtui ADCON rekkariin
		aDConversionIsOn_=true; //Käynnistetään AD-konversio
		//Jos Flash command rekkari tieto commandiksi.???????????
	if(directAddress==EESTCMD)
		flashCommandOfEESTCMD_=internalDataMemory_[directAddress].readValue();
	//Ehkä flash toiminto ei toimi tämän käskyn kanssa. Ohjelma pitää tehdä niin ettei käytetä tätä käskyä Flashin kanssa
}

//Muutokset:
//Konejakso.
//Dual DPTR.
//dptrNumber=-1 palauttaa aktiivisen tai jos yli DPTR:n lukumäärän
unsigned int MAX765x::readDPTRValue(void)
{
	using namespace MAX765x_NameS;
	
	unsigned int dptrAddress;
	//DPTR1
	if(internalDataMemory_[DPS].readValue()&0x01)
	{
		dptrAddress=internalDataMemory_[DPH1].readValue();
		dptrAddress<<=8;
		dptrAddress|=internalDataMemory_[DPL1].readValue();
	}
	else//DPTR0
	{
		dptrAddress=internalDataMemory_[DPH].readValue();
		dptrAddress<<=8;
		dptrAddress|=internalDataMemory_[DPL].readValue();
	}
	return dptrAddress;
}

//Muutokset:
//Konejakso.
//Timer clock/4 ajastus originaalin clock/12 lisäksi.
//void MAX765x::timersAndCountersHandling(void)
//{
//}


//Ohjaa sarjaportin 0 ja 1 lähetystä eri moodeissa
//Sama ohjelma kuin 8031. Ei käytä Timer 2 ajastukseen Portti 0 käyttää: Määritelty kantaluokassa 8032
void MAX765x::serialPortRxTxHandling(void)
{
	using namespace MAX765x_NameS;
	//Ajastus kohdalleen: Eri ajastuksia kuin kantaluokissa: clk/4 lisäys
	//Koska tässä processorissa on fosc/4(=konejakso eli step) ja fosc/12(=3*konejakso) nopeudet Modelle:
	//Portti 0 ekaksi
	//if(internalDataMemory_[SCON].readValue()&SCON_SM2)
	//	uart_->setMode0ClockDivider(4);
	//else
	//	uart_->setMode0ClockDivider(12);
	//Handlataan kantaluokka ensin lävi. PORTTI0
	I80C52::serialPortRxTxHandling();

	//SERIAL PORT1 : EI timer2 käyttöä.
	//Portti 1 MODE 0 ajastus
	//if(internalDataMemory_[SCON1].readValue()&SCON1_SM2)
	//	uart1_->setMode0ClockDivider(4);
	//else
	//	uart1_->setMode0ClockDivider(12);
	//Portti1 MODE1,2 ja 3 ajastus
	if(internalDataMemory_[EICON].readValue()&EICON_SMOD1)
	{
		uart1_->setMode1Mode3TimerDivider(16);
		uart1_->setMode2ClockDivider(32);
	}
	else
	{
		uart1_->setMode1Mode3TimerDivider(32);
		uart1_->setMode2ClockDivider(64);
	}

	uart1_->stepRxTx(clockTicksCounted_, timersCounters_->timer1Overflow());
}

void MAX765x::externalCycleAddsEndOfStep(void)
{
	I80C52::externalCycleAddsEndOfStep(); //Kantaluokan hommat: correct ja uart_
	dynamic_cast <MAX765x_ExternalCycle*>(externalCycleList_)->addSerialPort1Mode0Transmit(uart1_, oneStepMachineCycles_);
}

//INT0 pinnin ohjaus perus prossun tapan P3:een
void MAX765x::timersAndCountersHandling(void)
{
	using namespace MAX765x_NameS;
	unsigned char p3Value=internalDataMemory_[P3].readValue();//P3 talteen
	if(pinList_.readValue(pinINT0_Index_, false))
		internalDataMemory_[P3]|=P3_INT0;
	else
		internalDataMemory_[P3]&=~P3_INT0;
	I80C52::timersAndCountersHandling();
	internalDataMemory_[P3]=p3Value; //Palautetaan P3 arvo
}

unsigned int MAX765x::priority0IntCheck(void)
{
	using namespace MAX765x_NameS;
	unsigned int  retVal=I80C52::priority0IntCheck();//Katsotaan kantaluokan keskeytykset
	if(!retVal)
	{
		//SerialPort1 prioriteetilla 0
		if((internalDataMemory_[SCON1].readValue()&SCON1_TI)||(internalDataMemory_[SCON1].readValue()&SCON1_RI))
			if((priority0IntON_<SERIALPORT1_INTBIT))
				if(!(internalDataMemory_[IP].readValue()&IP_PS1) && (internalDataMemory_[IE].readValue()&IE_ES1))
					return SERIALPORT1_P0_INTON;
		//FLASH prioriteetilla 0
		if(internalDataMemory_[EXIF].readValue()&EXIF_IE2)
			if((priority0IntON_<FLASH_INTBIT))
				if(!(internalDataMemory_[EIP].readValue()&EIP_PX2)&&(internalDataMemory_[EIE].readValue()&EIE_EX2))
					return FLASH_P0_INTON;
		//ADC prioriteetilla 0
		if(internalDataMemory_[EXIF].readValue()&EXIF_IE3)
			if((priority0IntON_<ADC_INTBIT))
				if(!(internalDataMemory_[EIP].readValue()&EIP_PX3)&&(internalDataMemory_[EIE].readValue()&EIE_EX3))
					return ADC_P0_INTON;
		//WDTI prioriteetilla 0
		if(internalDataMemory_[EICON].readValue()&EICON_WDIF)
			if((priority0IntON_<WDT_INTBIT))
				if(!(internalDataMemory_[EIP].readValue()&EIP_PWDI)&&(internalDataMemory_[EIE].readValue()&EIE_EWDI))
					return WDT_P0_INTON;
	}
	return retVal;
}

unsigned int MAX765x::priority1IntCheck(void)
{
	using namespace MAX765x_NameS;
	unsigned int  retVal=I80C52::priority1IntCheck();//Katsotaan kantaluokan keskeytykset
	if(!retVal)
	{
		//SerialPort1 prioriteetilla 1
		if((internalDataMemory_[SCON1].readValue()&SCON1_TI)||(internalDataMemory_[SCON1].readValue()&SCON1_RI))
			if((priority1IntON_<SERIALPORT1_INTBIT))
				if((internalDataMemory_[IP].readValue()&IP_PS1) && (internalDataMemory_[IE].readValue()&IE_ES1))
					return SERIALPORT1_P1_INTON;
		//FLASH prioriteetilla 1
		if(internalDataMemory_[EXIF].readValue()&EXIF_IE2)
			if((priority1IntON_<FLASH_INTBIT))
				if((internalDataMemory_[EIP].readValue()&EIP_PX2)&&(internalDataMemory_[EIE].readValue()&EIE_EX2))
					return FLASH_P1_INTON;
		//ADC prioriteetilla 1
		if(internalDataMemory_[EXIF].readValue()&EXIF_IE3)
			if((priority1IntON_<ADC_INTBIT))
				if((internalDataMemory_[EIP].readValue()&EIP_PX3)&&(internalDataMemory_[EIE].readValue()&EIE_EX3))
					return ADC_P1_INTON;
		//WDTI prioriteetilla 1
		if(internalDataMemory_[EICON].readValue()&EICON_WDIF)
			if((priority1IntON_<WDT_INTBIT))
				if((internalDataMemory_[EIP].readValue()&EIP_PWDI)&&(internalDataMemory_[EIE].readValue()&EIE_EWDI))
					return WDT_P1_INTON;
	}
	return retVal;
}

bool MAX765x::putInterruptServiceOn(unsigned int const interruptToPutOn)
{
	using namespace MAX765x_NameS;
	switch(interruptToPutOn)
	{
		case SERIALPORT1_P0_INTON:
			priority0IntON_|=SERIALPORT1_INTBIT;
			programCounter_=SERIALPORT1_INTERRUPT_VECTOR;
			return true;
		case FLASH_P0_INTON:
			priority0IntON_|=FLASH_INTBIT;
			programCounter_=FLASH_INTERRUPT_VECTOR;
			return true;
		case ADC_P0_INTON:
			priority0IntON_|=ADC_INTBIT;
			programCounter_=ADC_INTERRUPT_VECTOR;
			return true;
		case WDT_P0_INTON:
			priority0IntON_|=WDT_INTBIT;
			programCounter_=WDT_INTERRUPT_VECTOR;
			return true;
		case SERIALPORT1_P1_INTON:
			priority1IntON_|=SERIALPORT1_INTBIT;
			programCounter_=SERIALPORT1_INTERRUPT_VECTOR;
			return true;
		case FLASH_P1_INTON:
			priority1IntON_|=FLASH_INTBIT;
			programCounter_=FLASH_INTERRUPT_VECTOR;
			return true;
		case ADC_P1_INTON:
			priority1IntON_|=ADC_INTBIT;
			programCounter_=ADC_INTERRUPT_VECTOR;
			return true;
		case WDT_P1_INTON:
			priority1IntON_|=WDT_INTBIT;
			programCounter_=WDT_INTERRUPT_VECTOR;
			return true;
	}
	return I80C52::putInterruptServiceOn(interruptToPutOn);
}

bool MAX765x::priority0IntReturn(void)
{
	using namespace MAX765x_NameS;
	bool retVal=I80C52::priority0IntReturn();
	if((!retVal)&&(priority0IntON_))
	{
		if(priority0IntON_&SERIALPORT1_INTBIT)
		{
			priority0IntON_&=~SERIALPORT1_INTBIT;
			return true;
		}
		if(priority0IntON_&FLASH_INTBIT)
		{
			priority0IntON_&=~FLASH_INTBIT;
			return true;
		}
		if(priority0IntON_&ADC_INTBIT)
		{
			priority0IntON_&=~ADC_INTBIT;
			return true;
		}
		if(priority0IntON_&WDT_INTBIT)
		{
			priority0IntON_&=~WDT_INTBIT;
			return true;
		}
	}
	return retVal;
}

bool MAX765x::priority1IntReturn(void)
{
	using namespace MAX765x_NameS;
	bool retVal=I80C52::priority1IntReturn();
	if((!retVal)&&(priority1IntON_))
	{
		if(priority1IntON_&SERIALPORT1_INTBIT)
		{
			priority1IntON_&=~SERIALPORT1_INTBIT;
			return true;
		}
		if(priority1IntON_&FLASH_INTBIT)
		{
			priority1IntON_&=~FLASH_INTBIT;
			return true;
		}
		if(priority1IntON_&ADC_INTBIT)
		{
			priority1IntON_&=~ADC_INTBIT;
			return true;
		}
		if(priority1IntON_&WDT_INTBIT)
		{
			priority1IntON_&=~WDT_INTBIT;
			return true;
		}
	}
	return retVal;
}

//Uudelleenmääritys koska pinnit eri kuin kantaprocessorissa
bool MAX765x::readExternalIntPin(char const exInterrupt)
{
	bool pinValue=true;
	switch(exInterrupt)
	{
		case 0:
			pinValue=pinList_.readValue(pinINT0_Index_, true);
			break;
		case 1:
			pinValue=pinList_.readValue(pinINT1_Index_, true);
			break;
	}
	return pinValue;
}

void MAX765x::handlingWatchdog(void)
{
	using namespace MAX765x_NameS;
	//Jos vahtikoira enbloitu
	if(internalDataMemory_[EICON].readValue()&EICON_EWT)
	{
		watchdogCounter_+=(clockTicksCounted_-lastPeriodClockTicksInHandlingPeripherals_);
		//Lasketaan periodiaika.
		double periodTime=pow(2.0,(internalDataMemory_[CKCON].readValue()>>6)*3.0+16.0);
		if((watchdogCounter_>periodTime)&&!watchdogPeriodReached_)
		{
			counterValueAtPeriodReached_=watchdogCounter_;
			internalDataMemory_[EICON]|=(unsigned char)EICON_WDIF;   //Asetetaan keskytyslippu.
			watchdogPeriodReached_=true;
		}
		if(watchdogPeriodReached_)
		{
			if((watchdogCounter_-counterValueAtPeriodReached_)>=512)
			{
				internalDataMemory_[WDT]|=(unsigned char)WDT_WTRF;
				reset_protected();
			}
			if(internalDataMemory_[EICON].readValue()&EICON_RWT)
			{
				watchdogCounter_=0;
				watchdogPeriodReached_=false;
				internalDataMemory_[EICON]&=~EICON_WDIF;   //Nollataan keskytyslippu
				internalDataMemory_[EICON]&=~EICON_RWT;   //Nollataan resettilippu
			}
		}
	}

	if(!(internalDataMemory_[WDT].readValue()&WDT_WTRF)&&prieviousWTRF_)
		watchdogCounter_=0;

	prieviousWTRF_=bool(internalDataMemory_[WDT].readValue()&WDT_WTRF);
}

void MAX765x::handlingADC(void)
{
	using namespace MAX765x_NameS;
	//Jos AD muunnos käynnityi
	//Luetaaan mux käynnistykesn yhteydessä
	if(!(aDConversionIsOnPrevValue_)&&(aDConversionIsOn_))
		inputSelect_=internalDataMemory_[ADCON].readValue()&0x0F;
	//Write ADCON rekkariin käynnistää muunnoksen: valmis 224 kellopulssin jälkeen.225 nouseva reuna
	if(aDConversionIsOn_)
	{
		conversionCounter_+=(clockTicksCounted_-lastPeriodClockTicksInHandlingPeripherals_);
		if(conversionCounter_>=225)
		{
			//AD-muunnos
			//Referenssijännite molemmissa modessa sama = VREF+ - VREF-
			double vRef=analogInputPins_.readValue(aPinVREFP_Index_)-analogInputPins_.readValue(aPinVREFN_Index_);
			double vIn,fsP,fsN,aCom,lsb;
			if(!(inputSelect_&ADCON_M3))//SingleEnded mode
			{
				//Referenssit
				fsP=vRef/2;
				fsN=(-vRef)/2;
				aCom=analogInputPins_.readValue(aPinACOM_Index_);
				lsb=vRef/4096;
				switch(inputSelect_&0x07) //Kolme alinta bittiä valitsee inputin
				{
					case 0:
						vIn=analogInputPins_.readValue(aPinAIN0_Index_)-aCom;
						break;
					case 1:
						vIn=analogInputPins_.readValue(aPinAIN1_Index_)-aCom;
						break;
					case 2:
						vIn=analogInputPins_.readValue(aPinAIN2_Index_)-aCom;
						break;
					case 3:
						vIn=analogInputPins_.readValue(aPinAIN3_Index_)-aCom;
						break;
					case 4:
						vIn=analogInputPins_.readValue(aPinAIN4_Index_)-aCom;
						break;
					case 5:
						vIn=analogInputPins_.readValue(aPinAIN5_Index_)-aCom;
						break;
					case 6:
						vIn=analogInputPins_.readValue(aPinAIN6_Index_)-aCom;
						break;
					case 7:
						vIn=analogInputPins_.readValue(aPinAIN7_Index_)-aCom;
						break;
				}
			}
			else //Differential mode
			{
				fsP=vRef;
				fsN=-vRef;
				lsb=2*vRef/4096;
				switch(inputSelect_&0x07) //Kolme alinta bittiä valitsee inputparin
				{
					case 0:
						vIn=analogInputPins_.readValue(aPinAIN1_Index_)-analogInputPins_.readValue(aPinAIN0_Index_);
						break;
					case 1:
						vIn=analogInputPins_.readValue(aPinAIN3_Index_)-analogInputPins_.readValue(aPinAIN2_Index_);
						break;
					case 2:
						vIn=analogInputPins_.readValue(aPinAIN5_Index_)-analogInputPins_.readValue(aPinAIN4_Index_);
						break;
					case 3:
						vIn=analogInputPins_.readValue(aPinAIN7_Index_)-analogInputPins_.readValue(aPinAIN6_Index_);
						break;
					case 4:
						vIn=analogInputPins_.readValue(aPinVREFP_Index_)-analogInputPins_.readValue(aPinVREFN_Index_);
						break;
				}
			}
			//Konversio
			int cL;
			if(vIn>0.0)
			{
				//Yli alueen tappiinsa
				if(vIn>fsP)
					vIn=fsP;
				cL=int(vIn/lsb-lsb);
			}
			else
			{
				//Alle alueen tappiinsa
				if(vIn<fsN)
					vIn=fsN;
				cL=int(vIn/lsb);
			}
			//Muunnostiedot rekkareihinsa
			internalDataMemory_[ADDAT1]=(unsigned char)(cL>>4);
			internalDataMemory_[ADDAT0]=(unsigned char)(cL<<4);
			//Jos edellinen konversio lukematta
			if(internalDataMemory_[ADCON].readValue()&ADCON_CC)
				internalDataMemory_[ADCON]|=(unsigned char)ADCON_OVRN; //OVRN bitti päälle
			internalDataMemory_[ADCON]|=(unsigned char)ADCON_CC;//Conversion Complete päälle
			//Jos interrupt enable ADCON rekkarissa
			if(internalDataMemory_[ADCON].readValue()&ADCON_CCIE)
				internalDataMemory_[EXIF]|=(unsigned char)EXIF_IE3;//Keskeytyslippu päälle
			//jos jatkuva konversio käynnissä
			if(internalDataMemory_[ADCON].readValue()&ADCON_CCVT)
			{
				conversionCounter_-=224;
				inputSelect_=internalDataMemory_[ADCON].readValue()&0x0F;
			}
			else
			{
				conversionCounter_=0;
				aDConversionIsOn_=false;
			}
		}
	}
	aDConversionIsOnPrevValue_=aDConversionIsOn_;
}

void MAX765x::handlingPWM(void)
{
	using namespace MAX765x_NameS;
	//Jos modulo 255 counter enabloitu
	if(internalDataMemory_[PWMC].readValue()&PWMC_PWON)
	{
		pwmCounter_+=(clockTicksCounted_-lastPeriodClockTicksInHandlingPeripherals_);
		//Jos counterin arvo ylittyy 2xprescalerin+1 arvon
		if(pwmCounter_>=static_cast<unsigned int>(2*(internalDataMemory_[PWPS].readValue()+1)))
		{
			int adder=pwmCounter_/(2*(internalDataMemory_[PWPS].readValue()+1));
			pwmCounter_-=adder*(2*(internalDataMemory_[PWPS].readValue()+1)); //Korjataan arvoa
			modulo255Counter_+=adder;   //Lisätään modulocounteria.
			if(modulo255Counter_>255)
			{
				modulo255Counter_-=255;
				pinList_.setPin(PINNAME_PWMA, false, false);
				pinList_.setPin(PINNAME_PWMB, false, false);
			}
			//Katsotaan outputit
			//A
			if(internalDataMemory_[PWMC].readValue()&PWMC_PWENA)
			{
				if(modulo255Counter_>=internalDataMemory_[PWDA].readValue())
					pinList_.setPin(PINNAME_PWMA, true, false);
				else
					pinList_.setPin(PINNAME_PWMA, false, false);
			}
			else
				pinList_.setPin(PINNAME_PWMA, false, false);
			//B
			if(internalDataMemory_[PWMC].readValue()&PWMC_PWENB)
			{
				if(modulo255Counter_>=internalDataMemory_[PWDB].readValue())
					pinList_.setPin(PINNAME_PWMB, true, false);
				else
					pinList_.setPin(PINNAME_PWMB, false, false);
			}
			else
				pinList_.setPin(PINNAME_PWMB, false, false);
		}
	}
	else
	{
		pwmCounter_=0;
		modulo255Counter_=0;
		pinList_.setPin(PINNAME_PWMA, false, false);
		pinList_.setPin(PINNAME_PWMB, false, false);
	}
}

void MAX765x::handlingFlashOperations(void)
{
	using namespace MAX765x_NameS;
	if(flashCommandOfEESTCMD_!=0) //Jos commandi 0 turha käydä kurkkimassa.
	{
		unsigned int flashAddress=(internalDataMemory_[EEAH].readValue()<<6)|(internalDataMemory_[EEAL].readValue()&0x3F);
		//Tarkistetaan, että ollaan oikealla alueella.
		if(flashAddress<0x3FC0)
		{
			//Read
			if(flashCommandOfEESTCMD_==0xAA)
			{
				internalDataMemory_[EEDAT]=readInternalProgramMemoryValue(flashAddress);
				flashCommandOfEESTCMD_=0; //Nollataan, jotta voidaan tunnistaa seuraava commandi
				flashStatusOfEESTCMD_=0xC0; //Voidaan hyväksyä seuraavat commandit koska tieto heti saatavilla
				//Ei tee keskeytystä.
			}
			//Write/Erase
			if((flashCommandOfEESTCMD_==0x55)||(flashCommandOfEESTCMD_==0xA5))
			{
				if(flashStatusOfEESTCMD_==0xC0) //Alkutilanne
				{
					if(internalDataMemory_[EEAH].readValue()>127) //Jos ylempi Flash blokki
						flashStatusOfEESTCMD_=0x40; //Nollataan ylempi bitti
					else
						flashStatusOfEESTCMD_=0x80; //Alempi blokki nollataan bitti 6
					flashTimeCounter_=0;   //Nollataan aikalaskuri
				}
				else
				{
					flashTimeCounter_+=(clockTicksCounted_-lastPeriodClockTicksInHandlingPeripherals_);
					double clockTickTime=1.0/clockSpeed_;
					double timeAtStart=static_cast<double>(clockTickTime*flashTimeCounter_);
					//Write
					if(flashCommandOfEESTCMD_==0x55)
					{
						//Write time = 63 mikrosekuntia
						if(timeAtStart>=0.000063)
						{
							internalProgramMem->setValue(flashAddress, internalDataMemory_[EEDAT].readValue());
							flashCommandOfEESTCMD_=0; //Nollataan, jotta voidaan tunnistaa seuraava commandi
							flashStatusOfEESTCMD_=0xC0; //Hyväksytään commandit
							if(internalDataMemory_[EIE].readValue()&EIE_EX2)
								internalDataMemory_[EXIF] |= static_cast<unsigned char>(EXIF_IE2);//Keskeytyslippu päälle
						}
					}
					//Erase
					else
					{
						//Block Erase Time = 9.4 millisekuntia
						if(timeAtStart>=0.0094)
						{
							flashAddress&=0xFFC0; //Ei oteta huomioon blokin alaosaosoitetta, vaan blokin page
							for(unsigned int n=flashAddress;n<flashAddress+64;n++)
							{
								if(n<0x3FC0)
									internalProgramMem->setValue(n, 0xFF);
							}
							if(internalDataMemory_[EIE].readValue()&EIE_EX2)
								internalDataMemory_[EXIF]|=(unsigned char)EXIF_IE2;//Keskeytyslippu päälle
							flashCommandOfEESTCMD_=0; //Nollataan, jotta voidaan tunnistaa seuraava commandi
							flashStatusOfEESTCMD_=0xC0; //Hyväksytään commandit
						}
					}
				}
			}
		}
		else
		{
			internalDataMemory_[EEDAT]=0;
			flashCommandOfEESTCMD_=0; //Nollataan, jotta voidaan tunnistaa seuraava commandi
			flashStatusOfEESTCMD_=0xC0; //Hyväksytään commandit
		}
	}
}

void MAX765x::handlePeripherals_VirtualPart(void)
{
	handlingADC();
	handlingPWM();
	handlingWatchdog();
	handlingFlashOperations();

		//Kaikki handlaajat voi käyttää tätä muuttujaa edellisen kierroksen kellopulssien määrrää
	lastPeriodClockTicksInHandlingPeripherals_=clockTicksCounted_;
}

void MAX765x::createSFRBitNameTable(void)
{
	using namespace MAX765x_NameS;
	//DPS:n bitti
	setSFRBitNameInUse(DPS, BIT_NUMBER_0, "SEL");
	//CKCON bitit
	setSFRBitNameInUse(CKCON, BIT_NUMBER_0, "MD0");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_1, "MD1");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_2, "MD2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_3, "T0M");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_4, "T1M");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_5, "T2M");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_6, "WD0");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_7, "WD1");
	//EXIF bitit
	setSFRBitNameInUse(EXIF, BIT_NUMBER_4, "IE2");
	setSFRBitNameInUse(EXIF, BIT_NUMBER_5, "IE3");
	//SCON1 bitit
	setSFRBitNameInUse(SCON1, BIT_NUMBER_0, "RI");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_1, "TI");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_2, "RB8");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_3, "TB8");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_4, "REN");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_5, "SM2");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_6, "SM1");
	setSFRBitNameInUse(SCON1, BIT_NUMBER_7, "SM0");
	//ADCON bitit
	setSFRBitNameInUse(ADCON, BIT_NUMBER_0, "M0");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_1, "M1");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_2, "M2");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_3, "M3");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_4, "OVRN");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_5, "CCIE");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_6, "CCVT");
	setSFRBitNameInUse(ADCON, BIT_NUMBER_7, "CC");
	//EICON bitit  Ei ole kuitenkaan bittiosoitettava
	setSFRBitNameInUse(EICON, BIT_NUMBER_0, "RWT");
	setSFRBitNameInUse(EICON, BIT_NUMBER_1, "EWT");
	setSFRBitNameInUse(EICON, BIT_NUMBER_3, "WDTI");
	setSFRBitNameInUse(EICON, BIT_NUMBER_7, "SMOD1");
	//WDT bitit
	setSFRBitNameInUse(WDT, BIT_NUMBER_2, "WTRF");
	//EIE bitit
	setSFRBitNameInUse(EIE, BIT_NUMBER_0, "EX2");
	setSFRBitNameInUse(EIE, BIT_NUMBER_1, "EX3");
	setSFRBitNameInUse(EIE, BIT_NUMBER_4, "EWDI");
	//EIP bitit
	setSFRBitNameInUse(EIP, BIT_NUMBER_0, "PX2");
	setSFRBitNameInUse(EIP, BIT_NUMBER_1, "PX3");
	setSFRBitNameInUse(EIP, BIT_NUMBER_4, "PWDI");
	//PWMC bitit
	setSFRBitNameInUse(PWMC, BIT_NUMBER_0, "PWENB");
	setSFRBitNameInUse(PWMC, BIT_NUMBER_1, "PWENA");
	setSFRBitNameInUse(PWMC, BIT_NUMBER_7, "PWON");

	//IP:n biti
	setSFRBitNameInUse(IP, BIT_NUMBER_6, "PSI");
	//IE:n bitit
	setSFRBitNameInUse(IE, BIT_NUMBER_6, "ESI");

	//P1:n bitit
	setSFRBitNameInUse(P1, BIT_NUMBER_2, "RXD1");
	setSFRBitNameInUse(P1, BIT_NUMBER_3, "TXD1");

	//P3:n INT pinnit ovat tässä proskussa omat, joten poistetaan ne portin nimilistatsta.
	setSFRBitNameInUse(P3, BIT_NUMBER_2, "P3.2");
	setSFRBitNameInUse(P3, BIT_NUMBER_3, "P3.3");
}

void MAX765x::createSFRNameTable(void)
{
	using namespace MAX765x_NameS;
	setSFRInUse(DPL1, "DPL1", false);
	setSFRInUse(DPH1, "DPH1", false);
	setSFRInUse(DPS, "DPS", false);
	setSFRInUse(CKCON, "CKCON", false);
	setSFRInUse(EXIF, "EXIF", false);
	setSFRInUse(VER_ID, "VERSION", false);
	setSFRInUse(SCON1, "SCON1", false);
	setSFRInUse(SBUF1, "SBUF1", false);
	setSFRInUse(ADDAT0, "ADDAT0", false);
	setSFRInUse(ADDAT1, "ADDAT1", false);
	setSFRInUse(ADCON, "ADCON", false);
	setSFRInUse(EICON, "EICON", false);
	internalDataMemory_[EICON].setIsBitAddressable(false); //Ei ole bittiosoitettava vaikka on 8-loppuinen
	setSFRInUse(PWPS, "PWPS", false);
	setSFRInUse(PWDA, "PWDA", false);
	setSFRInUse(PWDB, "PWDB", false);
	setSFRInUse(WDT, "WDT", false);
	setSFRInUse(EIE, "EIE", false);
	setSFRInUse(EEAL, "EEAL", false);
	setSFRInUse(EEAH, "EEAH", false);
	setSFRInUse(EEDAT, "EEDAT", false);
	setSFRInUse(EESTCMD, "EESTCMD", false);
	setSFRInUse(EIP, "EIP", false);
	setSFRInUse(PWMC, "PWMC", false);
}

void MAX765x::changeSomeInstructionsMachineCycles(void)
{
	instructions_->setNewMachineCycles(STR_add_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_add_A_DATA, 2);
	instructions_->setNewMachineCycles(STR_addc_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_addc_A_DATA, 2);
	instructions_->setNewMachineCycles(STR_subb_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_subb_A_DATA, 2);
	instructions_->setNewMachineCycles(STR_inc_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_dec_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_inc_DPTR, 3);
	instructions_->setNewMachineCycles(STR_div_AB, 5);
	instructions_->setNewMachineCycles(STR_mul_AB, 5);
	instructions_->setNewMachineCycles(STR_anl_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_anl_A_DATA, 2);
	instructions_->setNewMachineCycles(STR_anl_DIRECT_A, 2);
	instructions_->setNewMachineCycles(STR_anl_DIRECT_DATA, 3);
	instructions_->setNewMachineCycles(STR_orl_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_orl_A_DATA, 2);
	instructions_->setNewMachineCycles(STR_orl_DIRECT_A, 2);
	instructions_->setNewMachineCycles(STR_orl_DIRECT_DATA, 3);
	instructions_->setNewMachineCycles(STR_xrl_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_xrl_A_DATA ,2);
	instructions_->setNewMachineCycles(STR_xrl_DIRECT_A, 2);
	instructions_->setNewMachineCycles(STR_xrl_DIRECT_DATA, 3);
	instructions_->setNewMachineCycles(STR_mov_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_mov_A_DATA, 2);
	instructions_->setNewMachineCycles(STR_mov_RNx_DATA, 2);
	instructions_->setNewMachineCycles(STR_mov_DIRECT_A, 2);
	instructions_->setNewMachineCycles(STR_mov_DIRECT_DIRECT, 3);
	instructions_->setNewMachineCycles(STR_mov_DIRECT_DATA, 3);
	instructions_->setNewMachineCycles(STR_mov_RIx_DATA, 2);
	instructions_->setNewMachineCycles(STR_mov_DPTR_DATA16, 3);
	instructions_->setNewMachineCycles(STR_movc_A_A_DPTR, 3);
	instructions_->setNewMachineCycles(STR_movc_A_A_PC, 3);
	instructions_->setNewMachineCycles(STR_movx_A_RIx, 2); //Tässä myös Stretch toiminto.
	instructions_->setNewMachineCycles(STR_movx_RIx_A, 2); //Tässä myös Stretch toiminto.
	instructions_->setNewMachineCycles(STR_movx_A_DPTR, 2); //Tässä myös Stretch toiminto.
	instructions_->setNewMachineCycles(STR_movx_DPTR_A, 2); //Tässä myös Stretch toiminto.
	instructions_->setNewMachineCycles(STR_xch_A_DIRECT, 2);
	instructions_->setNewMachineCycles(STR_clr_BIT, 2);
	instructions_->setNewMachineCycles(STR_setb_BIT, 2);
	instructions_->setNewMachineCycles(STR_cpl_BIT, 2);
	instructions_->setNewMachineCycles(STR_acall_x00, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_lcall, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_ret, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_reti, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_ajmp_x00, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_ljmp, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_sjmp, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jc_REL, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jnc_REL, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jb_BIT_REL, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jnb_BIT_REL, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jbc_BIT_REL, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jmp_A_DPTR, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jz_REL, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_jnz_REL, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_cjne_A_DIRECT_REL, 4);
	instructions_->setNewMachineCycles(STR_cjne_A_DATA_REL, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_cjne_RNx_DATA_REL, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_cjne_RIx_DATA_REL, 4);   //MAXISSA 4 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_djnz_RNx_REL, 3);   //MAXISSA 3 konejaksoa. 8031:ssä 2.
	instructions_->setNewMachineCycles(STR_djnz_DIRECT_REL, 4);
}

void MAX765x::saveWorkDataToFile(QDataStream& saveFileStream)
{
	I80C52::saveWorkDataToFile(saveFileStream); //Talletetaan kantaluokan jutut

	analogInputPins_.saveWorkDataToFile(saveFileStream);

	//Tallennetaan myös program memory koska flashia, voitu muuttaa ohjelam ajolla.
	internalProgramMem->saveWorkDataToFile(saveFileStream);

	uart1_->saveWorkDataToFile(saveFileStream);

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)lastPeriodClockTicksInHandlingPeripherals_;
		//WATCHDOG
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)watchdogCounter_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)counterValueAtPeriodReached_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)watchdogPeriodReached_;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)prieviousWTRF_;
		//AD
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)aDConversionIsOn_;
	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)aDConversionIsOnPrevValue_;
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)inputSelect_;
	saveFileStream<<(Q_UINT16)8<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)conversionCounter_;
		//PWM
	saveFileStream<<(Q_UINT16)9<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)pwmCounter_;
	saveFileStream<<(Q_UINT16)10<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)modulo255Counter_;
		//Flash read/write/erase
	saveFileStream<<(Q_UINT16)11<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)flashCommandOfEESTCMD_;
	saveFileStream<<(Q_UINT16)12<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)flashStatusOfEESTCMD_;
	saveFileStream<<(Q_UINT16)13<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)flashTimeCounter_;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void MAX765x::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	I80C52::readWorkDataFromFile(readFileStream,funcHandleUnknownIndexRead);//Luetaan kantaluokan tallennukset

	analogInputPins_.readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	internalProgramMem->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	uart1_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8Bool;
	Q_UINT8 value8Data;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value32;
				lastPeriodClockTicksInHandlingPeripherals_=(unsigned long)value32;
				break;
			//WATCHDOG
			case 1:
				readFileStream>>typeReadData>>value32;
				watchdogCounter_=(unsigned long)value32;
				break;
			case 2:
				readFileStream>>typeReadData>>value32;
				counterValueAtPeriodReached_=(unsigned long)value32;
				break;
			case 3:
				readFileStream>>typeReadData>>value8Bool;
				watchdogPeriodReached_=(bool)value8Bool;
				break;
			case 4:
				readFileStream>>typeReadData>>value8Bool;
				prieviousWTRF_=(bool)value8Bool;
				break;
			//AD
			case 5:
				readFileStream>>typeReadData>>value8Bool;
				aDConversionIsOn_=(bool)value8Bool;
				break;
			case 6:
				readFileStream>>typeReadData>>value8Bool;
				aDConversionIsOnPrevValue_=(bool)value8Bool;
				break;
			case 7:
				readFileStream>>typeReadData>>value8Bool;
				inputSelect_=(char)value8Bool;
				break;
			case 8:
				readFileStream>>typeReadData>>value32;
				conversionCounter_=(int)value32;
				break;
			//PWM
			case 9:
				readFileStream>>typeReadData>>value32;
				pwmCounter_=(unsigned int)value32;
				break;
			case 10:
				readFileStream>>typeReadData>>value32;
				modulo255Counter_=(unsigned int)value32;
				break;
			//Flash read/write/erase
			case 11:
				readFileStream>>typeReadData>>value8Data;
				flashCommandOfEESTCMD_=(unsigned char)value8Data;
				break;
			case 12:
				readFileStream>>typeReadData>>value8Data;
				flashStatusOfEESTCMD_=(unsigned char)value8Data;
				break;
			case 13:
				readFileStream>>typeReadData>>value32;
				flashTimeCounter_=(int)value32;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}

