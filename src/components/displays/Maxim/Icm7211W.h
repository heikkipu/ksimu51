/**************************************************************************
*   Copyright (C) 2005 - ... Heikki Pulkkinen                             *
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
#ifndef ICM7211W_H
#define ICM7211W_H

#include "../../ExternalComponentBaseW.h"

#include <qlayout.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>

class QLCDNumber;



class Icm7211W : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		Icm7211W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Icm7211W();

		void clear(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:

		void update_VirtualPart(void) override;
		double execute_VirtualPart(double executeTime) override;


	private slots:
		void codeBQCheckBoxStateChanged(int s);

	private:
		static unsigned char const  NUM_DIGITS=4;
		char digitValue_[NUM_DIGITS];
		char prevDigitValue_[NUM_DIGITS]; //Päivitystoimintaa varten

		IOPin* pointterDS0_; //Apupointteri joka osoittaa aina pinnilistan dataselect pinnien ekpinniin DS0:aan.
		IOPin* pointterD0_; //Apupointteri joka osoittaa aina pinnilistan ekaan datapinniin.

		bool codeBIsOn_;


		QGridLayout* displayQGridLayout_;
		QLCDNumber* displayQLCDNumber_;
		QCheckBox* codeBQCheckBox_;

		#define PINNAME_DS0 "DS0"
		#define PINNAME_DS1 "DS1"
		#define PINNAME_DS2 "DS2"
		#define PINNAME_DS3 "DS3"
		#define PINNAME_D0 "D0"
		#define PINNAME_D1 "D1"
		#define PINNAME_D2 "D2"
		#define PINNAME_D3 "D3"
};

#endif
