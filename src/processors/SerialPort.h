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
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "Global_NameS.h"
#include "InternalRAM.h"

using namespace Global_NameS;
class SerialPort
{
	public:
		struct RegSCONBits
		{
			unsigned char bitRI;
			unsigned char bitTI;
			unsigned char bitRB8;
			unsigned char bitTB8;
			unsigned char bitREN;
			unsigned char bitSM2;
			unsigned char bitSM1;
			unsigned char bitSM0;
		};

		struct RxTxPins
		{
			unsigned char pinRx;
			unsigned char pinTx;
		};

		SerialPort(InternalRAM* const pToSCON, RegSCONBits const bitsSCON, InternalRAM* const pToSBUF, InternalRAM* const pToIOPort, RxTxPins const ioPort);
		virtual ~SerialPort();
		SerialPort(SerialPort const& copyFrom);
		SerialPort& operator =(SerialPort const& assignFrom);

		void reset(void);

		int readMode(void);

		void charToReceiveBuff(int schar);//Int tyyppin parametri, jos mode jossa bittejä>8
		//Parametri on seuraava eri moodeille:
		//Start bittiä ei oteta huomioon missään moodissa
		//Mode 0: 8 databittiä pelkästään eli intin alaosa kokonaan:
		//Mode 1: b0-b7=8dataa, b8=stop=1
		//Mode 2 ja 3: b0-b7=8dataa, b8=MP ,b9=stop=1

		//inline void setMode0ClockDivider(unsigned int const serialPortM0ClockDivider)
		//{
		//	serialPortM0ClockDivider_=serialPortM0ClockDivider;
		//};
		inline void setMode1Mode3TimerDivider(unsigned int const serialPortM1M3TimerDivider)
		{
			serialPortM1M3TimerDivider_=serialPortM1M3TimerDivider;
		};
		void setMode2ClockDivider(unsigned int const serialPortM2ClockDivider)
		{
			serialPortM2ClockDivider_=serialPortM2ClockDivider;
		};

		//Vastaanottopuskurin luku
		inline int readReceiveSbuf(void){return sbufReceive_;};
		int readInputShiftReg(int &bitToReceive);//When bitToReceive==0, StartBit
		
		//Erilliset sarjaportin lähetys ja vaastaanottorekisterit:
		//8031/51:ssä toiminta tapahtuu SBUF rekkarin kautta
		//Int tyyppiä koska vastaanotetut bitit > 8.
		void startTransmit(unsigned int const ch, unsigned long const clockTicks);
		//Serjaportista lähetys käynnissä
		inline bool isTaransmitOn(void){return serialPortTransmitIsOn_;};
		bool isTaransmitOn(unsigned char mode);
		inline int readTransmitSbuf(void){return sbufTransmit_;};//Lähetyksenpuskurin luku.
		int readTransmitSbuf(int &bitToTransmit);//When bitToTransmit==0, StartBit  
		//Lähetetty data voidaan lukea tällä kun serialTransmit laskeva reuna.
		int readTransmittedValue(void);

		//External cycleä varten.
		int readTransmitSbufMode0(int &bitToTransmit);//Lähetyksenpuskurin luku.Start bit =0.
		void stopTransmitMode0(void); //mode0 stoppaus
		unsigned char readSCON(void){return pToSCON_->readValue();}; //Luetaan SCON rekkarin tila.
		
		virtual void stepRxTx(unsigned long clockTicks, bool const timerOverflow);


		//Tietojen tallennus. Ei ANSI mukainen vaan QT
		//Pitää määritellä perivissä, jos tallenetaan niiden ominaisuuksien tiloja, mutta kutsuttava myös tätä.
		virtual void saveWorkDataToFile(QDataStream& saveFileStream);
		virtual void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);

	private:
		SerialPort(){};

		InternalRAM* pToSBUF_;
		InternalRAM* pToSCON_;
		RegSCONBits bitsSCON_;
		InternalRAM* pToIOPort_; //Rx ja Tx pinnien portti
		RxTxPins ioPort_;

		static unsigned int const MODE0_BITSTOSENDRECEIVE=8;
		//Vastaanotto
		int sbufReceive_;
		int inputShiftReg_;
		bool receiveEnabled(void); //Tutkii ren bittiä ja
		unsigned char lastRENValue_; //Käytetään REN bitin muutoksen tutkimiseen.Receive enable:
		unsigned long serialPortReceiveStartClockCounter_; //Käytetään sarjaportin vastaanotonlaskemiseen
		int serialPortReceiveBitCounter_;      //Lasketaan bitit
		int serialPortReceiveTimerOverflowCounter_; //Lasketaan timerien ylivuodot
		bool serialPortReceiveIsOn_; //Sarjaportin vastaanotto käynnissä
		bool receive1To0Detect(void); //Detektoi RxD pinnin muutoksen 1>0, jos receive ei päällä.
		unsigned char lastRxDPinValue_;
		void receiveMode2Mode3(void);
		virtual void handlingReceive(int mode, unsigned long clockTicks, bool const timerOverflow);
		
		//Lähetys
		int sbufTransmit_;
		int lastSbufTransmit_; //Viimeisin lähetys: mukana myös 9bitti
		unsigned long serialPortTransmitStartClockCounter_; //Käytetään sarjaportin lähetyksen laskemiseen
		unsigned int serialPortTransmitBufferReadCounter_; //Käytetään modessa 0 kun lutaaan transmit bufferia
		unsigned int serialPortTransmitBitCounter_;      //Lasketaan bitit
		unsigned int serialPortTransmitTimerOverflowCounter_; //Lasketaan timerien ylivuodot
		bool serialPortTransmitIsOn_; //Sarjaportin lähetys käynnissä
		bool serialPortTransmitMode0IsOn_; //Mode 0 lähetys käynnissä
		virtual void handlingTransmit(int mode, unsigned long clockTicks, bool const timerOverflow);
		void sendMode2Mode3(void);
		void transmitStop(void);

		//unsigned int serialPortM0ClockDivider_; //Moodin 0 nopeuden kellopulssijakaja.
		unsigned int serialPortM1M3TimerDivider_; //Timer jakaja MODESSA 1 ja 3
		unsigned int serialPortM2ClockDivider_; //Kellojakaja MODESSA 2

		void copy(SerialPort const& copyFrom);

};

#endif
