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
#include "AtmelT89C51RD2_32W.h"


AtmelT89C51RD2_32W::AtmelT89C51RD2_32W(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type), I51BaseInternalFLASHW(parent, name, wflags, index, type), I51BaseInternalIRAMW(parent, name, wflags, index, type), I51BaseInternalXRAMW(parent, name, wflags, index, type), I51BaseTimer2W(parent, name, wflags, index, type)
{

	delete processor_; //Deletoidaan kantaluokassa luotu prosessori
	processor_=new T89C51RD2_32; //Luodaan tämän oma prosessori
}

AtmelT89C51RD2_32W::~AtmelT89C51RD2_32W()
{
}

void AtmelT89C51RD2_32W::showPointerR0(void)
{
	I51BaseInternalIRAMW::showPointerR0();
	I51BaseInternalXRAMW::showPointerR0();
}

void AtmelT89C51RD2_32W::showPointerR1(void)
{
	I51BaseInternalIRAMW::showPointerR1();
	I51BaseInternalXRAMW::showPointerR1();
}

void AtmelT89C51RD2_32W::updateProcessorWindows_VirtualPart(void)
{
	I51BaseInternalFLASHW::updateProcessorWindows_VirtualPart();
	I51BaseInternalIRAMW::updateProcessorWindows_VirtualPart();
	I51BaseInternalXRAMW::updateProcessorWindows_VirtualPart();
	I51BaseTimer2W::updateProcessorWindows_VirtualPart();
}

void AtmelT89C51RD2_32W::createStaticWindows_VirtualPart(void)
{
	I51BaseInternalIRAMW::createStaticWindows_VirtualPart();
	I51BaseInternalXRAMW::createStaticWindows_VirtualPart();
	I51BaseTimer2W::createStaticWindows_VirtualPart();
}

void AtmelT89C51RD2_32W::clearInternalRamAreas(void)
{
	I51BaseInternalIRAMW::clearInternalRamAreas();
	I51BaseInternalXRAMW::clearInternalRamAreas();
}

void AtmelT89C51RD2_32W::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value)
{
	I51BaseInternalIRAMW::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(name, address, value);
	I51BaseInternalXRAMW::messageFromWorkspaceW_MemoryValueChanged_VirtualPart(name, address, value);
}

void AtmelT89C51RD2_32W::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	I51BaseInternalFLASHW::saveWorkDataToFile_VirtualPart(saveFileStream);
	I51BaseInternalIRAMW::saveWorkDataToFile_VirtualPart(saveFileStream);
	I51BaseInternalXRAMW::saveWorkDataToFile_VirtualPart(saveFileStream);
}

void AtmelT89C51RD2_32W::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
													  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	I51BaseInternalFLASHW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);
	I51BaseInternalIRAMW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);
	I51BaseInternalXRAMW::readWorkDataFromFile_VirtualPart(readFileStream, funcHandleUnknownIndexRead);
}
