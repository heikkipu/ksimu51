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
#include "ShareMainW.h"
#include "../processorsW/I51BaseW.h"

ShareMainW::ShareMainW( QWidget* parent, char const * name, int wflags) : KMainWindow(parent, name, wflags)
{
	clearWFlags(Qt::WStyle_NormalBorder|Qt::WStyle_NoBorder);
	setWFlags(Qt::WStyle_DialogBorder);
	clearWFlags(KMainWindow::WStyle_MinMax);
	setMinimumSize(20,20);
	setIcon(nullIcon_);
	setFocusPolicy(QWidget::StrongFocus);

	originalActiveColorGroup_=palette().active();
}

ShareMainW::~ShareMainW()
{
}

//Oletusmaksimikoko ikkunalle. Jos muu, perivä luokka määrittelee uudelleen.
QSize ShareMainW::getMaximumSize(void)
{
	return QSize(I51BaseW::WINDOW_WIDTH_MAX, I51BaseW::WINDOW_HEIGHT_MAX);
}

void ShareMainW::closeEvent(QCloseEvent *e)
{
	this->showMinimized();
}

void ShareMainW::setActiveStyle(void)
{
	QPalette activePalette=palette();
	QColorGroup cg=activePalette.active();
	QColor c=cg.color(QColorGroup::Background);
	cg.setColor(QColorGroup::Background, c.dark(140));
	activePalette.setActive(cg);
	setPalette(activePalette);
}

void ShareMainW::setNormalStyle(void)
{
	QPalette originalPalette=palette();
	originalPalette.setActive(originalActiveColorGroup_);
	setPalette(originalPalette);
}

bool ShareMainW::event(QEvent *e)
{
	bool hit=false; //Voidaan säädellä lähetetäänkö eventti eteenpäin.
	QPoint p;
	switch(int(e->type()))
	{
		//Suodatetaan maximize pois
		case QEvent::ShowMaximized:
			p=this->parentWidget()->pos();
			this->parentWidget()->setGeometry(p.x(), p.y(), 10, 10); //Poistaa epämääräistä toimintaa
			this->parentWidget()->setGeometry(p.x(), p.y(), this->parentWidget()->parentWidget()->width()-18, this->parentWidget()->parentWidget()->height()-18);
			hit=true;
			break;

		case QEvent::MouseButtonPress:
			hit=false;
			break;

		//Tähän voi lisätä eventtien käsittelyä jos tarttee?!?!?!?
		/* Poistettu käytöstä kokeilun jälkeen.
		case QEvent::Enter:
			entered();
			break;
		case QEvent::Leave:
			leaved();
			break;
		case QEvent::User+1:
			int mouseP=1;
			break;
		*/
	}
	if(!hit)
		return KMainWindow::event(e);
	return true;
}

void ShareMainW::showEvent(QShowEvent * e)
{
	parentWidget()->setMaximumSize(getMaximumSize());
	KMainWindow::showEvent(e);
}


