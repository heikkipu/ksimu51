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
#ifndef BITADDRESSABLEAREAW_H
#define BITADDRESSABLEAREAW_H

#include "../processors/I8031.h"
#include "../share/ShareMainW.h"
#include "../share/NumQTable.h"
#include <kmainwindow.h>
#include <kpixmap.h>
#include <string>

#include <qtable.h>

class QTable;

class BitAddressableAreaW :  public ShareMainW
{
	Q_OBJECT
	public:
		BitAddressableAreaW( QWidget* parent, char const * name, int wflags );
		~BitAddressableAreaW();

		void clear(void);
		void setValue(unsigned int address, unsigned char value);

		void setPointers(unsigned const int  spVal, unsigned char const  r0Val, unsigned char const  r1Val);
		void showPointerSP(void);
		void showPointerR0(void);
		void showPointerR1(void);

		QSize bitAddressableTableGeometrySize(void);
		QSize getMaximumSize(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	signals:
		void message_MemoryValueChanged(const QString&, const unsigned long, const int );

	private slots:
		void bitAddressableTableValueChanged(int row, int col);

	private:
		NumQTable* bitAddressableTable_;

		bool isSP_;
		bool isR0_;
		bool isR1_;
		int spRow_;
		int spColumn_;
		int r0Row_;
		int r0Column_;
		int r1Row_;
		int r1Column_;
		unsigned char spValue_;
		unsigned char r0Value_;
		unsigned char r1Value_;
		void writePointerValues(unsigned const int  spV, unsigned char const  r0V, unsigned char const  r1V);
		void clearPointers(void);
		void showPointers(void);

		KPixmap SPMark_;
		KPixmap R0Mark_;
		KPixmap R1Mark_;
		KPixmap SPR0Mark_;
		KPixmap SPR1Mark_;
		KPixmap R0R1Mark_;
		KPixmap SPR0R1Mark_;

};

#endif
