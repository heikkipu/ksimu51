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
#ifndef LOGIC74BW_H
#define LOGIC74BW_H

//Logic74 luokan kantaluokka.

#include "../ExternalComponentBaseW.h"

#include <functional>
#include <qlabel.h>


class Logic74BaseW : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		Logic74BaseW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Logic74BaseW();

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		static double constexpr DELAYTIME74HCXXX = 0.000000010;   //10ns Typ. Asetettava erikseen perivässä luokassa
		static int constexpr DELAYTIME_RANDOMVALUE = 5; //0-5 ns.

		double execute_VirtualPart(double executeTime) override;

		void initW(void); //Perivät luokat kutsuvat construktorissa lopuksi tätä:PAKKO

		QLabel *componentImageQLabel_;
		QPixmap componentImage_;

};


#endif
