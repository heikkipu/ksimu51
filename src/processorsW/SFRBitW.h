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
#ifndef SFRBW_H
#define SFRBW_H

#include "../processors/I8031.h"
#include "../share/MyQTableItem.h"
#include "BitChangeBreakpoint.h"
#include "../share/ShareMainW.h"
#include <kpixmap.h>

#include <qtable.h>
#include <string>

class QTable;

class SFRBitW : public ShareMainW
{
	Q_OBJECT
	public:
		SFRBitW( QWidget* parent, char const * name, int wflags );
		~SFRBitW();

		void removeRegsRows(void);
		void insertRegsRow(unsigned char const  registerAddress, const QString& registerName, const QString& registerBitNames);
		void setRegsBitsValue(const int  row, unsigned char const  value);

		//bitAddress: Upper=Address, lower=Bitnumber
		bool toggleBitChangeBreakpoint(unsigned int const bitAddress, bool const currentValue);
		unsigned int isAtBitChangeBreakpoint(I8031* const processor);
		bool showBitChangeBreakpointBit(void);
		
		QSize sfrBitTableGeometrySize(void);
		QSize getMaximumSize(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	signals:
		void message_MemoryValueChanged(const QString&, const unsigned long, const int );
		void message_ToggleBreakpointBit(const QString&, const int );

	private slots:
		void sfrBitTableMouseDoubleClicked(int row, int col, int button, const QPoint& mousePos);
		void sfrBitTableMouseClicked(int row, int col, int button, const QPoint & mousePos);
		
	private:

		BitChangeBreakpoint* bitBreakpoint_;

		int convertRowToAddress(int const row);
		int convertColumnToBit(int column);
		
		KPixmap ledOnIcon_;
		KPixmap ledOffIcon_;
		KPixmap breakpointOnIcon_;
		KPixmap breakpointOffIcon_;

		QTable* sfrBitTable_;
		MyQTableItem *pToSfrBitTableItem_;
};

#endif
