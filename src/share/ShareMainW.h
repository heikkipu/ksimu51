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
//Base class
#ifndef SHAREMAINW_H
#define SHAREMAINW_H

#include <kmainwindow.h>
#include <kpixmap.h>

#include <functional>
#include <qstylefactory.h>


class KMainWindow;

//Tämä on tällainen yhteisluokka eri ikkunoiden samojen toimintojen yhteistäiseksi.
class ShareMainW :  public KMainWindow
{
	Q_OBJECT
	public:
		ShareMainW(QWidget* parent, char const * name, int wflags);
		~ShareMainW();

		virtual QSize getMaximumSize(void); //Perivä luokka määrittelee uudelleen jos kiinteitä ikkunakokoja.

		void setActiveStyle(void);
		void setNormalStyle(void);
		
		virtual void saveWorkDataToFile(QDataStream& saveFileStream)=0;
		virtual void readWorkDataFromFile(QDataStream& readFileStream,
										  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)=0;

	protected:
		void closeEvent(QCloseEvent *e) override;
		bool event(QEvent *e) override;
		void showEvent(QShowEvent *e) override;

		KPixmap nullIcon_;

		QColorGroup originalActiveColorGroup_;
};

#endif
