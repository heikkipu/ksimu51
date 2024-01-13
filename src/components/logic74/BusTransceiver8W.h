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
#ifndef BUSTRANSCEIVER8W_H
#define BUSTRANSCEIVER8W_H

#include "Logic74B.h"

class Logic74BaseW;

class BusTransceiver8W : public Logic74BaseW
{
	Q_OBJECT
	public:
		BusTransceiver8W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~BusTransceiver8W();

	protected:
		void execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart(void) override;

	private:

		static double constexpr DELAYTIME = 0.000000025; //25ns TYP

		void loadComponentXPMImage(void);

		IOPin *pToGPin_;
		IOPin *pToDIRPin_;
		IOPin *pToA1Pin_;
		IOPin *pToB1Pin_;

		#define PINNAME_G "/G"
		#define PINNAME_DIR "DIR"
		#define PINNAME_A1 "A1"
		#define PINNAME_A2 "A2"
		#define PINNAME_A3 "A3"
		#define PINNAME_A4 "A4"
		#define PINNAME_A5 "A5"
		#define PINNAME_A6 "A6"
		#define PINNAME_A7 "A7"
		#define PINNAME_A8 "A8"
		#define PINNAME_B1 "B1"
		#define PINNAME_B2 "B2"
		#define PINNAME_B3 "B3"
		#define PINNAME_B4 "B4"
		#define PINNAME_B5 "B5"
		#define PINNAME_B6 "B6"
		#define PINNAME_B7 "B7"
		#define PINNAME_B8 "B8"
};

#endif
