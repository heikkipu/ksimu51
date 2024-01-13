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
#ifndef LEDINDICATORGRID_H
#define LEDINDICATORGRID_H

#include <kaboutdata.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>

#include <kled.h>

class LedIndicatorGrid : public QWidget
{
	public:
		LedIndicatorGrid(int const  margin, int const  spacing, QWidget * parent, char const  * name, WFlags f);
		~LedIndicatorGrid();

		//Lisätään ledi ja paluttaa ledin indeksin
		int addLed(int const  row, int const  col, const QString& text, const QColor& color);
		void ledON(int index);
		void ledOFF(int index);

		void hideLed(int index);
		void showLed(int index);

	private:
		int indexCounter_;

		QGridLayout* ledIndicatorQGridLayout_;
		KLed* ledKLed_;
		QLabel* ledQLabel_;
		QPtrList<KLed>ledQLabelList_;
		QPtrList<QLabel>ledTextQLabelList_;
};

#endif
