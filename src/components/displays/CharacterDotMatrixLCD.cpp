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
#include "CharacterDotMatrixLCD.h"

#include <qlayout.h>
#include <qptrlist.h>

CharacterDotMatrixLCD::CharacterDotMatrixLCD(const QSize displaySize, const QSize charMatrixSize, const QSize charMatrixDotsSize, int const  charSpace, QString* charSet, QWidget* parent, char const * name, WFlags f) : QWidget(parent, name, f)
{
	matrixQGridLayout_=0;
	recreate(displaySize, charMatrixSize, charMatrixDotsSize, charSpace, charSet);
	cursorIsON_=false;
	wholeMatrixCursor_=false;
	displayON_=true;
}

CharacterDotMatrixLCD::~CharacterDotMatrixLCD(void)
{

}

void CharacterDotMatrixLCD::recreate(const QSize displaySize, const QSize charMatrixSize, const QSize charMatrixDotsSize, int const  charSpace, QString* charSet)
{
	int indexCounter=0,n,m;
	if(matrixQGridLayout_)
	{
		for(m=0; m<dispSize_.height(); m++)
			for(n=0; n<dispSize_.width(); n++)
			{
				matrixQLabel_=matrixQLabelList_.at((m*dispSize_.width())+n);
				matrixQLabel_->clear();
				matrixQGridLayout_->remove(matrixQLabel_);
			}
		matrixQLabelList_.clear();
		matrixQGridLayout_->expand(1, 1);
	}
	else
		matrixQGridLayout_ = new QGridLayout(this, 1, 1, charSpace, charSpace, "matrixQGridLayout_");

	dispSize_=displaySize;               //Näytön koko (columns, rows)
	pointerToCharacterSet_ = charSet;
	createMatrix(charMatrixSize, charMatrixDotsSize);

	QString qstr;
	int w=charSpace*2,h=charSpace*2;
	matrixQGridLayout_->setSpacing(charSpace);
	matrixQGridLayout_->setMargin(charSpace);
	for(m=0; m<dispSize_.height(); m++)
	{
		for(n=0; n<dispSize_.width(); n++)
		{
			matrixQLabel_ = new QLabel("", this, "matrixQLabel_"+qstr.setNum(indexCounter,10), 0);
			matrixQLabel_->setPixmap(matrixQImage_);
			matrixQLabel_->show();
			matrixQGridLayout_->addWidget(matrixQLabel_, m, n, Qt::AlignCenter);
			matrixQLabelList_.append(matrixQLabel_);
			indexCounter++;
			if(!m)
				w+=charSpace+matrixQImage_.width();
		}
		h+=charSpace+matrixQImage_.height();
	}
	matrixQGridLayout_->setAlignment(Qt::AlignCenter);
	setMinimumSize(w,h);
	setMaximumSize(w,h);
	setGeometry(0,0,w,h);
	cursorIsON_=false;   //Kursori pois päältä jotta matriisi kursorin kohalla ei muutu.
}

void CharacterDotMatrixLCD::setDisplayONOFF(bool const  onOff)
{
	if(onOff)
	{
		displayON_=true;
	}
	else
	{
		clearCursor();
		for(int n=0; n<dispSize_.height(); n++)
			for(int m=0; m<dispSize_.width(); m++)
				setChar(n, m, ' ');
		displayON_=false;
	}
}

void CharacterDotMatrixLCD::setChar(int const  row, int const  col, char const  c)
{
	if((row<dispSize_.height()) && (col<dispSize_.width()) && (col>=0) && (row>=0) && displayON_)
	{
		QString charLine;
		pointerQImage32_=matrixQImage_.copy();
		for(int n=0; n<charMSize_.height(); n++)
		{
			int cSetRow=((unsigned char)c)*charMSize_.height()+n;
			charLine=*(pointerToCharacterSet_+cSetRow);
			for(int m=0; m<charMSize_.width(); m++)
			{
				if(charLine[m]=='1')
				{
					for(int i=0; i<charMDotsSize_.width(); i++)
						for(int j=0; j<charMDotsSize_.height(); j++)
							pointerQImage32_.setPixel(m*(charMDotsSize_.width()+1)+i+1, n*(charMDotsSize_.height()+1)+j+1, qRgb(0x25,0x31,0x97));
				}
			}
		}
		matrixQLabel_=matrixQLabelList_.at((row*dispSize_.width())+col);
		matrixQLabel_->setPixmap(pointerQImage32_);
	}
}

void CharacterDotMatrixLCD::setCursor(int const  row, int const  col)
{
	if((row<dispSize_.height()) && (col<dispSize_.width()) && (col>=0) && (row>=0) && displayON_)
	{
	//   if(cursorIsON_)
	//      clearCursor(); //Poistetaan edellinen
		cursorCol_=col;
		cursorRow_=row;
		cursorIsON_=true;
		matrixQLabel_=matrixQLabelList_.at((row*dispSize_.width())+col);
		pointerQImage32_=*matrixQLabel_->pixmap();
		cursorSaveQImage_=pointerQImage32_.copy();
		int startHeight;
		if(wholeMatrixCursor_)
			startHeight=0;
		else
			startHeight=charMSize_.height()-1;
		for(int n=startHeight; n<charMSize_.height(); n++)
			for(int m=0; m<charMSize_.width(); m++)
				for(int i=0; i<charMDotsSize_.width(); i++)
					for(int j=0; j<charMDotsSize_.height(); j++)
						pointerQImage32_.setPixel(m*(charMDotsSize_.width()+1)+i+1, n*(charMDotsSize_.height()+1)+j+1, qRgb(0x25,0x31,0x97));
		matrixQLabel_->setPixmap(pointerQImage32_);
	}
}

void CharacterDotMatrixLCD::clearCursor(void)
{
	if(cursorIsON_)
	{
		matrixQLabel_=matrixQLabelList_.at((cursorRow_*dispSize_.width())+cursorCol_);
		matrixQLabel_->setPixmap(cursorSaveQImage_);
		cursorIsON_=false;
	}
}

void CharacterDotMatrixLCD::setWholeMatrixCursor(bool const  wholeMatrix)
{
	wholeMatrixCursor_=wholeMatrix;
}

void CharacterDotMatrixLCD::createMatrix(const QSize& charMatrixSize, const QSize& charMatrixDotsSize)
{
	if((charMatrixSize.height()>0)&&(charMatrixSize.width()>0))
	{
		charMSize_=charMatrixSize;            //Character matrixin koko (width, height) pixels
		charMDotsSize_=charMatrixDotsSize;   //matriisin pistennkoko (width, height) pixels

		matrixQImage_.reset();
		matrixQImage_.create(charMSize_.width()*(charMDotsSize_.width()+1)+1, charMSize_.height()*(charMDotsSize_.height()+1)+1, 32, 0, QImage::IgnoreEndian);

		//Pohjaväri
		for(int y=0; y<charMSize_.height()*(charMDotsSize_.height()+1)+1; y++)
			for(int x=0; x<charMSize_.width()*(charMDotsSize_.width()+1)+1; x++)
				matrixQImage_.setPixel(x, y, qRgb(0xDD,0xDD,0xDD));
		//Ristikon horiz
		for(int y=0; y<charMSize_.height()*(charMDotsSize_.height()+1)+1; y+=charMDotsSize_.height()+1)
			for(int x=0; x<charMSize_.width()*(charMDotsSize_.width()+1)+1; x++)
				matrixQImage_.setPixel(x, y, qRgb(0x80,0x80,0x80));
		//Ristikon verti
		for(int y=0; y<charMSize_.height()*(charMDotsSize_.height()+1)+1; y++)
			for(int x=0; x<charMSize_.width()*(charMDotsSize_.width()+1)+1; x+=charMDotsSize_.width()+1)
				matrixQImage_.setPixel(x, y, qRgb(0x80,0x80,0x80));

	}
}

