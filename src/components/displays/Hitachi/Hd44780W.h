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
#ifndef HD44780W_H
#define HD44780W_H

#include "../CharacterDotMatrixLCD.h"
#include "../../../share/LedIndicatorGrid.h"
#include "../../ExternalComponentBaseW.h"

#include <qlayout.h>
#include <qslider.h>


namespace Hd44780W_NameS
{
	double const  DELAYTIME=0.000037;
	double const  ENABLEPULSEWIDTH=0.000000230; //230ns

	unsigned char const  DISPLAY_RAM_SIZE=80;
	unsigned char const  CHARACTER_RAM_SIZE=64;

	unsigned char const  BF=0x80;   //Busy flagin paikka StatusRead tavussa
	unsigned char const  DL=0x10; //DL flagin paikka FunctionSet tavussa.
	unsigned char const  N=0x08; //N flagin paikka FunctionSet tavussa
	unsigned char const  F=0x04; //F flagin paikka FunctionSet tavussa
	unsigned char const  D=0x04; //D flagin paikka DispalyControl tavussa
	unsigned char const  C=0x02; //C flagin paikka DispalyControl tavussa
	unsigned char const  B=0x01; //B flagin paikka DispalyControl tavussa
	unsigned char const  ID=0x02; //ID flagin paikka EntryMode tavussa
	unsigned char const  S=0x01; //S flagin paikka EntryMode tavussa
	unsigned char const  SC=0x08; //SC flagin paikka DisplayShift/CursosrMove tavussa
	unsigned char const  RL=0x04; //RL flagin paikka DisplayShift/CursosrMove tavussa

	#define PINNAME_DB0 "DB0"
	#define PINNAME_DB1 "DB1"
	#define PINNAME_DB2 "DB2"
	#define PINNAME_DB3 "DB3"
	#define PINNAME_DB4 "DB4"
	#define PINNAME_DB5 "DB5"
	#define PINNAME_DB6 "DB6"
	#define PINNAME_DB7 "DB7"
	#define PINNAME_EN "EN"
	#define PINNAME_RS "RS"
	#define PINNAME_RW "R;/W"
};



class Hd44780W : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		Hd44780W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Hd44780W(void);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void update_VirtualPart(void) override;
		double execute_VirtualPart(double executeTime) override;

	private slots:
		void numColumnsQSliderReleased();
		void numColumnsQSliderMoved(int value);

	private:

		enum works
		{
			CLEAR=1,
			HOME,
			ONOFF,
			SHIFTRIGHT,
			SHIFTLEFT,
			MOVECURSORRIGHT,
			MOVECURSORLEFT,
			WRITE_RAM,
			READ_RAM,
		};

		unsigned char workToDo_;
		void doWork(void);

		QGridLayout* displayQGridLayout_;

		IOPin* pToDB0Pin_;
		IOPin* pToENPin_;
		IOPin* pToRSPin_;
		IOPin* pToRWPin_;

		QSlider* numColumnsQSlider_;
		QTable* registerTable_;

		CharacterDotMatrixLCD* dotMatrix_;
		QSize characterMatrixSize_;
		QSize dotSize_;
		unsigned char spaceBetweenMatrixies_;
		bool changesInMatrix_;
		void loadCharacterSet(void);
		QString* hd44780CharSet5x10_;
		QString* hd44780CharSet5x8_;

		LedIndicatorGrid* stateIndicator_;
		int busyIndicator_INDEX_;
		int dataLength4FirstNibbleIndicator_INDEX_;

		bool lastENPinValue_;

		bool wasOutput_;

		double instructionTime_;
		double lowToHighENPinStartTime_;
		double enablePinHighForStatus_;

		double blinkingCursorTimer_;
		bool blinkingState_;

		unsigned char instructionRegister_;
		unsigned char dataRegister_;
		int addressCounter_;
		void changeAddressCounter(bool const  incrementDecrement);
		int shiftCounter_; //Ei HD:n rekkari: shiftin apu. Jos decrement ja menee alle 0, niin vähennetään max arvosta eli 40:stä. Pitää olla aina positiivinen luku
		void shiftDisplay(bool const  incrementDecrement);
		int cursorPosition_;
		void moveCursor(bool const  incrementDecrement);

		bool usesDisplayRAM_; //1=dispaly RAM, 0=charcterRAM
		bool dataReadyToRead_;   //Rammin data valmis luettavaksi
		unsigned char displayRAM_[Hd44780W_NameS::DISPLAY_RAM_SIZE]; //DDRAM
		unsigned char characterRAM_[Hd44780W_NameS::CHARACTER_RAM_SIZE];

		bool busy_;   //Busy flag
		bool dataLength8_DL_;
		bool numLines2_N_;
		bool fontSet5x10_F_;
		bool displayOn_D_;
		bool cursorOn_C_;
		bool blinkingOn_B_;
		bool shiftIncrement_ID_;
		bool shift_S_;

		bool functionWasSet_;
		int dataLength4NibbleManipulationCounterForInst_;
		bool dataLength4FirstNibbleForStatus_;
		bool firstTimeToSetMatrix_;
		bool firstTimeTo4DL_;

		int lastInstruction_;

		void init(void);
		void doInstruction(void);

		unsigned char readFromDatapins(void);
		void writeToDatapins(unsigned char const  value);
		void setDatapinsDirection(bool const  direction);

		void updateRegisterTable(void);
		void setCharacterMatrix(void);
		void setWindowSize(void);


};

#endif
