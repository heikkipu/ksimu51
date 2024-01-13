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
#include "I51BaseTimer2W.h"

I51BaseTimer2W::I51BaseTimer2W(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type)
{
	miscWindowUpdateStartRow_=-1;
}

I51BaseTimer2W::~I51BaseTimer2W()
{
}

void I51BaseTimer2W::updateProcessorWindows_VirtualPart(void)
{
	int m=miscWindowUpdateStartRow_;
	if(m>=0)
	{
		miscw_->setValue(m++, (unsigned long)processor_->readTimerValue16(2));
		unsigned long rval=processor_->readInternalDataMemory(I8032_NameS::RCAP2H);
		rval<<=8;
		rval|=processor_->readInternalDataMemory(I8032_NameS::RCAP2L);
		miscw_->setValue(m++, rval);
	}
}

void I51BaseTimer2W::createStaticWindows_VirtualPart(void)
{
	miscw_->hide();
	miscWindowUpdateStartRow_=miscw_->addRow("Timer2", MiscW::NOT_EDITABLE);
	miscw_->addRow("RCAP2", MiscW::NOT_EDITABLE);
	miscw_->show();
}


void I51BaseTimer2W::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}


void I51BaseTimer2W::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
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
