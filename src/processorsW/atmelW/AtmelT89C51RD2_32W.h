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
#ifndef ATMELT89C51RD2W_H
#define ATMELT89C51RD2W_H

#include "../I51BaseW.h"
#include "../I51BaseInternalIRAMW.h"
#include "../I51BaseInternalXRAMW.h"
#include "../I51BaseInternalFLASHW.h"
#include "../I51BaseTimer2W.h"


#include "../../processors/Atmel/T89C51RD2_32.h"
class T89C51RD2_32;

class AtmelT89C51RD2_32W :  virtual public I51BaseW,  public I51BaseInternalFLASHW, public I51BaseInternalIRAMW, public I51BaseInternalXRAMW, public I51BaseTimer2W
{
	Q_OBJECT
	public:
		AtmelT89C51RD2_32W(QWidget* parent, char const * name, int wflags, int const  index, int const  type);
		~AtmelT89C51RD2_32W();

		void showPointerR0(void) override;
		void showPointerR1(void) override;

	protected:
		void createStaticWindows_VirtualPart(void) override;
		void clearInternalRamAreas(void) override;

		void updateProcessorWindows_VirtualPart(void) override;
		void messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value) override;


		void saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream) override;
		void readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
										  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

};

#endif
