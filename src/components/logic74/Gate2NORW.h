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
#ifndef GATE2NORW_H
#define GATE2NORW_H

#include "Gate2NANDW.h"

class Gate2NANDW;

//Periytetty luokasta Gate2NANDW muuttamaalla logiikkaoperaatiota.

class Gate2NORW : public Gate2NANDW
{
	Q_OBJECT
	public:
		Gate2NORW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Gate2NORW();

	protected:
		bool logic2Operation(IOPin* inA, IOPin* inB) override;

	private:

		static double constexpr DELAYTIME = 0.000000009; //9ns TYP 25degreesC

		void loadComponentXPMImage(void);

};


#endif
