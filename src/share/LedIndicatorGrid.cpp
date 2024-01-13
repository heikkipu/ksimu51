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
#include "LedIndicatorGrid.h"

LedIndicatorGrid::LedIndicatorGrid(int const  margin, int const  spacing, QWidget * parent, char const  * name, WFlags f) : QWidget(parent, name, f)
{
	indexCounter_=0;
	ledIndicatorQGridLayout_= new QGridLayout(this, 1, 1, 1, 1, "matrixQGridLayout");
	ledIndicatorQGridLayout_->setSpacing(spacing);
	ledIndicatorQGridLayout_->setMargin(margin);

}

LedIndicatorGrid::~LedIndicatorGrid()
{
}

int LedIndicatorGrid::addLed(int const  row, int const  col, const QString& text, const QColor& color)
{
	int retVal=indexCounter_;
	ledKLed_=new KLed(color, this, text);
	ledQLabel_=new QLabel(text , this, "ledQLabel_"+text, 0);
	//Joka toisessa sarakkeessa ledi ja joka toisessa teksti
	ledIndicatorQGridLayout_->addWidget(ledKLed_, row, col*2, Qt::AlignCenter);
	ledIndicatorQGridLayout_->addWidget(ledQLabel_, row, col*2+1,  Qt::AlignLeft);

	ledQLabelList_.append(ledKLed_);
	ledTextQLabelList_.append(ledQLabel_);

	indexCounter_++;

	setMaximumHeight(ledIndicatorQGridLayout_->numRows()*(ledKLed_->height()+ledIndicatorQGridLayout_->spacing())+(ledIndicatorQGridLayout_->margin()*2));

	return retVal;
}

void LedIndicatorGrid::ledON(int index)
{
	ledKLed_=ledQLabelList_.at(index);
	ledKLed_->setState(KLed::On);
}

void LedIndicatorGrid::ledOFF(int index)
{
	ledKLed_=ledQLabelList_.at(index);
	ledKLed_->setState(KLed::Off);
}

void LedIndicatorGrid::hideLed(int index)
{
	ledKLed_=ledQLabelList_.at(index);
	ledKLed_->hide();
	ledQLabel_=ledTextQLabelList_.at(index);
	ledQLabel_->hide();
}

void LedIndicatorGrid::showLed(int index)
{
	ledKLed_=ledQLabelList_.at(index);
	ledKLed_->show();
	ledQLabel_=ledTextQLabelList_.at(index);
	ledQLabel_->show();
}

