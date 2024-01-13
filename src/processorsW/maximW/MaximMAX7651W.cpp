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
#include "MaximMAX7651W.h"


MaximMAX7651W::MaximMAX7651W(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type), I51BaseInternalIRAMW(parent, name, wflags, index, type), I51BaseADConverterW(parent, name, wflags, index, type), I51BaseInternalFLASHW(parent, name, wflags, index, type), I51BaseTimer2W(parent, name, wflags, index, type)
{

	delete processor_; //Deletoidaan kantaluokassa luotu prosessori
	processor_=new MAX7651; //Luodaan tämän oma prosessori
}

MaximMAX7651W::~MaximMAX7651W()
{
}

void MaximMAX7651W::updateProcessorWindows_VirtualPart(void)
{
	I51BaseInternalFLASHW::updateProcessorWindows_VirtualPart();
	I51BaseInternalIRAMW::updateProcessorWindows_VirtualPart();
	I51BaseTimer2W::updateProcessorWindows_VirtualPart();
}

void MaximMAX7651W::createStaticWindows_VirtualPart(void)
{
	I51BaseInternalFLASHW::createStaticWindows_VirtualPart();
	I51BaseInternalIRAMW::createStaticWindows_VirtualPart();
	I51BaseTimer2W::createStaticWindows_VirtualPart();
}

void MaximMAX7651W::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value)
{
	I51BaseADConverterW::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(name, address, value);
	I51BaseInternalIRAMW::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(name, address, value);
}

void MaximMAX7651W::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	I51BaseADConverterW::saveWorkDataToFile_VirtualPart(saveFileStream);
	I51BaseInternalFLASHW::saveWorkDataToFile_VirtualPart(saveFileStream);
	I51BaseInternalIRAMW::saveWorkDataToFile_VirtualPart(saveFileStream);
}

void MaximMAX7651W::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
												 std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	I51BaseADConverterW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);
	I51BaseInternalFLASHW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);
	I51BaseInternalIRAMW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);
}
