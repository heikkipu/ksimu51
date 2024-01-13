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
#ifndef GAL22V10W_H
#define GAL22V10W_H

#include "../ExternalComponentBaseW.h"


class Gal22V10W : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		Gal22V10W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Gal22V10W();

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void update_VirtualPart(void) override;
		double execute_VirtualPart(double executeTime) override;
		void execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart(void) override;//Yleisohjelman virtuaaliosa

		IOPin *pToIn1Pin_;
		IOPin *pToIn2Pin_;
		IOPin *pToIn3Pin_;
		IOPin *pToIn4Pin_;
		IOPin *pToIn5Pin_;
		IOPin *pToIn6Pin_;
		IOPin *pToIn7Pin_;
		IOPin *pToIn8Pin_;
		IOPin *pToIn9Pin_;
		IOPin *pToIn10Pin_;
		IOPin *pToIn11Pin_;
		IOPin *pToIn12Pin_;

		#define PINNAME_IN1 "IN1/CLK"
		#define PINNAME_IN2 "IN2"
		#define PINNAME_IN3 "IN3"
		#define PINNAME_IN4 "IN4"
		#define PINNAME_IN5 "IN5"
		#define PINNAME_IN6 "IN6"
		#define PINNAME_IN7 "IN7"
		#define PINNAME_IN8 "IN8"
		#define PINNAME_IN9 "IN9"
		#define PINNAME_IN10 "IN10"
		#define PINNAME_IN11 "IN11"
		#define PINNAME_IN12 "IN12"

		#define PINNAME_IO1 "IO1"
		#define PINNAME_IO2 "IO2"
		#define PINNAME_IO3 "IO3"
		#define PINNAME_IO4 "IO4"
		#define PINNAME_IO5 "IO5"
		#define PINNAME_IO6 "IO6"
		#define PINNAME_IO7 "IO7"
		#define PINNAME_IO8 "IO8"
		#define PINNAME_IO9 "IO9"
		#define PINNAME_IO10 "IO10"

	private:

		static double constexpr  DELAYTIMEGAL22V10 = 0.0000000075; //7,5ns TYP 25degreesC

};


#endif
