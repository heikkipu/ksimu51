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
#ifndef INTERNALRAMW_H
#define INTERNALRAMW_H

#include <kpixmap.h>

#include <qtable.h>
#include <string>

#include "../share/ShareMainW.h"
#include "../share/MyQTableItem.h"

class QTable;

class InternalRAMW : public ShareMainW
{
	Q_OBJECT
	public:
		InternalRAMW(QWidget* parent, char const * name, int wflags, unsigned const int startAddress, unsigned const int size, bool const  charColumn);
		~InternalRAMW();

		void clear(void);
		void setValue(unsigned int address, unsigned char value);

		void setPointers(unsigned const int  pointerSPVal, unsigned char const  registerR0Val, unsigned char const registerR1Val);
		void showPointerSP(void);
		void showPointerR0(void);
		void showPointerR1(void);

		QSize internalRAMTableGeometrySize(void);
		QSize getMaximumSize(void) override;

	signals:
		void message_MemoryValueChanged(const QString&, const unsigned long, const int );

	protected:
		QTable* internalRAMTable_;

		KPixmap pointerSPMark_;
		KPixmap registerR0Mark_;
		KPixmap registerR1Mark_;
		KPixmap pointerSPRegisterR0Mark_;
		KPixmap pointerSPRegisterR1Mark_;
		KPixmap registerR0RegisterR1Mark_;
		KPixmap pointerSPRegisterR0RegisterR1Mark_;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	private slots:
		void internalRAMTableValueChanged(int row, int col);
		void internalRAMTableCurrentChanged(int row, int col);

	private:

		bool charColumnOn_;
		unsigned int iRAMStartAddress_;
		unsigned int iRAMSize_;
		MyQTableItem *pToInternalRAMTableItem_;
		QString internalRAMTableCurrentCellText_;

		bool isPointerSP_;
		bool isRegisterR0_;
		bool isRegisterR1_;
		int pointerSPRow_;
		int pointerSPColumn_;
		int registerR0Row_;
		int registerR0Column_;
		int registerR1Row_;
		int registerR1Column_;
		unsigned char pointerSPValue_;
		unsigned char registerR0Value_;
		unsigned char registerR1Value_;
		void writePointerValues(unsigned const int  pointerSPV, unsigned char const  registerR0V, unsigned char const  registerR1V);
		void clearPointers(void);
		void showPointers(void);

		void setText(int const row, int const column, const QString &text);
		void setPixmap(int const row, int const column, const QPixmap &pix);
};

#endif
