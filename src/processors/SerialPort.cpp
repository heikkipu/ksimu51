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
#include "SerialPort.h"
#include "../ksimu51.h"


SerialPort::SerialPort(InternalRAM* const pToSCON, RegSCONBits const bitsSCON, InternalRAM* const pToSBUF, InternalRAM* const pToIOPort, RxTxPins const ioPort)
{
	pToSCON_=pToSCON;
	bitsSCON_.bitRI=bitsSCON.bitRI;
	bitsSCON_.bitTI=bitsSCON.bitTI;
	bitsSCON_.bitRB8=bitsSCON.bitRB8;
	bitsSCON_.bitTB8=bitsSCON.bitTB8;
	bitsSCON_.bitREN=bitsSCON.bitREN;
	bitsSCON_.bitSM2=bitsSCON.bitSM2;
	bitsSCON_.bitSM1=bitsSCON.bitSM1;
	bitsSCON_.bitSM0=bitsSCON.bitSM0;

	pToSBUF_=pToSBUF;

	pToIOPort_=pToIOPort;
	ioPort_.pinRx=ioPort.pinRx;
	ioPort_.pinTx=ioPort.pinTx;

	serialPortM1M3TimerDivider_=32;
	serialPortM2ClockDivider_=64;
	serialPortTransmitIsOn_=false;
	serialPortReceiveIsOn_=false;
	serialPortTransmitMode0IsOn_=false;
	inputShiftReg_=0;
	sbufTransmit_=0;
}


SerialPort::~SerialPort()
{
}

SerialPort::SerialPort(SerialPort const& copyFrom)
{
	copy(copyFrom);
}

SerialPort& SerialPort ::operator=(SerialPort const& assignFrom)
{
	copy(assignFrom);
	return *this;
}

void SerialPort::copy(SerialPort const& copyFrom)
{
	pToSCON_=copyFrom.pToSCON_;
	bitsSCON_.bitRI=copyFrom.bitsSCON_.bitRI;
	bitsSCON_.bitTI=copyFrom.bitsSCON_.bitTI;
	bitsSCON_.bitRB8=copyFrom.bitsSCON_.bitRB8;
	bitsSCON_.bitTB8=copyFrom.bitsSCON_.bitTB8;
	bitsSCON_.bitREN=copyFrom.bitsSCON_.bitREN;
	bitsSCON_.bitSM2=copyFrom.bitsSCON_.bitSM2;
	bitsSCON_.bitSM1=copyFrom.bitsSCON_.bitSM1;
	bitsSCON_.bitSM0=copyFrom.bitsSCON_.bitSM0;

	pToSBUF_=copyFrom.pToSBUF_;

	pToIOPort_=copyFrom.pToIOPort_;
	ioPort_.pinRx=copyFrom.ioPort_.pinRx;
	ioPort_.pinTx=copyFrom.ioPort_.pinTx;

	//serialPortM0ClockDivider_=copyFrom.serialPortM0ClockDivider_;
	serialPortM1M3TimerDivider_=copyFrom.serialPortM1M3TimerDivider_;
	serialPortM2ClockDivider_=copyFrom.serialPortM2ClockDivider_;
	serialPortTransmitIsOn_=copyFrom.serialPortTransmitIsOn_;
}

void SerialPort::reset(void)
{
	serialPortTransmitIsOn_=false;
	serialPortTransmitMode0IsOn_=false;
	serialPortTransmitBitCounter_=0;
	sbufTransmit_=0;
	serialPortReceiveIsOn_=false;
	serialPortReceiveBitCounter_=0;
	inputShiftReg_=0;
}

int SerialPort::readMode(void)
{
	if(!(pToSCON_->readValue()&bitsSCON_.bitSM0) && !(pToSCON_->readValue()&bitsSCON_.bitSM1))
		return 0;
	if(!(pToSCON_->readValue()&bitsSCON_.bitSM0) && (pToSCON_->readValue()&bitsSCON_.bitSM1))
		return 1;
	if((pToSCON_->readValue()&bitsSCON_.bitSM0) && !(pToSCON_->readValue()&bitsSCON_.bitSM1))
		return 2;
	if((pToSCON_->readValue()&bitsSCON_.bitSM0) && (pToSCON_->readValue()&bitsSCON_.bitSM1))
		return 3;
	return -1;
}

void SerialPort::charToReceiveBuff(int const schar)
{
	if(!(pToSCON_->readValue()&bitsSCON_.bitRI) && (pToSCON_->readValue()&bitsSCON_.bitREN))
	{
		//Mode 0
		if(!(pToSCON_->readValue()&bitsSCON_.bitSM0) && !(pToSCON_->readValue()&bitsSCON_.bitSM1))
		{
			*pToSCON_ |= bitsSCON_.bitRI;
			//*pToSBUF_ = static_cast<unsigned char>(schar);
			//sbufReceive_ = pToSBUF_->readValue();
			sbufReceive_ = static_cast<unsigned char>(schar);
		}
		else //Modet 1,2 ja 3
		{
			if(schar&0x100)      //Jos stoppibitti modessa 1 ja multiprocessor modessa 2 ja 3
				*pToSCON_ |= bitsSCON_.bitRB8;
			else
				*pToSCON_ &= static_cast<unsigned char>(~bitsSCON_.bitRB8);

			if(!((pToSCON_->readValue())&bitsSCON_.bitSM2) || (((pToSCON_->readValue())&bitsSCON_.bitSM2) && (pToSCON_->readValue()&bitsSCON_.bitRB8)))
			{
				*pToSCON_ |= bitsSCON_.bitRI;
				//*pToSBUF_ = static_cast<unsigned char>(schar);
				//sbufReceive_ = pToSBUF_->readValue();
				sbufReceive_ = static_cast<unsigned char>(schar);
			}
		}
	}
}

int SerialPort::readInputShiftReg(int &bitToReceive)
{
	bitToReceive=serialPortReceiveBitCounter_-1;
	return inputShiftReg_;
}

bool SerialPort::receiveEnabled(void)
{
	unsigned char ren_bit=pToSCON_->readValue() & bitsSCON_.bitREN;
	bool retVal=false;
	if(ren_bit)
	{
		if(!lastRENValue_)
		{
			//Alkuarvoja vastaanottoa varten
			serialPortReceiveBitCounter_=0;
			inputShiftReg_=0;
			serialPortReceiveIsOn_=false;
			lastRxDPinValue_=pToIOPort_->readValue() & ioPort_.pinRx;
		}
		retVal=true;
	}
	lastRENValue_=ren_bit;
	return retVal;
}

bool SerialPort::receive1To0Detect(void)
{
	bool retVal=false;
	unsigned char rxd=pToIOPort_->readValue() & ioPort_.pinRx;

	if(!serialPortReceiveIsOn_)
		if((!rxd)&&lastRxDPinValue_)
			retVal=true;
	
	lastRxDPinValue_=rxd;
	return retVal;
}

void SerialPort::handlingReceive(int mode, unsigned long clockTicks, bool const timerOverflow)
{
	if(mode)
	{
		if(receive1To0Detect())
		{
			serialPortReceiveStartClockCounter_=clockTicks;
			serialPortReceiveTimerOverflowCounter_=serialPortM1M3TimerDivider_/2; //Viivettä ekabittiin
			serialPortReceiveBitCounter_=0;
			serialPortReceiveIsOn_=true;
			inputShiftReg_=0;
		}
	}
	
	switch(mode)
	{
		case 0:
			//Mode 0 nopeus 1/12 oskillattorin taajuus = konejakso=>
			//Katsotaan TXD:n muutosta nollaksi. Toimii kellona datalle. Koska fosc/12 = konejakso eli steppi
			//Portin nollaus merkki lähetyksen alkamisesta. Oikeasti kellopulssi stepissä.
			//Ei toimi oikein, koska ei ota huomioon useamman konejakson käskyjä. DO NOT USE This MODE at SIMU
			//Käyttävä ohjelma kirjoittaa charToReceiveBuff metodille kun lähetetty data valmis.
			/*if(!(pToIOPort_->readValue() & ioPort_.pinTx))
			{
				if(serialPortReceiveBitCounter_==0)
					inputShiftReg_=0;
				if(serialPortReceiveBitCounter_<8)
				{
					//Datat
					if(pToIOPort_->readValue()&ioPort_.pinRx)
						inputShiftReg_|=1<<serialPortReceiveBitCounter_;
				}
				if(serialPortReceiveBitCounter_==7)
					charToReceiveBuff(inputShiftReg_);
				serialPortReceiveBitCounter_++;
			}
			else
				serialPortReceiveBitCounter_=0;
			*/
			break;
		case 1:
			//Mode 1 Timer 1 overflow rate / 32
			//10bittiä vastaanotetaan
			if(serialPortReceiveIsOn_)
			{
				if(timerOverflow)
				{
					serialPortReceiveTimerOverflowCounter_++;
					if(serialPortReceiveTimerOverflowCounter_ >= serialPortM1M3TimerDivider_)
					{
						serialPortReceiveTimerOverflowCounter_=0;
						if(serialPortReceiveBitCounter_ < 10)
						{
							if(pToIOPort_->readValue() & ioPort_.pinRx)
								inputShiftReg_|=1<<(serialPortReceiveBitCounter_);
							serialPortReceiveBitCounter_++;
						}
						else
						{
							serialPortReceiveIsOn_=false;
							charToReceiveBuff(inputShiftReg_>>1);
							serialPortReceiveBitCounter_=0;
						}
					}
				}
			}
			break;
		case 2:
			if(serialPortReceiveIsOn_)
			{
				//Mode 2   1/64 oskillaattorin taajuudesta = n.5,3 konejaksoa
				//11bittiä kestää  60 konejaksoa
				if(clockTicks>=(24+serialPortReceiveStartClockCounter_+serialPortM2ClockDivider_*serialPortReceiveBitCounter_))
				{
					//Datat
					receiveMode2Mode3();
				}
			}
			break;
		case 3:
			if(serialPortReceiveIsOn_)
			{
				//Mode 3 Timer 1 overflow rate / 32
				//11 bittiä lähetetään
				if(timerOverflow)
				{
					serialPortReceiveTimerOverflowCounter_++;
					if(serialPortReceiveTimerOverflowCounter_ >= serialPortM1M3TimerDivider_)
					{
						serialPortReceiveTimerOverflowCounter_=0;
						receiveMode2Mode3();
					}
				}
			}
			break;
	}
}

void SerialPort::receiveMode2Mode3(void)
{
	if(serialPortReceiveBitCounter_==11)
	{
		serialPortReceiveIsOn_=false;
		charToReceiveBuff(inputShiftReg_>>1);
		serialPortReceiveBitCounter_=0;
	}
	if(serialPortReceiveIsOn_)
	{
		if(pToIOPort_->readValue() & ioPort_.pinRx)
			inputShiftReg_|=1<<(serialPortReceiveBitCounter_);
		serialPortReceiveBitCounter_++;
	}
}

void SerialPort::stepRxTx(unsigned long clockTicks, bool const timerOverflow)
{
	bool ren=receiveEnabled();

	if(serialPortTransmitIsOn_ || ren)
	{
		int mode=readMode();
		if(serialPortTransmitIsOn_)
			handlingTransmit(mode, clockTicks, timerOverflow);
		if(ren)
			handlingReceive(mode, clockTicks, timerOverflow);
	}
}

bool SerialPort::isTaransmitOn(unsigned char mode)
{
	switch(mode)
	{
		case 0:
			return serialPortTransmitMode0IsOn_;
			break;
		default:
			return serialPortTransmitIsOn_;
	}
}

int SerialPort::readTransmitSbuf(int &bitToTransmit)
{
	bitToTransmit=serialPortTransmitBitCounter_-1;
	return sbufTransmit_;
}

int SerialPort::readTransmitSbufMode0(int &bitToTransmit)
{
	bitToTransmit=serialPortTransmitBufferReadCounter_;
	if(serialPortTransmitBufferReadCounter_<MODE0_BITSTOSENDRECEIVE)
	{
		serialPortTransmitBufferReadCounter_++;
		if(serialPortTransmitBufferReadCounter_>=MODE0_BITSTOSENDRECEIVE)
		{
			serialPortTransmitBufferReadCounter_=MODE0_BITSTOSENDRECEIVE;
		}
	}
	return sbufTransmit_;
}

int SerialPort::readTransmittedValue(void)
{
	if(readMode())
	{
		int rv=lastSbufTransmit_>>1;//Starttibitti pois
		if(rv&0x200)//Multiprocessor mode
			return rv&0x1FF;
		return rv&0xFF;
	}
	return lastSbufTransmit_;
}

void SerialPort::stopTransmitMode0(void)
{
	transmitStop();
	serialPortTransmitMode0IsOn_=false;
}

void SerialPort::transmitStop(void)
{
	serialPortTransmitIsOn_=false;      //Lähetys seis
	serialPortTransmitBitCounter_=0;
	*pToSCON_|=bitsSCON_.bitTI;    //Keskeytys
	lastSbufTransmit_=sbufTransmit_;
}

//Ohjaa sarjaportin lähetystä eri moodeissa
void SerialPort::handlingTransmit(int mode, unsigned long clockTicks, bool const timerOverflow)
{
	switch(mode)
	{
		case 0:
			//Mode 0 nopeus 1/12 oskillattorin taajuus = konejakso=>
			//kokonaisuudessaan 8 bittiä kestää 10 konejaksoa 2 jaksoa menee höpinöihin
			//Nollataan TXD lähtyksen merkiksi. Toimii kellona datalle. Koska fosc/12 = konejakso eli steppi
			//Portin nollaus merkki lähetyksen alkamisesta. Oikeasti kellojakso stepissä.
			//Ei toimi oikein, koska ei ota huomioon useamman konejakson käskyjä. DO NOT USE This SIMU
			//Pitäisi ilmeisesti rakentaa ExternalCycle luokkaan. BUILDATTU!
			//*pToIOPort_ &= static_cast<unsigned char>(~ioPort_.pinTx);
			//if((serialPortTransmitBitCounter_<MODE0_BITSTOSENDRECEIVE)&&(serialPortTransmitBufferReadCounter_<MODE0_BITSTOSENDRECEIVE))
			if(serialPortTransmitBufferReadCounter_<MODE0_BITSTOSENDRECEIVE)
			{
				//if(clockTicks>=serialPortTransmitStartClockCounter_*serialPortM0ClockDivider_*serialPortTransmitBitCounter_)
				//{
					if(!serialPortTransmitMode0IsOn_)
						serialPortTransmitMode0IsOn_=true;
					/*
					//Datat
					if((sbufTransmit_>>(serialPortTransmitBitCounter_))&BIT_0)
						*pToIOPort_|=ioPort_.pinRx;//Modessa 0 Datan lähetys tapahtuu RXD: kautta
					else
						*pToIOPort_ &= static_cast<unsigned char>(~ioPort_.pinRx);
					*/
					serialPortTransmitBitCounter_++;
				//}
			}
			break;
		case 1:
			//Mode 1 Timer 1 overflow rate / 32
			//10bittiä lähetetään
			if(timerOverflow)
			{
				serialPortTransmitTimerOverflowCounter_++;
				if(serialPortTransmitTimerOverflowCounter_ >= serialPortM1M3TimerDivider_)
				{
					serialPortTransmitTimerOverflowCounter_=0;
					if(serialPortTransmitBitCounter_ < 10)
					{
						//Datat aloitetaan start bitillä
						if((sbufTransmit_>>(serialPortTransmitBitCounter_))&BIT_0)
							*pToIOPort_ |= ioPort_.pinTx;
						else
							*pToIOPort_ &= static_cast<unsigned char>(~ioPort_.pinTx);
						serialPortTransmitBitCounter_++;
					}
					else
					{
						transmitStop();
						*pToIOPort_|=ioPort_.pinTx;//Nostetaan TXD jälleen ykköseksi merkki lähetyksen loppumisesta
					}
				}
			}
			break;
		case 2:
			//Mode 2   1/64 oskillaattorin taajuudesta = n.5,3 konejaksoa
			//11bittiä kestää  60 konejaksoa
			if(clockTicks>=(serialPortTransmitStartClockCounter_+serialPortM2ClockDivider_*serialPortTransmitBitCounter_))
			{
				//Datat
				//Lähetetään bitti kerrallaan porttiin:
				sendMode2Mode3();
			}
			break;
		case 3:
			//Mode 3 Timer 1 overflow rate / 32
			//11 bittiä lähetetään
			if(timerOverflow)
			{
				serialPortTransmitTimerOverflowCounter_++;
				if(serialPortTransmitTimerOverflowCounter_ >= serialPortM1M3TimerDivider_)
				{
					serialPortTransmitTimerOverflowCounter_=0;
					//Lähetetään bitti kerrallaan porttiin:
					sendMode2Mode3();
				}
			}
			break;
	}
}

void SerialPort::sendMode2Mode3(void)
{
	if(serialPortTransmitBitCounter_==11)
	{
		transmitStop();
		*pToIOPort_|=ioPort_.pinTx;
	}
	if(serialPortTransmitIsOn_)
	{
		if((sbufTransmit_>>(serialPortTransmitBitCounter_))&BIT_0)
			*pToIOPort_ |= ioPort_.pinTx;
		else
			*pToIOPort_ &= static_cast<unsigned char>(~ioPort_.pinTx);
		serialPortTransmitBitCounter_++;
	}
}

void SerialPort::startTransmit(unsigned int const ch, unsigned long const clockTicks)
{
	sbufTransmit_=ch;
	int mode=readMode();
	if(mode)
	{
		sbufTransmit_<<=1; //Starttibitti
		if(mode==1)
		{
			sbufTransmit_|= 0x200; //Stoppi
		}
		else
		{
			if(pToSCON_->readValue()&bitsSCON_.bitTB8)
				sbufTransmit_|= 0x200;
			else
				sbufTransmit_ &= ~0x200;
			sbufTransmit_|= 0x400; //Stoppi
		}
	}
	serialPortTransmitIsOn_=true;
	serialPortTransmitStartClockCounter_=clockTicks;
	serialPortTransmitTimerOverflowCounter_=0;
	serialPortTransmitBitCounter_=0;
	serialPortTransmitBufferReadCounter_=0;
	serialPortTransmitMode0IsOn_=false;
}

void SerialPort::saveWorkDataToFile(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortM1M3TimerDivider_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortM2ClockDivider_;
	
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)sbufReceive_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)inputShiftReg_;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)lastRENValue_;
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortReceiveStartClockCounter_;
	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortReceiveBitCounter_;
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortReceiveTimerOverflowCounter_;
	saveFileStream<<(Q_UINT16)8<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)serialPortReceiveIsOn_;
	saveFileStream<<(Q_UINT16)9<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)lastRxDPinValue_;
	
	saveFileStream<<(Q_UINT16)10<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)sbufTransmit_;
	saveFileStream<<(Q_UINT16)11<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)lastSbufTransmit_;
	saveFileStream<<(Q_UINT16)12<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortTransmitStartClockCounter_;
	saveFileStream<<(Q_UINT16)13<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortTransmitBufferReadCounter_;
	saveFileStream<<(Q_UINT16)14<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortTransmitBitCounter_;
	saveFileStream<<(Q_UINT16)15<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)serialPortTransmitTimerOverflowCounter_;
	saveFileStream<<(Q_UINT16)16<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)serialPortTransmitIsOn_;
	saveFileStream<<(Q_UINT16)17<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)serialPortTransmitMode0IsOn_;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void SerialPort::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value32;
				serialPortM1M3TimerDivider_=value32;
				break;
			case 1:
				readFileStream>>typeReadData>>value32;
				serialPortM2ClockDivider_=value32;
				break;
			case 2:
				readFileStream>>typeReadData>>value32;
				sbufReceive_=(int)value32;
				break;
			case 3:
				readFileStream>>typeReadData>>value32;
				inputShiftReg_=(int)value32;
				break;
			case 4:
				readFileStream>>typeReadData>>value8;
				lastRENValue_=(unsigned char)value8;
				break;
			case 5:
				readFileStream>>typeReadData>>value32;
				serialPortReceiveStartClockCounter_=(unsigned long)value32;
				break;
			case 6:
				readFileStream>>typeReadData>>value32;
				serialPortReceiveBitCounter_=(int)value32;
				break;
			case 7:
				readFileStream>>typeReadData>>value32;
				serialPortReceiveTimerOverflowCounter_=(int)value32;
				break;
			case 8:
				readFileStream>>typeReadData>>value8;
				serialPortReceiveIsOn_=(bool)value8;
				break;
			case 9:
				readFileStream>>typeReadData>>value8;
				lastRxDPinValue_=(unsigned char)value8;
				break;
			case 10:
				readFileStream>>typeReadData>>value32;
				sbufTransmit_=(int)value32;
				break;
			case 11:
				readFileStream>>typeReadData>>value32;
				lastSbufTransmit_=(int)value32;
				break;
			case 12:
				readFileStream>>typeReadData>>value32;
				serialPortTransmitStartClockCounter_=value32;
				break;
			case 13:
				readFileStream>>typeReadData>>value32;
				serialPortTransmitBufferReadCounter_=(unsigned int)value32;
				break;
			case 14:
				readFileStream>>typeReadData>>value32;
				serialPortTransmitBitCounter_=(int)value32;
				break;
			case 15:
				readFileStream>>typeReadData>>value32;
				serialPortTransmitTimerOverflowCounter_=(int)value32;
				break;
			case 16:
				readFileStream>>typeReadData>>value8;
				serialPortTransmitIsOn_=(bool)value8;
				break;
			case 17:
				readFileStream>>typeReadData>>value8;
				serialPortTransmitMode0IsOn_=(bool)value8;
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

