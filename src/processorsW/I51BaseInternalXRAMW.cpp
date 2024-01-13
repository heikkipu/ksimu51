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
#include "I51BaseInternalXRAMW.h"
#include "../processors/I8032.h"
#include "../processors/I8031_Xram.h"

I51BaseInternalXRAMW::I51BaseInternalXRAMW(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type)
{
	internalxramw_=0;
	showThisR0_=false;
	showThisR1_=false;
	showThisDPTR_=false;
}

I51BaseInternalXRAMW::~I51BaseInternalXRAMW()
{
}

void I51BaseInternalXRAMW::updateProcessorWindows_VirtualPart(void)
{
	if(!firstProgramLoad_)
	{
		unsigned int address;
		unsigned int minA,maxA; //Osoite saadaan aliaksilta
		I8031_Xram* p=dynamic_cast<I8031_Xram*>(processor_);
		if(p->isInternalXRAMValueChanged(minA, maxA))
		{
			for(address=minA;address<=maxA;address++)
			{
				if(p->isInternalXRAMValueChanged(address))
				{
					unsigned char value=p->readInternalXRAMValue(address);
					internalxramw_->setValue(address,value);
				}
			}
		}
		internalxramw_->setPointers(processor_->readDPTRValue(), r0Value(), r1Value());
	}
}

void I51BaseInternalXRAMW::createStaticWindows_VirtualPart(void)
{
	//Internal XRAM
	I8031_Xram* p=dynamic_cast<I8031_Xram*>(processor_);
	unsigned int startAddress;
	unsigned long size=p->readInternalXRAMSize(startAddress);
	internalxramw_ = new InternalXRAMW(windowWorkspace_, WINDOWNAME_INTERNALXRAM, WDestructiveClose|WMouseNoMask, startAddress, size, true);
	internalxramw_->setPlainCaption(i18n("INTERNAL XRAM"));
	internalxramw_->show();

}

void I51BaseInternalXRAMW::clearInternalRamAreas(void)
{
	I51BaseW::clearInternalRamAreas();

	//XRAM
	internalxramw_->clear();
}

void I51BaseInternalXRAMW::showPointerDPTR(void)
{
	if(showThisDPTR_)
	{
		internalxramw_->showPointerSP();   //Käyttää SP:n pointteria koska sitä ei tarvita tässä
		centerActiveWindow();
		showThisDPTR_=false;
	}
	else
	{
		I51BaseW::showPointerDPTR();
		showThisDPTR_=true;
	}

}

void I51BaseInternalXRAMW::showPointerR0(void)
{
	if(showThisR0_)
	{
		internalxramw_->showPointerR0();
		showThisR0_=false;
		centerActiveWindow();
	}
	else
	{
		I51BaseW::showPointerR0();
		showThisR0_=true;
	}

}

void I51BaseInternalXRAMW::showPointerR1(void)
{

	if(showThisR1_)
	{
		internalxramw_->showPointerR1();
		centerActiveWindow();
		showThisR1_=false;
	}
	else
	{
		I51BaseW::showPointerR1();
		showThisR1_=true;
	}
}

void I51BaseInternalXRAMW::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value)
{
	if(name==WINDOWNAME_INTERNALXRAM)
	{
		I8031_Xram* p=dynamic_cast<I8031_Xram*>(processor_);
		p->writeInternalXRAMValue(address, value);
	}
}


void I51BaseInternalXRAMW::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}


void I51BaseInternalXRAMW::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
															std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			//case X:
			//	readFileStream>>typeReadData>>Data_xxx_;
			//	break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}
