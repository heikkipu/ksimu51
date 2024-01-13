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
#include "I51BaseInternalIRAMW.h"
#include "../processors/I8032.h"


I51BaseInternalIRAMW::I51BaseInternalIRAMW(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type)
{
	internalindirectramw_=0;
}

I51BaseInternalIRAMW::~I51BaseInternalIRAMW()
{
}

void I51BaseInternalIRAMW::updateProcessorWindows_VirtualPart(void)
{
	if(!firstProgramLoad_)
	{
		unsigned int address;
		I8032* p = dynamic_cast<I8032*>(processor_);
		//Internal Indirect RAM
		for(address=I8032_NameS::MIN_INTERNAL_INDIRECT_RAM_ADDRESS;address<=I8032_NameS::MAX_INTERNAL_INDIRECT_RAM_ADDRESS;address++)
		{
			if(p->isInternalIndirectDataMemoryChanged(address))
			{
				internalindirectramw_->setValue(address, p->readInternalIndirectDataMemory(address));
			}
		}
		internalindirectramw_->setPointers(spValue(), r0Value(), r1Value());
	}
}

void I51BaseInternalIRAMW::createStaticWindows_VirtualPart(void)
{
	//Internal Indirect RAM
	internalindirectramw_=new InternalRAMW(windowWorkspace_, WINDOWNAME_INTERNALINDIRECTRAM, WDestructiveClose|WMouseNoMask, I8032_NameS::MIN_INTERNAL_INDIRECT_RAM_ADDRESS, I8032_NameS::MAX_INTERNAL_INDIRECT_RAM_ADDRESS-I8032_NameS::MIN_INTERNAL_INDIRECT_RAM_ADDRESS+1,false);
	internalindirectramw_->setPlainCaption(i18n("INTERNAL INDIRECT RAM"));
	internalindirectramw_->show();
}

void I51BaseInternalIRAMW::clearInternalRamAreas(void)
{
	I51BaseW::clearInternalRamAreas();

	//Internal Indirect RAM
	internalindirectramw_->clear();
}

void I51BaseInternalIRAMW::showPointerSP(void)
{
	I51BaseW::showPointerSP();

	internalindirectramw_->showPointerSP();
}

void I51BaseInternalIRAMW::showPointerR0(void)
{
	I51BaseW::showPointerR0();

	if(!internalR0_)
		internalindirectramw_->showPointerR0();
}

void I51BaseInternalIRAMW::showPointerR1(void)
{
	I51BaseW::showPointerR1();

	if(!internalR1_)
		internalindirectramw_->showPointerR1();
}

void I51BaseInternalIRAMW::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value)
{
	if(name==WINDOWNAME_INTERNALINDIRECTRAM)
	{
		I8032* p = dynamic_cast<I8032*>(processor_);
		p->writeInternalIndirectDataMemory(address, value);
	}
}


void I51BaseInternalIRAMW::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}


void I51BaseInternalIRAMW::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
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
