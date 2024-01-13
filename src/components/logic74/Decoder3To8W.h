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
#ifndef DECODER3TO8W_H
#define DECODER3TO8W_H

#include "Logic74B.h"

class Logic74BaseW;

class Decoder3To8W : public Logic74BaseW
{
	Q_OBJECT
	public:
		Decoder3To8W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Decoder3To8W();

	protected:
		void execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart(void) override;



	private:

		IOPin *pToAPin_;
		IOPin *pToG1Pin_;
		IOPin *pToG2APin_;
		IOPin *pToG2BPin_;
		IOPin *pToY0Pin_;

		#define PINNAME_A "A"
		#define PINNAME_B "B"
		#define PINNAME_C "C"
		#define PINNAME_G1 "G1"
		#define PINNAME_G2A "/G2A"
		#define PINNAME_G2B "/G2B"
		#define PINNAME_Y0 "Y0"
		#define PINNAME_Y1 "Y1"
		#define PINNAME_Y2 "Y2"
		#define PINNAME_Y3 "Y3"
		#define PINNAME_Y4 "Y4"
		#define PINNAME_Y5 "Y5"
		#define PINNAME_Y6 "Y6"
		#define PINNAME_Y7 "Y7"

		static double constexpr DELAYTIME = 0.000000022; //22ns TYP 25degreesC

		void loadComponentXPMImage(void);

};


#endif
