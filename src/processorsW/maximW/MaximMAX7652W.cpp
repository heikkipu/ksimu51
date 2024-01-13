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
#include "MaximMAX7652W.h"


MaximMAX7652W::MaximMAX7652W(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type), MaximMAX7651W(parent, name, wflags, index, type)
{

	delete processor_; //Deletoidaan kantaluokassa luotu prosessori
	processor_=new MAX7652; //Luodaan tämän oma prosessori
}

MaximMAX7652W::~MaximMAX7652W()
{
}


void MaximMAX7652W::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	MaximMAX7651W::saveWorkDataToFile_VirtualPart(saveFileStream);

	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}


void MaximMAX7652W::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
													  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	MaximMAX7651W::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);

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
