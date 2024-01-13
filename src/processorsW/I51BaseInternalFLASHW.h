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
#ifndef I51BASEINTERNALFLASHW_H
#define I51BASEINTERNALFLASHW_H

#include "I51BaseW.h"
#include "I51BaseInternalROMW.h"

class I51BaseW;
class I51BaseInternalROMW;

class I51BaseInternalFLASHW : virtual public I51BaseW, public I51BaseInternalROMW
{
	Q_OBJECT
	public:
		I51BaseInternalFLASHW(QWidget* parent, char const * name, int wflags, int const  index, int const  type);
		~I51BaseInternalFLASHW();

	protected:
		void updateProgramMemoryWindows(void) override;
		void createDynamicWindows_VirtualPart(void) override;

		void saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream) override;
		void readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
													  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;
	private:
		bool flashWNotCreated_;

		#define WINDOWNAME_INTERNAL_FLASH "internalFLASHW"
};

#endif
