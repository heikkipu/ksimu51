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
#ifndef LATCH8W_H
#define LATCH8W_H

#include "Logic74B.h"

class Logic74BaseW;

class Latch8W : public Logic74BaseW
{
	Q_OBJECT
	public:
		Latch8W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Latch8W();

	protected:
		void execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart(void) override;

	private:

		void loadComponentXPMImage(void);

		IOPin *pToENPin_;
		IOPin *pToDPin_;
		IOPin *pToD0Pin_;
		IOPin *pToQ0Pin_;


};

namespace Latch8W_NameS
{
	double const  DELAYTIME=0.000000027; //27ns TYP
	
	#define PINNAME_EN "/EN"
	#define PINNAME_C1 "C1"
	#define PINNAME_D0 "D0"
	#define PINNAME_D1 "D1"
	#define PINNAME_D2 "D2"
	#define PINNAME_D3 "D3"
	#define PINNAME_D4 "D4"
	#define PINNAME_D5 "D5"
	#define PINNAME_D6 "D6"
	#define PINNAME_D7 "D7"
	#define PINNAME_Q0 "Q0"
	#define PINNAME_Q1 "Q1"
	#define PINNAME_Q2 "Q2"
	#define PINNAME_Q3 "Q3"
	#define PINNAME_Q4 "Q4"
	#define PINNAME_Q5 "Q5"
	#define PINNAME_Q6 "Q6"
	#define PINNAME_Q7 "Q7"
};

#endif
