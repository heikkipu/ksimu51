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
#ifndef REGSW_H
#define REGSW_H

#include "../share/ShareMainW.h"
#include "../share/NumQTable.h"

#include <qtable.h>
#include <string>


class QTable;


class RegsW : public ShareMainW
{
	Q_OBJECT
	public:
		RegsW( QWidget* parent, char const * name, int wflags );
		~RegsW();

		void removeAllRows(void);
		void add(unsigned char const  registerAddress, const QString& registerName);
		void setValue(const int  row, unsigned char const  registerValue);

		QSize regsTableGeometrySize(void);
		QSize getMaximumSize(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	signals:
		void message_MemoryValueChanged(const QString&, const unsigned long, const int );

	private slots:
		void registerTableValueChanged(int row, int col);

	private:
		NumQTable* registerTable_;
};

#endif
