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
#ifndef CHARACTERDOTMATRIXLCD_H
#define CHARACTERDOTMATRIXLCD_H


#include <kaboutdata.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qimage.h>

class QLabel;

class CharacterDotMatrixLCD : public QWidget
{
	Q_OBJECT
	public:
		CharacterDotMatrixLCD(const QSize displaySize, const QSize charMatrixSize, const QSize charMatrixDotsSize, int const  charSpace, QString* charSet, QWidget* parent, char const * name, WFlags f);
		~CharacterDotMatrixLCD();

		//Laitetaan merkki
		void setChar(int const  row, int const  col, char const  c);

		void setDisplayONOFF(bool const  onOff);

		void setCursor(int const  row, int const  col);
		void clearCursor(void);
		void setWholeMatrixCursor(bool const  wholeMatrix);


		void recreate(const QSize displaySize, const QSize charMatrixSize, const QSize charMatrixDotsSize, int const  charSpace, QString* charSet);
		void destroyThis(bool destroyWindow, bool destroySubWindows){ destroy(destroyWindow, destroySubWindows);};

	protected:

	private:

		QSize dispSize_;   //Näytön koko (columns, rows)
		QSize charMSize_;   //Character matrixin koko (width, height) pixels
		QSize charMDotsSize_;   //matriisin pistennkoko (width, height) pixels

		QString* pointerToCharacterSet_;

		QGridLayout* matrixQGridLayout_;
		QLabel* matrixQLabel_;
		QPtrList<QLabel>matrixQLabelList_;

		QImage matrixQImage_;
		QImage pointerQImage32_;
		QImage cursorSaveQImage_;

		bool displayON_;

		bool wholeMatrixCursor_;
		bool cursorIsON_;
		int cursorCol_;
		int cursorRow_;

		void createMatrix(const QSize& charMatrixSize, const QSize& charMatrixDotsSize);
};



#endif
