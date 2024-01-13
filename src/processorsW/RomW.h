/**************************************************************************
*   Copyright (C) 2005 - ... Heikki Pulkkinen                             *
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
#ifndef ROMW_H
#define ROMW_H

#include "../share/ShareMainW.h"

#include <functional>
#include <qtable.h>

class QTable;

class RomW : public ShareMainW
{
	Q_OBJECT
	public:
		RomW( QWidget* parent, char const * name, int wflags , unsigned const int  startAddress, unsigned long const  size);
		~RomW();

		void clear(void);
		void writeValue(unsigned long address, unsigned char value);
	//   void changeValue(unsigned long address, unsigned char value); //Sallitaan yksittäisen alkion muutos jos Flash
		void focusAddress(unsigned long const address);
		void setProgramCounterPointer(unsigned long const address);
		
		QSize romTableGeometrySize(void);
		QSize getMaximumSize(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	private:
		unsigned int tableRowCounter_;
		QTable* romTable_;
		KPixmap programCounterIcon_;
		unsigned long prevProgramCounterPointerAddress_;
};

#endif
