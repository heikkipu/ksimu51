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
#ifndef MISCW_H
#define MISCW_H

#include <qtable.h>
#include "../share/ShareMainW.h"
#include "../share/MyQTableItem.h"

class QTable;

class MiscW : public ShareMainW
{
	Q_OBJECT
	public:
		MiscW( QWidget* parent, char const * name, int wflags );
		~MiscW();

		void remove(void);
		static bool const EDITABLE=false;
		static bool const NOT_EDITABLE=true;
		int addRow(const QString& rowName, bool const editable); //Return value: row where added.
		void setValue(int const  row, unsigned long const  value); //Column 0 HEX. Column 1 DEC.
		void setValue(int const  row, double const  value); //Vain Column 1 desimaalilukune
		void setValue(int const  row, unsigned int const  value, int bitToShow); //Binäärisenä column 0: Column 1 unsigned char DEC.
		void setValue(int const  row, const QString& value); //Column 0 merkkijonona.

		QSize miscTableGeometrySize(void);
		QSize getMaximumSize(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	signals:
		void message_ValueChanged(const QString&, const unsigned long, const long );

	private slots:
		void miscTableValueChanged(int row, int col);
		void miscTableCurrentChanged(int row, int col);

	private:
		QString miscTableCurrentCellText_;
		QTable *miscTable_;
		MyQTableItem *pToMiscTableItem_;
};

#endif
