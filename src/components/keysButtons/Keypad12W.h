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
#ifndef KEYPAD12W_H
#define KEYPAD12W_H

#include "../ExternalComponentBaseW.h"

#include <qlayout.h>
#include <qcheckbox.h>

#include <kpushbutton.h>

class KPopupMenu;
class KPushButton;

class Keypad12W : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		Keypad12W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Keypad12W();

		void clear(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		double execute_VirtualPart(double executeTime) override;

	private slots:
		void keyFuncQCheckBoxStateChanged(int s);
		void outputLevelQCheckBoxStateChanged(int s);

		void button1Pressed(void);
		void button1Released(void);
		void button2Pressed(void);
		void button2Released(void);
		void button3Pressed(void);
		void button3Released(void);
		void button4Pressed(void);
		void button4Released(void);
		void button5Pressed(void);
		void button5Released(void);
		void button6Pressed(void);
		void button6Released(void);
		void button7Pressed(void);
		void button7Released(void);
		void button8Pressed(void);
		void button8Released(void);
		void button9Pressed(void);
		void button9Released(void);
		void button0Pressed(void);
		void button0Released(void);
		void risuaitaPressed(void);
		void risuaitaReleased(void);
		void asteriskiPressed(void);
		void asteriskiReleased(void);

	private:
		//char digitValue[4];

		IOPin *pointterC0_;
		IOPin *pointterR0_;
		IOPin *activeButtonColumn_;
		IOPin *activeButtonRow_;

		bool buttonOn_;   //Kytkin painettu pulssi
		bool buttonOff_;   //Kytkin vapautettu pulssi
		int row_, column_;    //Kytkimen rivi ja sarakepiuhat
		bool buttonIsOn_; // Button On tilassa

		bool columnInput0PrevState_;
		bool columnInput1PrevState_;
		bool columnInput2PrevState_;

		bool toggleButtons_;
		bool outputActiveLevelLow_;


		void addPopupMenu(void);

		QGridLayout* keypadQGridLayout_;
		QCheckBox* keyFuncQCheckBox_;
		QCheckBox* outputLevelQCheckBox_;

		KPushButton* pushButtonPointerTable_[12];
		KPushButton* number1KPushButton_;
		KPushButton* number2KPushButton_;
		KPushButton* number3KPushButton_;
		KPushButton* number4KPushButton_;
		KPushButton* number5KPushButton_;
		KPushButton* number6KPushButton_;
		KPushButton* number7KPushButton_;
		KPushButton* number8KPushButton_;
		KPushButton* number9KPushButton_;
		KPushButton* number0KPushButton_;
		KPushButton* risuaitaKPushButton_;
		KPushButton* asteriskiKPushButton_;

		void setButtonsOff(int dontChangeButton); //-1 offaa kaikki
		void setOutputsDeactiveState(void);

		#define PINNAME_C0 "C0"      //Column
		#define PINNAME_C1 "C1"
		#define PINNAME_C2 "C2"
		#define PINNAME_R0 "R0"      //Row
		#define PINNAME_R1 "R1"
		#define PINNAME_R2 "R2"
		#define PINNAME_R3 "R3"

		enum buttons
		{
			BN1,
			BN2,
			BN3,
			BN4,
			BN5,
			BN6,
			BN7,
			BN8,
			BN9,
			BN0,
			BRISUAITA,
			BASTERISKI,
		};
};

#endif
