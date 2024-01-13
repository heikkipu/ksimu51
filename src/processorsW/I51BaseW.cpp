/**************************************************************************
*   Copyright (C) 2005 - ... Heikki Pulkkinen                             *
*   heikki@CompaQ7113EA                                                   *
*                                                                         *
z*   This program is free software; you can redistribute it and/or modify  *
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
#include "I51BaseW.h"
#include "../ksimu51.h"

#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kpixmap.h>

#include <qapplication.h>
#include <qlayout.h>
#include <qdir.h>
#include <string>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>

//I51BASEW
I51BaseW::I51BaseW(QWidget* parent, char const * name, int wflags, int const index, int const type) : WorkspaceMainW(parent, name, wflags)
{
	external_disassemblew_=0;
	external_assemblersourcew_=0;
	sfrw_=0;
	regsw_=0;
	miscw_=0;
	sfrbitw_=0;
	bitaddressableareaw_=0;
	registerbankareaw_=0;
	scratchpadareaw_=0;
	external_romw_=0;
	externalw_=0;

	//Ladataan jokunen tarvittava ikoni
	resetIcon_.load ( "pics/KS51hi32ResetProcessor.png", 0, KPixmap::Auto );
	runIcon_.load ( "pics/KS51hi32RunProcessor.png", 0, KPixmap::Auto );
	intelIcon_.load ( "pics/KS51hi32Intel8051.png", 0, KPixmap::Auto );
	whiteLedIcon_.load("pics/KS51hi16WhiteLedOn.png", 0, KPixmap::Auto );

	running_=false;
	firstProgramLoad_=true;
	readyToRun_=false;
	loadingWorkFile_=false;
	resetActiveLastValue_=true;

	internalR0_=true;
	internalR1_=true;
	pointerPC_WCount_=0;

	addPopupMenu();

	externalProgramMemoryHexFilename_=QString::null;

	connect(this, SIGNAL(workspaceWindowActivated()), this, SLOT(childWindowActivated()));

	processor_=new I8031;
	processorIndex_=index;
	processorType_=type;
	processor_->powerOnReset();
	pinEA_Index_=processor_->readPinIndex(PINNAME_EA);

	previousStepSerialTransmitFlag_=processor_->isSerialPortTaransmitOn(I8031_NameS::UART);

	horizontalTileDivider_=2;
}

I51BaseW::~I51BaseW()
{

}

//8031
QString I51BaseW::readProcessorName(void)
{
	return QString(processor_->readProcessorName());
}

//name
QString I51BaseW::readProcessorWindowName(void)
{
	return this->name();
}

//Lasketaan processorin koneaika = (1/kellotaajuus) * clockCyclesPerMachineCycle
double I51BaseW::readProcessorMachineCycleTime(void)
{
	double rVal=(1.0/processor_->readClockSpeed())*double (processor_->readClockCyclesPerMachineCycle());
	return rVal;
}

double I51BaseW::readWholeStepTimeCounter(void)
{
	return wholeStepTimeCounter_;
}

bool I51BaseW::readEAPin(void)
{
	return processor_->readPinValue(pinEA_Index_);
}

/*
//Voidaan tarvittaesssa heittää infoa disassemble ikkunan active labeliin.
void I51BaseW::updateDisassembleActiveLable(const QString& text)
{
	if((processorSpecialProperty->isInternalROM())&&(processor_->readPin(PINNAME_EA)))
	{
		if(isIdasw)
			idasw->updateActiveLable(text);
	}
	else
	{
		if(isEdasw)
			external_disassemblew_->updateActiveLable(text);
	}
}
*/
void I51BaseW::updateExternalDataIOAreaAndProcessor(bool const  updateToShow)
{
	//Processori->Ulkoinen muisti
	updateProcessorExDataToExternalArea();
	//Data Pointers DPTR, R0 ja R1
	unsigned int dptrAddress=processor_->readDPTRValue();//Luetaan aktiivinen DPTR
	int usedRegisterBank=processor_->registerBankInUse();
	unsigned char r0Address= processor_->readInternalDataMemory(I8031_NameS::MIN_REGISTER_BANK_ADDRESS+8*usedRegisterBank);
	unsigned char r1Address= processor_->readInternalDataMemory((I8031_NameS::MIN_REGISTER_BANK_ADDRESS+1)+8*usedRegisterBank);
	externalw_->setDatapointerValues(dptrAddress, r0Address, r1Address);

	//Päivitetään komponentit - ajetaan cycle
	externalw_->updateAllComponents(processor_->readLastExternalCycle(), wholeStepTimeCounter_, processor_->readMachineCycles(), processor_->readClockTicks(), processor_->readProgramCounterValue(), updateToShow);
	//ProcessoriIO pinnit lopputilaan
	updateProcessorIOToExternalArea(updateToShow);

	//Ulkoinen muisti -> Processor
	updateExternalAreaToProcessorExData();
	//Ulkoinen IO alue ->ProcessoriIO
	updateExternalAreaToProcessorIO();
}

void I51BaseW::updateProcessorExDataToExternalArea(void)
{
	unsigned int value,address;
	//Päivitetään vain jos ulkoisen muistin osoituksia tapahtunut.
	//Nopeuttaa toimintaa...
	if((processor_->is16bAddressExternalDataMemoryWriteCycleLastStep()) ||(processor_->is8bAddressExternalDataMemoryWriteCycleLastStep()))
	{
		address=0;
		if(processor_->is16bAddressExternalDataMemoryWriteCycleLastStep())
		{
			//address=processor_->readInternalDataMemory(I8031_NameS::P2); //Osoitteen yläosa on Proskun portiss P2.
			address=processor_->readAddressInP2AfterALE();
			address<<=8;
		}
		//Osoitten alaosa saadaan.
		address|=processor_->readAddressInP0AfterALE();
		value=processor_->readExternalDataMemory(address);
		externalw_->writeExternalDataMemoryAreaTable(address, value);
	}
}

void I51BaseW::updateExternalAreaToProcessorExData(void)
{
	unsigned int address, minA, maxA;
	unsigned char value;
	if(externalw_->isExternalDataMemoryAreaTableValueChanged(minA, maxA))
	{
		for(address=minA;address<=maxA;address++)
		{
			if(externalw_->isExternalDataMemoryAreaTableValueChanged(address))
			{
				value=externalw_->readExternalDataMemoryAreaTable(address);
				processor_->writeExternalDataMemory(address, value);
			}
		}
	}
}

void I51BaseW::updateProcessorIOToExternalArea(bool show)
{
	//Pinnit
	externalw_->writePins(show);

	//Portit
	for(unsigned int address=I8031_NameS::MIN_SFR_ADDRESS;address<=I8031_NameS::MAX_SFR_ADDRESS;address++)
	{
		if(processor_->isInternalDataMemoryLocationInUse(address))
			if(processor_->isSFRRegisterIOPort(address))
				externalw_->writePort(address, processor_->readInternalDataMemory(address), show);
	}

}

void I51BaseW::updateExternalAreaToProcessorIO(void)
{
	bool v, hits;
	//Ekaksi yksittäiset processorin inputpinnit
	IOPin* processorPin=processor_->readPins()->pToFirstPin();
	//IOPin* externalPin=externalw_->readPinsList(); 
	IOPin* externalPin=externalw_->readPinsListSampleInputs();
	while((processorPin!=0)&&(externalPin!=0))
	{
		if(processorPin->isIn()&&externalPin->isValueChanged())
		{
			v=externalPin->readValue(true); //Nollataan changed pitti luettaessa.
			processorPin->writeValue(v, false); //Ei tartte asettaa muuttuneeksi.
		}
		processorPin=processorPin->next();
		externalPin=externalPin->next();
	}
	//Sitten portien Pinnit
	//IOPortPin* externalPortPinList=externalw_->readPortPinList();
	IOPortPin* externalPortPinList=externalw_->readPortPinListSampleInputs();
	unsigned char bitCount, bitsToZeroByte, bitsToOneByte, valueToWrite;
	for(unsigned int address=I8031_NameS::MIN_SFR_ADDRESS;address<=I8031_NameS::MAX_SFR_ADDRESS;address++)
	{
		if((processor_->isInternalDataMemoryLocationInUse(address))&&(processor_->isSFRRegisterIOPort(address)))
		{
			hits=false;
			bitsToZeroByte=0xFF; bitsToOneByte=0;
			for(bitCount=0;bitCount<8;bitCount++)
			{
				if(externalPortPinList->isValueChanged())
				{
					v=externalPortPinList->readValue(true);
					if(v)
						bitsToOneByte|=1<<bitCount;
					else
						bitsToZeroByte&=~(1<<bitCount);
					hits=true;
				}
				externalPortPinList=externalPortPinList->next();
			}
			if(hits)
			{
				valueToWrite=processor_->readInternalDataMemory(address);
				valueToWrite|=bitsToOneByte;
				valueToWrite&=bitsToZeroByte;
				processor_->writeInternalDataMemory(address, valueToWrite);
			}
		}
	}
}

//Kopioitaessa prosessri luetaan komponentit
QWidgetList I51BaseW::readExternalAreaComponents(void)
{
	return externalw_->listOfComponents();
}

//Kopioitaessa  processori lisätään komponentit ikkunaan
void I51BaseW::addExternalAreaComponents(QWidgetList externalDataAreaComponentList)
{
	externalw_->addComponents(externalDataAreaComponentList);
}

IOPin* I51BaseW::readExternalAreaNets(void)
{
	return externalw_->listOfNets();
}

void I51BaseW::addExternalAreaNets(IOPin* nets)
{
	externalw_->addNets(nets);
}

void I51BaseW::updateProgramMemoryWindows(void)
{
	unsigned long address=processor_->readProgramCounterValue();
	//Disassemble
	if(external_disassemblew_)
		external_disassemblew_->update(address, processor_->readLastExecutionProgramCounterValue(), lastStepProgramCounterValue);
	//Source
	//Käytetään samaa osoitetta kuin disassemble
	if(external_assemblersourcew_)
		external_assemblersourcew_->updateAsmSource(address, lastStepProgramCounterValue);
	//External ROM
	if(external_romw_)
		//external_romw_->focusAddress(address);
		external_romw_->setProgramCounterPointer(address);

}

void I51BaseW::updateSFRWindow(void)
{
	//SFR Area
	int n=0;
	int m=0;
	unsigned long address;
	unsigned char ucval;
	for(address=I8031_NameS::MIN_SFR_ADDRESS;address<=I8031_NameS::MAX_SFR_ADDRESS;address++)
	{
		if(processor_->isInternalDataMemoryLocationInUse(address))
		{
			if(processor_->isInternalDataMemoryLocationChanged(address))
			{
				ucval=processor_->readInternalDataMemory(address);
				//SFR window
				sfrw_->setValue(address, ucval);
				//SFR Regs window
				regsw_->setValue(n, ucval);
				//SFR BIT window
				if(processor_->isSFRBitNamesInUse(address))
					sfrbitw_->setRegsBitsValue(m,ucval);
				//Päivitetään sourceW:n käyttäjän nimeämät portit
				if(processor_->isSFRRegisterIOPort(address))
				{
					if(external_assemblersourcew_)
						external_assemblersourcew_->updateNamedPortBits(address, ucval);
				}
			}
			n++;
			if(processor_->isSFRBitNamesInUse(address))
				m++;
		}
	}
}

void I51BaseW::updateMiscWindow(void)
{
	int m=0;
	//Kellotaajuus
	miscw_->setValue(m++, processor_->readClockSpeed());
	//Machine cycles
	miscw_->setValue(m++, processor_->readMachineCycles());
	//Clk ticks
	miscw_->setValue(m++, processor_->readClockTicks());
	//Kokonaisaikalaskuri
	miscw_->setValue(m++, wholeStepTimeCounter_);
	//PC
	miscw_->setValue(m++, processor_->readProgramCounterValue());
	//DPTR
	miscw_->setValue(m++, (unsigned long)processor_->readDPTRValue());
	//Timerit
	miscw_->setValue(m++, (unsigned long)processor_->readTimerValue16(0));
	miscw_->setValue(m++, (unsigned long)processor_->readTimerValue16(1));
	//Sarjaportin shift rekkarit
	int sreg=processor_->readSerialPortRxBits(I8031_NameS::UART);
	unsigned int val=sreg&0x0FFF;
	miscw_->setValue(m++, val, sreg>>12);
	sreg=processor_->readSerialPortTxBits(I8031_NameS::UART);
	val=sreg&0x0FFF;
	miscw_->setValue(m++, val, sreg>>12);
}

void I51BaseW::updateRegisterBankWindow(void)
{
	for(unsigned long address=I8031_NameS::MIN_REGISTER_BANK_ADDRESS;address<=I8031_NameS::MAX_REGISTER_BANK_ADDRESS;address++)
		if(processor_->isInternalDataMemoryLocationChanged(address))
			registerbankareaw_->setValue(address, processor_->readInternalDataMemory(address));
	
	registerbankareaw_->setPointers(spValue(), r0Value(), r1Value());
	registerbankareaw_->selectActiveRegisterBankRow(processor_->registerBankInUse());
}

void I51BaseW::updateBitAddressableAreaWindow(void)
{
	for(unsigned long address=I8031_NameS::MIN_BIT_AREA_ADDRESS;address<=I8031_NameS::MAX_BIT_AREA_ADDRESS;address++)
	{
		unsigned char ucval=processor_->readInternalDataMemory(address);
		bitaddressableareaw_->setValue(address, ucval);
		if(external_assemblersourcew_)//Assembler Source Windowin nimetyt bitit
			external_assemblersourcew_->updateNamedBits(address, ucval);
	}
	bitaddressableareaw_->setPointers(spValue(), r0Value(), r1Value());
}

void I51BaseW::updateScratchPadAreaWindow(void)
{
	for(unsigned long address=I8031_NameS::MIN_SCRATCH_PAD_ADDRESS;address<=I8031_NameS::MAX_SCRATCH_PAD_ADDRESS;address++)
		if(processor_->isInternalDataMemoryLocationChanged(address))
			scratchpadareaw_->setValue(address, processor_->readInternalDataMemory(address));
	
	scratchpadareaw_->setPointers(spValue(), r0Value(), r1Value());
}

void I51BaseW::updateProcessorWindows(void)
{
	//Jos ohjelma ladattu on myös ikkunat:
	if(!firstProgramLoad_)
	{
		updateProcessorWindows_VirtualPart();
		updateProgramMemoryWindows();
		updateSFRWindow();
		updateMiscWindow();
		updateRegisterBankWindow();
		updateBitAddressableAreaWindow();
		updateScratchPadAreaWindow();
	}
	//External DATA
	updateExternalDataIOAreaAndProcessor(true);
}

void I51BaseW::powerOnReset(void)
{
	wholeStepTimeCounter_=0.0;
	wholeStepTimeCounterInSync_=0.0;
	processor_->powerOnReset();
	clearInternalRamAreas();
	clearExternalAreas();
	isAtBreakpoint(); //Resettaa bitChangeBreakpointit
	processor_->writeInputPin(PINNAME_RESET, false); //resettipinni pois päältä jos on
	this->setIcon(intelIcon_);

	if(external_disassemblew_)
		external_disassemblew_->clearExecutionPoint();
	if(external_assemblersourcew_)
		external_assemblersourcew_->clearExecutionPoint();

	lastStepProgramCounterValue=processor_->readProgramCounterValue();
	externalw_->resetComponents();
	
	updateProcessorWindows();
}

void I51BaseW::clearInternalRamAreas(void)
{
	//Register BANK
	registerbankareaw_->clear();
	//Bit Addressable Area
	bitaddressableareaw_->clear();
	//Scratch Pad Area
	scratchpadareaw_->clear();

}

void I51BaseW::clearExternalAreas(void)
{
	externalw_->clearComponents();
}

//Ollanko breikpointissa
bool I51BaseW::isAtBreakpoint(void)
{
	bool isAt=isAtBreakpoint_VirtualPart();

	//Looking for SFRBitW bit change
	if(sfrbitw_->isAtBitChangeBreakpoint(processor_)!=BitChangeBreakpoint::NO_BITCHANGES)
		isAt=true;

	return isAt;
}

bool I51BaseW::isAtBreakpoint_VirtualPart(void)
{
	bool isAt=false;
	if(external_disassemblew_)
		if(external_disassemblew_->isAtBreakpoint(processor_->readProgramCounterValue()))
			isAt=true;
	if(external_assemblersourcew_)
	{
		if(external_assemblersourcew_->isAtBreakpoint(processor_->readProgramCounterValue()))
			isAt=true;
		if(external_assemblersourcew_->isAtBitChangeBreakpoint(processor_)!=BitChangeBreakpoint::NO_BITCHANGES)
			isAt=true;
	}
	return isAt;
}


void I51BaseW::showBreakpoint(void)
{
	showBreakpoint_VirtualPart();

	if(sfrbitw_->showBitChangeBreakpointBit())
		sfrbitw_->setFocus();

	centerActiveWindow();
}

void I51BaseW::showBreakpoint_VirtualPart(void)
{
	if(external_assemblersourcew_)
		if(external_assemblersourcew_->showBitChangeBreakpoint()||external_assemblersourcew_->isAtBreakpoint(processor_->readProgramCounterValue()))
			external_assemblersourcew_->setFocus();

	if(external_disassemblew_)
		if(external_disassemblew_->isAtBreakpoint(processor_->readProgramCounterValue()))
			external_disassemblew_->setFocus();
}

bool I51BaseW::step(bool const  disassemble, double const  stepTime)
{
	if(external_disassemblew_&&!processor_->readPinValue(pinEA_Index_))
		return doStep(disassemble, stepTime);
	return false;
}

//jos dissassemble tosi tekee disassemble listan myös I8031 luokassa
bool I51BaseW::doStep(bool const  disassemble, double const  stepTime)
{
	bool steppi=false;
	double machineCycleTime=readProcessorMachineCycleTime() * ( double(processor_->readMachineCycles()+processor_->calculateNextStepMachineCycles()));
	if(stepTime)//Jos laskennallista aikaa
	{

		if(wholeStepTimeCounterInSync_>=machineCycleTime)//Stepitetään vain jos aika ylittynyt
			steppi=true;
		wholeStepTimeCounterInSync_+=stepTime;
	}
	else //tai jos stepTime==0
	{
		//stepitetään kerran ja päivitetään aikalaskuri kohdalleen
		steppi=true;
	}

	if(steppi)
	{
		if(processor_->isSerialPortTaransmitOn(I8031_NameS::UART))
			previousStepSerialTransmitFlag_=true;
		processor_->step(disassemble, true);
		wholeStepTimeCounter_=machineCycleTime;
		updateExternalDataIOAreaAndProcessor(false);
	}

	//Visualisoidaan resettipinnin asettumista.
	bool resetActive=processor_->isAtReset();
	if(running_)
	{
		//Resetin nouseva reuna
		if(!resetActiveLastValue_&&resetActive)
		{
			this->setIcon(resetIcon_);
			//wholeStepTimeCounter_=0.0;
			//wholeStepTimeCounterInSync_=0.0;
		}
		//Resetin laskeva reuna
		if(resetActiveLastValue_&&!resetActive)
		{
			this->setIcon(runIcon_);
		}
	}
	else
	{
		if(resetActive)
		{
			this->setIcon(resetIcon_);
			//wholeStepTimeCounter_=0.0;
			//wholeStepTimeCounterInSync_=0.0;
		}
		else
			this->setIcon(intelIcon_);
	}

	resetActiveLastValue_=resetActive;
	lastStepProgramCounterValue=processor_->readProgramCounterValue();

	return steppi;
}

void I51BaseW::run(void)
{
	running_=true;
	if(processor_->isAtReset())
	{
		this->setIcon(resetIcon_);
		//wholeStepTimeCounter_=0.0;
		//wholeStepTimeCounterInSync_=0.0;
	}
	else
		this->setIcon(runIcon_);
}

void I51BaseW::stop(void)
{
	running_=false;
	this->setIcon(intelIcon_);
}

//Näyttää tuodun pointteri ikkunan.
void I51BaseW::showPointerPC(void)
{
	updateProcessorWindows();
	switch(pointerPC_WCount_)
	{
		case 0:
			if(external_disassemblew_)
			{
				external_disassemblew_->setFocus();
				external_disassemblew_->update(processor_->readProgramCounterValue(), processor_->readLastExecutionProgramCounterValue(), lastStepProgramCounterValue);
			}
			break;
		case 1:
			if(external_assemblersourcew_)
			{
				external_assemblersourcew_->setFocus();
				external_assemblersourcew_->updateAsmSource(processor_->readProgramCounterValue(), lastStepProgramCounterValue);
			}
			break;
		case 2:
			if(external_romw_)
			{
				external_romw_->setFocus();
				external_romw_->focusAddress(processor_->readProgramCounterValue());
			}
		default:
			pointerPC_WCount_=-1;
	}
	pointerPC_WCount_++;
	centerActiveWindow();
}

void I51BaseW::showPointerSP(void)
{
	updateProcessorWindows();   //Päivitettää ens ikkunat
	bitaddressableareaw_->showPointerSP();
	registerbankareaw_->showPointerSP();
	scratchpadareaw_->showPointerSP();

	centerActiveWindow();
}

void I51BaseW::showPointerR0(void)
{
	updateProcessorWindows();   //Päivitettää ens ikkunat
	if(internalR0_)
	{
		bitaddressableareaw_->showPointerR0();
		registerbankareaw_->showPointerR0();
		scratchpadareaw_->showPointerR0();
		internalR0_=false;
	}
	else
	{
		externalw_->showPointerR0();
		internalR0_=true;
	}
	centerActiveWindow();
}

void I51BaseW::showPointerR1(void)
{
	updateProcessorWindows();   //Päivitettää ens ikkunat

	if(internalR1_)
	{
		bitaddressableareaw_->showPointerR1();
		registerbankareaw_->showPointerR1();
		scratchpadareaw_->showPointerR1();
		internalR1_=false;
	}
	else
	{
		externalw_->showPointerR1();
		internalR1_=true;
	}
	centerActiveWindow();
}
void I51BaseW::showPointerDPTR(void)
{
	updateProcessorWindows();   //Päivitettää ens ikkunat

	externalw_->showPointerDPTR();

	centerActiveWindow();
}

bool I51BaseW::isSerialPortTransmitOn(void)
{
	return processor_->isSerialPortTaransmitOn(I8031_NameS::UART);
}

bool I51BaseW::readPrevoiusStepSerialTransmitFlag(void)
{
	bool rV=previousStepSerialTransmitFlag_;
	previousStepSerialTransmitFlag_=false; //Nollataan jos oli päällä, jottei tulosteta turhaan monta kertaa
	return rV;
}

int I51BaseW::lastTransmittedData(void)
{
	return processor_->readSerialPortTransmittedValue(I8031_NameS::UART);
}

void I51BaseW::writeDataToSerialPort(int data)
{
	processor_->charToSerialPortReceiveBuff(I8031_NameS::UART, data);
}

bool I51BaseW::readTxDPin(void)
{
	return static_cast<bool>(processor_->readPortPin(I8031_NameS::P3, I8031_NameS::P3_TXD_NUM));
}

void I51BaseW::writeRxDPin(bool const value)
{
	processor_->writePortPin(I8031_NameS::P3, I8031_NameS::P3_RXD_NUM, value);
}

//Luodaan ne ikkunat jotka tarvitsee luoda vain ensimmäisellä ohjelman latauskerralla
void I51BaseW::createStaticWindows(void)
{
	string apuStr;
	unsigned int processorAddress;

	//Regs
	regsw_=new RegsW(windowWorkspace_, WINDOWNAME_REGS, WDestructiveClose|WMouseNoMask);
	regsw_->setPlainCaption(i18n("REGISTERS"));
	for(processorAddress=I8031_NameS::MIN_SFR_ADDRESS;processorAddress<=I8031_NameS::MAX_SFR_ADDRESS;processorAddress++)
	{
		if(processor_->isInternalDataMemoryLocationInUse(processorAddress))
		{
			apuStr=processor_->readSFRName(processorAddress);
			regsw_->add(processorAddress, apuStr);
		}
	}
	regsw_->show();

	//Misc
	miscw_=new MiscW( windowWorkspace_, WINDOWNAME_MISC, WDestructiveClose|WMouseNoMask);
	miscw_->setPlainCaption(i18n("MISC"));
	//Kellotaajuus
	miscWClockSpeedIndex=miscw_->addRow("Clock Hz", MiscW::EDITABLE);
	//Machine cycles
	miscw_->addRow("Cycles", MiscW::NOT_EDITABLE);
	//Clk ticks
	miscw_->addRow("Ticks", MiscW::NOT_EDITABLE);
	//Kokonaisaika
	miscw_->addRow("Time(s)", MiscW::NOT_EDITABLE);
	//PC
	miscw_->addRow("PC", MiscW::NOT_EDITABLE);
	//DPTR pitkänä
	miscw_->addRow("DPTR", MiscW::NOT_EDITABLE);
	//Timerit
	miscw_->addRow("Timer0", MiscW::NOT_EDITABLE);
	miscw_->addRow("Timer1", MiscW::NOT_EDITABLE);
	//Sarjaportin SBUF
	miscw_->addRow("UART Rx", MiscW::NOT_EDITABLE);
	miscw_->addRow("UART Tx", MiscW::NOT_EDITABLE);
	
	miscw_->show();


	//SFR Area
	sfrw_=new SfrW( windowWorkspace_, WINDOWNAME_SFR_AREA, WDestructiveClose|WMouseNoMask);
	sfrw_->setPlainCaption(i18n("SFR AREA"));
	sfrw_->show();
	for(processorAddress=I8031_NameS::MIN_SFR_ADDRESS;processorAddress<=I8031_NameS::MAX_SFR_ADDRESS;processorAddress++)
	{
		if(processor_->isInternalDataMemoryLocationInUse(processorAddress))
		{
			apuStr=processor_->readSFRName(processorAddress);
			sfrw_->setRegisterName(processorAddress, apuStr);
		}
	}


	//SFR bittiosoitettavat
	sfrbitw_ = new SFRBitW(windowWorkspace_, WINDOWNAME_SFR_BIT, WDestructiveClose|WMouseNoMask);
	sfrbitw_->setPlainCaption(i18n("SFR BITS"));
	for(processorAddress=I8031_NameS::MIN_SFR_ADDRESS;processorAddress<=I8031_NameS::MAX_SFR_ADDRESS;processorAddress++)
	{
		if((processor_->isInternalDataMemoryLocationInUse(processorAddress)) && (processor_->isSFRBitNamesInUse(processorAddress)))
		{
			apuStr=processor_->readSFRName(processorAddress);
			string apuStr2("");
			for(int n=0;n<8;n++)
			{
				apuStr2+=processor_->readSFRBitName(processorAddress,n);
				apuStr2+=";";
			}
			sfrbitw_->insertRegsRow(processorAddress, apuStr, apuStr2);
		}
	}
	sfrbitw_->show();

	//Bittiosoitettava alue
	bitaddressableareaw_=new BitAddressableAreaW(windowWorkspace_, WINDOWNAME_BITADDRESSABLE_AREA, WDestructiveClose|WMouseNoMask);
	bitaddressableareaw_->setPlainCaption(i18n("BIT ADDR AREA"));
	bitaddressableareaw_->show();

	//Register BANKs
	registerbankareaw_=new RegisterBankAreaW(windowWorkspace_, WINDOWNAME_REGISTERBANK_AREA, WDestructiveClose|WMouseNoMask);
	registerbankareaw_->setPlainCaption(i18n("REGISTER BANK AREA"));
	registerbankareaw_->show();

	//Scratch Pad Area
	scratchpadareaw_=new InternalRAMW(windowWorkspace_, WINDOWNAME_SCRATCHPAD_AREA, WDestructiveClose|WMouseNoMask, I8031_NameS::MIN_SCRATCH_PAD_ADDRESS, I8031_NameS::MAX_SCRATCH_PAD_ADDRESS-I8031_NameS::MIN_SCRATCH_PAD_ADDRESS+1, false);
	scratchpadareaw_->setPlainCaption(i18n("SCRATCH PAD AREA"));
	scratchpadareaw_->show();

	createStaticWindows_VirtualPart();
}

void I51BaseW::createExternalAreaWindow(void)
{
	//External Data Area Oma workspace
	externalw_= new ExternalW(processor_, windowWorkspace_, WINDOWNAME_EXTERNAL_AREA, WDestructiveClose|WMouseNoMask);
	externalw_->show();

	countUpWindowsCounter(); //Tämä varmaan riittää tähän ikkunalaskennaksi
}


//Ladataan näyttöikkunoihin arvot ensimmäisen kerran latauksen jälkeen.
//Niihin ikkunoihin jotka ovat dynaamisia: (muuntuvat processorin/ohjelman mukaan)
//Tai ikkunoihin joiden tietoja ei muuteta.
void I51BaseW::insertItems(void)
{
	//Luodaan ne ikkunat jotka eivät vaihdu ohjelman mukaan
	if(firstProgramLoad_)
		createStaticWindows();

	createDynamicWindows();
}

void I51BaseW::createDynamicWindows(void)
{
	processor_->powerOnReset(); //Program counteri alkuun
	unsigned int wState;
	string apuStr;

	if(!processor_->readPinValue(pinEA_Index_))
	{
		//DISASSEMBLE (external)
		createDisassembleWindow(external_disassemblew_, i18n("external"), WINDOWNAME_EXTERNAL_DISASSEMBLE);
		externalProgramMemoryHexFilename_=activeHexFilename_;
		//ExternalROM ikkuna
		//Luodaan jos ei luotu
		if(!external_romw_)
		{
			external_romw_ = new RomW(windowWorkspace_, WINDOWNAME_EXTERNAL_ROM, WDestructiveClose|WMouseNoMask,0,I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE);
			external_romw_->setPlainCaption(i18n("EXTERNAL ROM"));
			external_romw_->setGeometry(0,0,external_romw_->romTableGeometrySize().width()+20, height()/2);
		}

		unsigned int address;
		char value;
		wState=external_romw_->windowState();
		external_romw_->showMinimized();

		for(address=I8031_NameS::MIN_EXTERNAL_PROGRAM_MEMORY_ADDRESS;address<=I8031_NameS::MAX_EXTERNAL_PROGRAM_MEMORY_ADDRESS;address++)
		{
			value=processor_->readExternalProgramMemory(address);
			external_romw_->writeValue(address,value);
		}
		if(wState!=Qt::WindowMinimized)
			external_romw_->showNormal();
	}

	//Source Window
	if(!processor_->readPinValue(pinEA_Index_))
		createAsmSourceWindow(external_assemblersourcew_, i18n("external"), WINDOWNAME_EXTERNAL_ASM_SOURCE);

	createDynamicWindows_VirtualPart();

	//Resetoidaan oletukset näkyviin
	powerOnReset();
	firstProgramLoad_=false;
}

void I51BaseW::createAsmSourceWindow(AssemblerSourceW*& aliasPToW, QString programMemoryName, const char* windowName)
{
	//Ekaksi pitää kattoa löytyykö sourcefileä.
	unsigned int l= activeHexFilename_.length()-4;
	QString sourceFilename(activeHexFilename_);
	sourceFilename.setLength(l); //.hex pois
	bool removeSW=false;
	if((sourceFilename.contains(".h51",false))||(sourceFilename.contains(".a51",false)))
	{
		//Luodaan external SOURCE ikkuna jos ei ole
		if(!aliasPToW)
		{
			aliasPToW = new AssemblerSourceW( windowWorkspace_, windowName, WDestructiveClose|WMouseNoMask);
			aliasPToW->show();
			aliasPToW->setGeometry (0, 0, this->width()-240,380);
		}

		unsigned int wState=aliasPToW->windowState();
		aliasPToW->setPlainCaption(i18n("ASM SOURCE ("+programMemoryName+") : ")+sourceFilename);
		aliasPToW->clear();
		aliasPToW->showMinimized();
		if(aliasPToW->addSourceFile(sourceFilename, activeHexFilename_, processor_))
			aliasPToW->setWindowState(wState);
		else
			removeSW=true;
	}
	else
		removeSW=true;
	if(removeSW)   //Poistetaan source ikkuna, jos oli olemassa
	{
		if(aliasPToW)
			delete aliasPToW;
		aliasPToW=nullptr;
	}
}

void I51BaseW::createDisassembleWindow(DisassembleW*& aliasPToW, QString programMemoryName, const char* windowName)
{
	unsigned int pSize=processor_->readProgramSize();
	//Luodaan ensin ikkuna jos sitä ei ole luotu vielä.
	if(!aliasPToW)
	{
		aliasPToW = new DisassembleW( windowWorkspace_, windowName, WDestructiveClose|WMouseNoMask);
		aliasPToW->setPlainCaption(i18n("DISASSEMBLE ("+programMemoryName+")"));
		aliasPToW->show();
	}
	aliasPToW->clear();
	do
	{
		unsigned int processorAddress=(processor_->readProgramCounterValue());
		processor_->step(true,false);
		aliasPToW->insertItem(processor_->readCodeBufferStr(),processorAddress);
	}
	while(processor_->readProgramCounterValue()<=pSize);
}


void I51BaseW::loadProgramAndCreateAll( const QString& hexFilename, std::vector<std::tuple<unsigned int, unsigned char>>* programLoaded)
{
	activeHexFilename_=hexFilename;
	programLoaded->clear();
	QString apustr;
	if((hexFilename.contains(".hex"))||(hexFilename.contains(".HEX"))||(hexFilename.contains(".ihx"))||(hexFilename.contains(".IHX")))
	{
		//Puretaan HEX fileformaatti
		unsigned long int osoite_laskuri=0, alku_osoite=0, lines=0;
		unsigned int n,osh,osl,maara;
		char merkki,a,b;
		unsigned char arvo;
		bool eka=true;
		const char* afn = activeHexFilename_.ascii();
		ifstream hx(afn);
		if(hx)
		{
			processor_->clearProgramMemory();
			while(hx.get(merkki))
			{
				if(eka && !(merkki==':'))
				{
					hx.close();
					apustr=i18n("Invalid file format in file: ");
					apustr+=activeHexFilename_;
					apustr+="!";
					emit message_SetStatusBar(processorIndex_, apustr, 20000);
					return;
				}
				eka=false;
				if (merkki==':')
				{
					lines++;
					hx>>a;
					hx>>b;
					maara=aschex(a,b);
					if (maara==0)             // Jos mr 0, tiedosto loppu
						break;
					// Osoitteen ylosa
					hx>>a;
					hx>>b;
					osh=aschex(a,b);
					osh=osh&0x0ff;
					alku_osoite=osh<<8;
				// Osoitteen alaosa
					hx>>a;
					hx>>b;
					osl=aschex(a,b);
					osl=osl&0x0ff;
					alku_osoite+=osl;              // Listn edelliseen osoitteeseen
					arvo=0;

/*               while (alku_osoite-osoite_laskuri)  // Tytetn vli nollilla Ei toimi kikissa tilanteissa!!!!!!!!!!!!!!
					{
						processor_->writeProgramMemory(osoite_laskuri,0x0);
						osoite_laskuri++;
					}
*/
					osoite_laskuri=alku_osoite;
					hx>>a;
					hx>>b;
					for(n=0;n<maara;n++)      // Luetaan HEX tiedostosta ja muunnetaan binääriseksi
					{
						hx>>a;
						hx>>b;
						arvo=aschex(a,b);
						processor_->writeProgramMemory(osoite_laskuri,arvo);
						programLoaded->push_back(std::tuple<unsigned int, unsigned char>(osoite_laskuri, arvo));
						osoite_laskuri++;
					}
				}
			}
			hx.close();
			emit message_SetStatusBar(processorIndex_, i18n("Creating..."), 20000);

			insertItems();

			//Tyhjätään muistialueet aina ohjelman latauksen yhteydessä
			clearInternalRamAreas();
			//clearExternalAreas();
			apustr=i18n("Program loaded: ");
			apustr+=captionIndexText_;//this->caption();
			apustr+=": ";
			apustr+=activeHexFilename_;
			emit message_SetStatusBar(processorIndex_, QString(apustr), 20000);
			setCaptionAndIconToProgramMemoryWs(whiteLedIcon_);
			updateProcessorWindows();
			readyToRun_=true;
		}
		else
		{
			apustr=i18n("Could not open file: ");
			apustr+=activeHexFilename_;
			apustr+="!";
			emit message_SetStatusBar(processorIndex_, apustr, 20000);
			return;
		}
		//
	}
	else
	{
		emit message_SetStatusBar(processorIndex_, i18n("No [.hex] or [.ihx] File Found!"), 20000);
		return;
	}

}

void I51BaseW::loadProgramAndCreateWindows(const QString& hexFilename)
{
	if(!processor_->readPinValue(pinEA_Index_))
		loadProgramAndCreateAll(hexFilename, &externalLoadedProgram_);
	else
		emit message_SetStatusBar(processorIndex_, i18n("Could not load to internal ROM!"), 20000);
}

void I51BaseW::copy(I51BaseW* sourceProcessorW)
{
	bool eaP=sourceProcessorW->processor_->readPinValue(pinEA_Index_); //EA Pinni talteen
	//Ladataan ulkoisen muistialueen komponentit ensin progressin toiminan järkeistämiseksi.
	QWidgetList externalAreaComponentList = sourceProcessorW->readExternalAreaComponents();
	if(externalAreaComponentList.count())
	{
		//Jos on komponentteja on myös apukytkentäpinnit. todennäköisesti.
		addExternalAreaNets(sourceProcessorW->readExternalAreaNets());
		//Luodaan komponentit
		addExternalAreaComponents(externalAreaComponentList);
	}
	//Ladataan sama ohjelma jos on.
	//External
	QString qStrApu;
	if(sourceProcessorW->isReadyToRun())
	{
		sourceProcessorW->setEAPin(false);
		qStrApu=sourceProcessorW->readEAPinPointingProgramMemoryFilename();
		if(qStrApu!=QString::null)
		{
			setEAPin(false);
			loadProgramAndCreateWindows(qStrApu);
		}
	}

	copy_VirtualPart(sourceProcessorW);


	//Ikkunoiden geometriat samanlaisiksi kuin kopioitavassa.
	setGeometry(sourceProcessorW->frameGeometry());
	QWidgetList sProWs = sourceProcessorW->readWindowList();
	for(int i = 0; i<int(sProWs.count()); i++)
	{
		QWidget *spw = sProWs.at(i);
		QString sname=spw->name();
		QPoint sp=spw->parentWidget()->pos();
		QRect sr=spw->frameGeometry();
		unsigned int sWs=spw->windowState();

		QWidgetList dProWs = this->readWindowList();
		for(int i = 0; i<int(dProWs.count()); i++)
		{
			QWidget *dpw = dProWs.at(i);
			QString dname=dpw->name();
			if(sname==dname)
			{
				dpw->setWindowState(sWs);
				dpw->setGeometry(sr);
				dpw->parentWidget()->move(sp);
			}
		}
	}
	//EA Pinnin tila sourceeen ja tähän (copy) prosessoriin samaksi.
	sourceProcessorW->setEAPin(eaP);
	setEAPin(eaP);
	setCaptionAndIconToProgramMemoryWs(whiteLedIcon_);
	updateProcessorWindows();
}

QString I51BaseW::readEAPinPointingProgramMemoryFilename(void) const
{
	if(!processor_->readPinValue(pinEA_Index_))
		return externalProgramMemoryHexFilename_;
	return QString::null;
}

void I51BaseW::setEAPin(bool value)
{
	processor_->writeInputPin(PINNAME_EA, value);
}

void I51BaseW::print( QPrinter* printer)
{
}

void I51BaseW::setCaptionAndIconToProgramMemoryWs(KPixmap const &icon)
{
	setPlainCaption(captionIndexText_+externalProgramMemoryHexFilename_);
	if(external_disassemblew_)
		external_disassemblew_->setIcon(icon);
	if(external_assemblersourcew_)
		external_assemblersourcew_->setIcon(icon);
	if(external_romw_)
		external_romw_->setIcon(icon);
}

void I51BaseW::resizeEvent( QResizeEvent *e )
{
	QSize oldSize=e->oldSize();
	QSize newSize=e->size();
	if((oldSize.height()) != (newSize.height()))
	{
	}
}

void I51BaseW::messageFromWorkspaceW_SetExecutionPoint(const QString& name, const unsigned long address)
{
	processor_->setProgramCounterValue(address);
	
	if(name==WINDOWNAME_EXTERNAL_ASM_SOURCE)
	{
		if(external_disassemblew_)
			external_disassemblew_->clearExecutionPoint();
	}
	if(name==WINDOWNAME_EXTERNAL_DISASSEMBLE)
	{
		if(external_assemblersourcew_)
			external_assemblersourcew_->clearExecutionPoint();
	}

	messageFromWorkspaceW_SetExecutionPoint_VirtualPart(name, address);
	
	updateProcessorWindows();
}

void I51BaseW::messageFromWorkspaceW_MemoryValueChanged(const QString& name, const unsigned long address, const int value)
{
	if((name==WINDOWNAME_REGISTERBANK_AREA)||(name==WINDOWNAME_BITADDRESSABLE_AREA)||(name==WINDOWNAME_SCRATCHPAD_AREA)||(name==WINDOWNAME_REGS))
		processor_->writeInternalDataMemory(address, value);
	if(name==WINDOWNAME_SFR_BIT)
	{
		int n=processor_->readInternalDataMemory(address);
		int m=value;
		n^=m;
		processor_->writeInternalDataMemory(address, n);
	}

	messageFromWorkspaceW_MemoryValueChanged_VirtualPart(name, address, value);
	
	updateProcessorWindows();
}

void I51BaseW::messageFromWorkspaceW_ValueChanged(const QString& name, const unsigned long index, const long value)
{
	if(name==WINDOWNAME_MISC)
		if(index==miscWClockSpeedIndex)
			processor_->changeClockSpeed(value);
	
	messageFromWorkspaceW_ValueChanged_VirtualPart(name, index, value);
	
	updateProcessorWindows();
}

void I51BaseW::messageFromWorkspaceW_ToggleBit(const QString& name, const QString& messageString, const int messageValue)
{

	if(name==WINDOWNAME_EXTERNAL_AREA)
	{
		if(messageValue==0)
		{
			processor_->toggleInputPin(messageString);
			setCaptionAndIconToProgramMemoryWs(whiteLedIcon_);
		}
		else
			processor_->togglePortPin((messageValue>>8),messageValue&0x0F);
	}
	//if(name==WINDOWNAME_EXTERNALSOURCE)
	if(name.contains("ASMSource"))
	{
		if(messageString[1]=='P')
			processor_->togglePortPin((messageValue>>8),messageValue&0x0F);
		else
		{
			unsigned char address=messageValue>>8;
			int m=processor_->readInternalDataMemory(address);
			int n=1<<(messageValue&0x0F);
			n=m^n;
			processor_->writeInternalDataMemory(address, n);
		}
	}

	messageFromWorkspaceW_ToggleBit_VirtualPart(name, messageString, messageValue);
	
	updateProcessorWindows();
}

void I51BaseW::messageFromWorkspaceW_ToggleBreakpointBit(const QString& name, const int messageValue)
{
	int address=messageValue>>8;
	int value=processor_->readInternalDataMemory(address);
	processor_->writeInternalDataMemory(address, value);//Kirjotetaan takaisin jotta päivittyy
	int bitMask=1<<(messageValue&0x0F);

	if(name==WINDOWNAME_SFR_BIT)
		sfrbitw_->toggleBitChangeBreakpoint(messageValue, bool(value&bitMask));
	if((name==WINDOWNAME_EXTERNAL_ASM_SOURCE)&&external_assemblersourcew_)
		external_assemblersourcew_->toggleBitChangeBreakpoint(messageValue, bool(value&bitMask));

	messageFromWorkspaceW_ToggleBreakpointBit_VirtualPart(name, messageValue, bool(value&bitMask));
	
	updateProcessorWindows();
}

void I51BaseW::messageFromWorkspaceW_Update(const QString& name)
{
	updateProcessorWindows();
}

void I51BaseW::messageFromWorkspaceW_FocusProgramMemoryAddress(const QString& name, const unsigned long address)
{
	if(!processor_->readPinValue(pinEA_Index_))
	{
		if(external_assemblersourcew_)
			external_assemblersourcew_->focusLine(address);
		if(external_disassemblew_)
			external_disassemblew_->focusLine(address);
		if(external_romw_)
			external_romw_->focusAddress(address);
	}
	
	messageFromWorkspaceW_FocusProgramMemoryAddress_VirtualPart(name, address);
}

void I51BaseW::messageFromWorkspaceW_WindowLevel(const QString& name, const int level)
{
	emit message_SetWindowLevel(processorIndex_, level);
	updateProcessorWindows();
}


void I51BaseW::childWindowActivated(void)
{
	emit message_SetWindowLevel(processorIndex_, KSimu51_NameS::WINDOWLEVEL_PROCESSOR);
}


void I51BaseW::centerActiveWindowInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->centerActiveWindow();
}

void I51BaseW::cornerActiveWindowInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->cornerActiveWindow();
}

void I51BaseW::nextWindowToShowInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->nextWindowToShow();
}

void I51BaseW::backPreviousWindowStateInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->backPreviousWindowState();
}

void I51BaseW::activeWindowNearToWindowInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->activeWindowNearToWindow();
}

void I51BaseW::activeWindowResizeHiddenOffInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->activeWindowResizeHiddenOff();
}

void I51BaseW::markActiveWindowInExternalArea(void)
{
	if(externalw_->readWindowsCounterValue())
		externalw_->markActiveWindow();
}

void I51BaseW::unmarkWindowsInExternalArea(void)
{
	if(externalw_&&readyToRun_)
		if(externalw_->readWindowsCounterValue())
			externalw_->unmarkWindows();
}

void I51BaseW::addPopupMenu(void)
{
	workspaceMenu_=new KPopupMenu(this);
	workspaceMenu_->insertItem(i18n("&Windows"), windowsMenu_); //windowsMenu_ kantaluokasta

}


void I51BaseW::mousePressEvent(QMouseEvent *e)
{
	QPoint mousePosition=e->globalPos();
	if((e->button() == QMouseEvent::RightButton))
	{
		workspaceMenu_->popup(mousePosition);
	}
	WorkspaceMainW::mousePressEvent(e);
}

void I51BaseW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//Talletetaan
	bool curProcessorEAPinValue=processor_->readPinValue(pinEA_Index_);

	setEAPin(false);
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QSTRING<<readEAPinPointingProgramMemoryFilename(); //Ulkoisen ohjelmamuistin ohjelma QString

	saveWorkDataToFile_VirtualPart(saveFileStream); //Perivät luokat

	setEAPin(curProcessorEAPinValue);

	//Prosessorin sisäinen tila
	processor_->saveWorkDataToFile(saveFileStream);

	//Prosessori ikkunan tila, eli tämä luokka
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QPOINT<<parentWidget()->pos();
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QRECT<<frameGeometry();
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT16<<(Q_UINT16)windowState();
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

	//prosessorin ikkunoiden geometriat ja datat.
	QWidgetList proWs = windowWorkspace_->windowList();
	for(int i = 0; i<int(proWs.count()); i++)
	{
		QWidget *pw = proWs.at(i);
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QPOINT<<pw->parentWidget()->pos();
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QRECT<<pw->frameGeometry();
		saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT16<<(Q_UINT16)pw->windowState();
		saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QSTRING<<pw->name();
		saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

		if(dynamic_cast<ShareMainW*>(pw))
			static_cast<ShareMainW*>(pw)->saveWorkDataToFile(saveFileStream);
	}

	//Tämän luokan talletettavat datat
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)resetActiveLastValue_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)previousStepSerialTransmitFlag_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::DOUBLE<<(double)wholeStepTimeCounter_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::DOUBLE<<(double)wholeStepTimeCounterInSync_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void I51BaseW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	loadingWorkFile_=true;

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	bool eaP=processor_->readPinValue(pinEA_Index_);
	QString extFNQStr;
	readFileStream>>indexReadData>>typeReadData>>extFNQStr;
	if(extFNQStr!=QString::null) //Ulkoisen ohjelmamuistin ohjelma, jos on
	{
		setEAPin(false);
		loadProgramAndCreateWindows(extFNQStr);
	}

	readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);

	setEAPin(eaP);

	//Prosessorin sisäinen tila
	processor_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	//Saattanu ohjelmamuisti muuttua jos FLASCHIA edellisessä, ni talletetaan ohjelmaosa uuvestaa.
	rewriteProgramMemory();

	//Prosessoriikkunan tila, eli tämä luokka
	QPoint pos;
	QRect rect;
	Q_UINT16 wState;
	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>pos;
				parentWidget()->move(pos);
				break;
			case 1:
				readFileStream>>typeReadData>>rect;
				setGeometry(rect);
				break;
			case 2: //Ikkunan tila
				readFileStream>>typeReadData>>wState;
				setWindowState(wState);
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);

	//Prosessorin ikkunoiden geometriat ja datat
	QWidgetList proWs = windowWorkspace_->windowList();
	for(int i = 0; i<int(proWs.count()); i++)
	{
		QWidget *pw = proWs.at(i);
		QString name;
		bool done=false;
		do
		{
			readFileStream>>indexReadData;
			switch(indexReadData)
			{
				case 0:
					readFileStream>>typeReadData>>pos;
					pw->parentWidget()->move(pos);
					break;
				case 1:
					readFileStream>>typeReadData>>rect;
					pw->setGeometry(rect);
					break;
				case 2:
					readFileStream>>typeReadData>>wState;
					pw->setWindowState(wState);
					break;
				case 3:
					readFileStream>>typeReadData>>name;
					pw->name();
					break;
				case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
					done=true;
					break;
				default:
					done = !funcHandleUnknownIndexRead(readFileStream);
			}
		}
		while(!done);

		if(dynamic_cast<ShareMainW*>(pw))
			static_cast<ShareMainW*>(pw)->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	}

	//Tämän luokan datat
	Q_UINT8 value8;
	double valueDouble;
	done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8;
				resetActiveLastValue_=(bool)value8;
				break;
			case 1:
				readFileStream>>typeReadData>>value8;
				previousStepSerialTransmitFlag_=(bool)value8;
				break;
			case 2: //Ikkunan tila
				readFileStream>>typeReadData>>valueDouble;
				wholeStepTimeCounter_=(double)valueDouble;
				break;
			case 3:
				readFileStream>>typeReadData>>valueDouble;
				wholeStepTimeCounterInSync_=(double)valueDouble;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);

	loadingWorkFile_=false;
	setCaptionAndIconToProgramMemoryWs(whiteLedIcon_);
	updateProcessorWindows();
}


void I51BaseW::rewriteProgramMemory(void)
{
	bool eaPinCurrValue = readEAPin();
	setEAPin(false);
	for( auto memoryData : externalLoadedProgram_)
		processor_->writeProgramMemory(std::get<0>(memoryData), std::get<1>(memoryData));
	setEAPin(eaPinCurrValue);
}


//Uudeleenmääritelty, jotta ohittaa shareMainWindow vastaavan.
void I51BaseW::closeEvent( QCloseEvent *e )
{
	delete processor_;
	processor_=nullptr;
	emit message_Close(processorIndex_);
	KMainWindow::closeEvent(e);
}

bool I51BaseW::event(QEvent *e)
{
	return KMainWindow::event(e); //Potkastaan eventti pääkantaluokkaan ohi shareMainWindowin
}

bool I51BaseW::activateWindowFromGlobalPointValue(QPoint* pos)
{
	bool retVal=false;
	retVal=activateWorkspaceWindowHittingTileBarFromActiveWindow(pos);
	retVal=externalw_->activateWorkspaceWindowHittingTileBarFromActiveWindow(pos);
	return retVal;
}
