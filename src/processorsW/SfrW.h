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
#ifndef SFRW_H
#define SFRW_H

#include "../processors/I8031.h"
#include "../share/ShareMainW.h"
#include <qtable.h>

class QTable;

class SfrW: public ShareMainW
{
	Q_OBJECT
	public:
		SfrW( QWidget* parent, char const * name, int wflags );
		~SfrW();

		//Arvo SFR taulukkoon, jos SFR paikka käytössä.
		void setValue(unsigned const int  address, unsigned char const  value);
		void setRegisterName(unsigned const int  address, const QString & rName);
		
		QSize sfrTableGeometrySize(void);
		QSize getMaximumSize(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	private:
		QTable* sfrTable_;
};

#endif
