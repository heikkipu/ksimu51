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
#ifndef INDEXEDKDOUBLENUMINPUT_H
#define INDEXEDKDOUBLENUMINPUT_H

#include <knuminput.h>
#include <qstring.h>

//Määritellää KDoubleNumInput uudelleen, jotta tiedetään minkä analogiainputin tila muuttui.
class IndexedKDoubleNumInput:public KDoubleNumInput
{
	Q_OBJECT
	public:
		IndexedKDoubleNumInput(int id, double lower, double upper, double value, double step, int precision, QWidget *parent, char const  *name);
		~IndexedKDoubleNumInput();
	protected:

	signals:
		void valueIsChanged(int, QString&, double);

	private slots:
		void slotValueIsChanged(double);

	private:
		QString pname_;
		int index_;
};

#endif
