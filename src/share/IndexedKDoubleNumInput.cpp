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

//Uudelleen määritetty KDoubleNumInput-------------------------------------------------------------
#include "IndexedKDoubleNumInput.h"

IndexedKDoubleNumInput::IndexedKDoubleNumInput(int id, double lower, double upper, double value, double step, int precision, QWidget *parent, char const  *name) : KDoubleNumInput(lower, upper, value, step, precision, parent, name)
{
	QFont kDoubleNumInput_font(font());
	kDoubleNumInput_font.setFamily("MonoSpace");
	kDoubleNumInput_font.setPointSize(10);
	setFont(kDoubleNumInput_font);

	pname_=name;
	index_=id;
	//Ohjataan signaali takaisin.
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(slotValueIsChanged(double)));
}

IndexedKDoubleNumInput::~IndexedKDoubleNumInput()
{
}

void IndexedKDoubleNumInput::slotValueIsChanged(double dV)
{
	//Lisätään takaisin ohjattuun signaaliin rojuva.
	emit valueIsChanged(index_, pname_, dV);
}
