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
#include "I51BaseInternalFLASHW.h"
#include "../processors/I8051_Flash.h"

I51BaseInternalFLASHW::I51BaseInternalFLASHW(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type), I51BaseInternalROMW(parent, name, wflags, index, type)
{
   flashWNotCreated_=true;
}

I51BaseInternalFLASHW::~I51BaseInternalFLASHW()
{
}
   
void I51BaseInternalFLASHW::updateProgramMemoryWindows(void)
{
   unsigned int minA,maxA; //Osoite saadaan aliaksilta
   I8051_Flash* p = dynamic_cast<I8051_Flash*>(processor_);
   if(p->isFlashMemoryValueChanged(minA, maxA))
   {
      for(unsigned int address=minA;address<=maxA;address++)
      {
         if(p->isFlashMemoryValueChanged(address))
         {
            unsigned char value=p->readInternalProgramMemoryValue(address);
            internal_romw_->writeValue(address,value);
         }
      }
   }
   
   I51BaseInternalROMW::updateProgramMemoryWindows();
}

void I51BaseInternalFLASHW::createDynamicWindows_VirtualPart(void)
{
   I8051_Flash* p = dynamic_cast<I8051_Flash*>(processor_);
   unsigned int maxInternalROM=p->readInternalProgramMemorySize();
   if(flashWNotCreated_)
   {
      //ROM olemassa
      if(internal_romw_)
      {
         delete internal_romw_; internal_romw_=0;
      }
      internal_romw_=new RomW(windowWorkspace_, WINDOWNAME_INTERNAL_FLASH, WDestructiveClose|WMouseNoMask,0,maxInternalROM);
      internal_romw_->setPlainCaption(i18n("INTERNAL FLASH"));
      internal_romw_->setGeometry(0, 0, internal_romw_->romTableGeometrySize().width()+20, height()/2);
      flashWNotCreated_=false;
   }
/*   unsigned int wState=iromw->windowState();
   iromw->showMinimized();
   
   unsigned long address;
   for(address=0;address<maxInternalROM;address++)
   {
      int value;
      value=processor->readInternalProgramMemoryValue(address);
      iromw->writeValue(address,value);
   }
   if(wState!=Qt::WindowMinimized)
      iromw->showNormal();
   */   
   I51BaseInternalROMW::createDynamicWindows_VirtualPart();
}


void I51BaseInternalFLASHW::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	I51BaseInternalROMW::saveWorkDataToFile_VirtualPart(saveFileStream);

	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}


void I51BaseInternalFLASHW::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
													  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	I51BaseInternalROMW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);

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
