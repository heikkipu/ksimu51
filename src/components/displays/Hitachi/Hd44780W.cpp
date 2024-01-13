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
#include "Hd44780W.h"
#include "../../../ksimu51.h"

#include <qtooltip.h>


Hd44780W::Hd44780W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType)
{
	using namespace Hd44780W_NameS;
	componentName_+=" HD44780";

	firstTimeToSetMatrix_=true;

	displayQGridLayout_ = new QGridLayout(centralWidget(), 4, 1, 1, 1, "displayQGridLayout_");

	//Input pins
	addInputPin(PINNAME_EN);
	addInputPin(PINNAME_RS);
	addInputPin(PINNAME_RW);
	addBidirPinInLeftKToolBar(PINNAME_DB0);
	addBidirPinInLeftKToolBar(PINNAME_DB1);
	addBidirPinInLeftKToolBar(PINNAME_DB2);
	addBidirPinInLeftKToolBar(PINNAME_DB3);
	addBidirPinInLeftKToolBar(PINNAME_DB4);
	addBidirPinInLeftKToolBar(PINNAME_DB5);
	addBidirPinInLeftKToolBar(PINNAME_DB6);
	addBidirPinInLeftKToolBar(PINNAME_DB7);

	pToENPin_=ioList_.pToPin(PINNAME_EN);
	pToRSPin_=ioList_.pToPin(PINNAME_RS);
	pToRWPin_=ioList_.pToPin(PINNAME_RW);

	pToDB0Pin_=ioList_.pToPin(PINNAME_DB0);      //Pointteri DB0:n alkuun

	//Dot matrix
	//oletus 8columns x 1rows ; Matrix size 5x8, Dot Size 4x4 , Matriisien väli 3
	characterMatrixSize_=QSize(8,1);
	dotSize_=QSize(4,4);
	spaceBetweenMatrixies_=3;
	loadCharacterSet();
	dotMatrix_ = new CharacterDotMatrixLCD(characterMatrixSize_, QSize(5,8), dotSize_, spaceBetweenMatrixies_, &hd44780CharSet5x8_[0], centralWidget(), "dotMatrix_", 0);
	displayQGridLayout_->addWidget(dotMatrix_, 0, 0, Qt::AlignCenter);
	dotMatrix_->hide();

	//Sarakemääräslideri
	numColumnsQSlider_=new QSlider(Qt::Horizontal, centralWidget(), "numColumnsQSlider_");
	numColumnsQSlider_->setRange(1, 80);
	numColumnsQSlider_->setValue(8);
	//numColumnsQSlider_->setTracking(true);
	displayQGridLayout_->addWidget(numColumnsQSlider_, 1, 0, Qt::AlignCenter);
	connect(numColumnsQSlider_, SIGNAL(sliderReleased()), this, SLOT(numColumnsQSliderReleased()));
	connect(numColumnsQSlider_, SIGNAL(sliderMoved(int)), this, SLOT(numColumnsQSliderMoved(int)));
	numColumnsQSlider_->hide();

	//Rekisteritaulukko
	registerTable_ = new QTable(centralWidget(),"registerTable_");
	QFont registerTable_font(registerTable_->font());
	registerTable_font.setFamily(FONTFAMILY);
	registerTable_font.setPointSize(10);
	registerTable_->setFont(registerTable_font);
	registerTable_->setFrameShape( QTable::GroupBoxPanel );
	registerTable_->setNumRows(2);
	registerTable_->setNumCols(9);
	registerTable_->verticalHeader()->setLabel(0, "IR");
	registerTable_->verticalHeader()->setLabel(1, "DR");
	int width=registerTable_->verticalHeader()-> frameGeometry().width();
	registerTable_->horizontalHeader()->setLabel(0," H ");
	registerTable_->adjustColumn(0);
	width+=registerTable_->columnWidth(0);
	for(int n=7; n>=0; n--)
	{
		QString qStr;
		qStr.setNum(n,16);
		qStr.prepend(" b");
		registerTable_->horizontalHeader()->setLabel(8-n,qStr);
		registerTable_->adjustColumn(8-n);
		width+=registerTable_->columnWidth(8-n);
	}
	width+=4;
	registerTable_->setRowMovingEnabled(false);
	registerTable_->setColumnMovingEnabled(false);
	registerTable_->setReadOnly(true);
	registerTable_->setFixedSize(width, registerTable_->rowHeight(0)*2+registerTable_->horizontalHeader()-> frameGeometry().height()+4);
	displayQGridLayout_->addWidget(registerTable_, 2, 0, Qt::AlignLeft);

	stateIndicator_=new LedIndicatorGrid(2,5, centralWidget(), "stateIndicator_", 0);
	displayQGridLayout_->addWidget(stateIndicator_, 3, 0, Qt::AlignLeft);
	busyIndicator_INDEX_=stateIndicator_->addLed(0,0,"BUSY",Qt::red);
	dataLength4FirstNibbleIndicator_INDEX_=stateIndicator_->addLed(0,1,"DL4 HighN",Qt::red);
	stateIndicator_->ledOFF(busyIndicator_INDEX_);
	stateIndicator_->ledOFF(dataLength4FirstNibbleIndicator_INDEX_);
	stateIndicator_->hideLed(dataLength4FirstNibbleIndicator_INDEX_);


	setWindowSize();

	init();
	lastInstruction_=0;
	firstTimeTo4DL_=true;
	blinkingState_=true;
	dataReadyToRead_=false;
	enablePinHighForStatus_=false;
	workToDo_=0;
	changesInMatrix_=false;
}

Hd44780W::~Hd44780W(void)
{

}

void Hd44780W::setWindowSize(void)
{
	unsigned int dMatrixW=dotMatrix_->width();
	unsigned int heights=dotMatrix_->height()+registerTable_->height()+numColumnsQSlider_->height()+stateIndicator_->height();
	unsigned int rTableW=registerTable_->width();
	unsigned int ioListH=(inputPinKListBox_->itemHeight(0)*inputPinKListBox_->numRows())+(bidirPinKListBoxInLeftKToolBar_->itemHeight(0)*bidirPinKListBoxInLeftKToolBar_->numRows());
	unsigned int biggerW=QMAX(rTableW, dMatrixW);
	unsigned int biggerH=QMAX(ioListH, heights);
	unsigned int heightM=biggerH+38;
	unsigned int widthM=bidirPinKListBoxInLeftKToolBar_->maxItemWidth()+biggerW+10;
	maxWSize_=QSize(widthM,heightM);
	setGeometry(0,0,10,10);
	showMinimized();
	showNormal();
	setGeometry(0,0,widthM,heightM);
	numColumnsQSlider_->setMinimumWidth(biggerW-20);
}

void Hd44780W::init(void)
{
	wasOutput_=false;
	functionWasSet_=false;
	busy_=false;
	//Flagien alkuarvot
	dataLength8_DL_=true; //8 bit datalength
	numLines2_N_=false; //1 line display
	fontSet5x10_F_=false; //5x8 fonttisetti
	displayOn_D_=false; //Dispaly OFF
	cursorOn_C_=false; //Cursor OFF
	blinkingOn_B_=false; //Blinking OFF
	shiftIncrement_ID_=true; //Inkrement 1
	shift_S_=false; //No shift

	addressCounter_=0;
	dataLength4NibbleManipulationCounterForInst_=0;
	dataLength4FirstNibbleForStatus_=false;
	lastENPinValue_=pToENPin_->readValue(false);
}

void Hd44780W::updateRegisterTable(void)
{
	QString qStr, qStrT[8];
	int n;
	//Instruction register Hex value
	qStr=qStr.setNum(instructionRegister_,16);
	if(qStr.length()<2)
		qStr.prepend('0');
	registerTable_->setText(0, 0, qStr.upper());
	//Data register HEX value
	qStr=qStr.setNum(dataRegister_,16);
	if(qStr.length()<2)
		qStr.prepend('0');
	registerTable_->setText(1, 0, qStr.upper());
	//BIN val
	switch(lastInstruction_)
	{
		case 0x01: //CLR
			qStrT[0]="CLR";
			break;
		case 0x02: //HOME
			qStrT[1]="HOME";
			break;
		case 0x20: //Function set
			qStrT[2]="F=";
			qStrT[3]="N=";
			qStrT[4]="DL=";
			qStrT[5]="FSet";
			break;
		case 0x10: //Cursor or display shift
			qStrT[2]="RL=";
			qStrT[3]="SC=";
			qStrT[4]="SFT";
			break;
		case 0x08: //Display on/off control
			qStrT[0]="B=";
			qStrT[1]="C=";
			qStrT[2]="D=";
			qStrT[3]="OnOf";
			break;
		case 0x04: //Entry mode set
			qStrT[0]="S=";
			qStrT[1]="ID=";
			qStrT[2]="EnMo";
			break;
	}
	for(n=0; n<8; n++)
	{
		//Instruction register
		if(instructionRegister_&(1<<n))
			qStrT[n].append("1");
		else
			qStrT[n].append("0");
		registerTable_->setText(0, 8-n, qStrT[n]);
		//Data register
		if(dataRegister_&(1<<n))
			qStr="1";
		else
			qStr="0";
		registerTable_->setText(1, 8-n, qStr);
	}
}

void Hd44780W::update_VirtualPart(void)
{
	updateRegisterTable();

	if(busy_)
		stateIndicator_->ledON(busyIndicator_INDEX_);
	else
		stateIndicator_->ledOFF(busyIndicator_INDEX_);

	if(!dataLength8_DL_)
	{
		if(firstTimeTo4DL_)
		{
			stateIndicator_->showLed(dataLength4FirstNibbleIndicator_INDEX_);
			firstTimeTo4DL_=false;
		}
		if((dataLength4NibbleManipulationCounterForInst_==1)||dataLength4FirstNibbleForStatus_)
		//if(dataLength4NibbleManipulationCounterForInst_==1)
			stateIndicator_->ledON(dataLength4FirstNibbleIndicator_INDEX_);
		else
			stateIndicator_->ledOFF(dataLength4FirstNibbleIndicator_INDEX_);
	}

	//Siirretään display ramista tiedot matriiseihin.
	if(functionWasSet_ && displayOn_D_ && changesInMatrix_)
	{
		//Poistetaan kursori
		dotMatrix_->clearCursor();
		for(int n=0; n<characterMatrixSize_.width(); n++)
		{
			unsigned char pointOfDRAM=shiftCounter_+n;
			if(numLines2_N_) //Jos 2 rivin näyttö
			{
					//Lasketaan kohta
					if(pointOfDRAM>=40)
						pointOfDRAM=n;
					//Line1
					dotMatrix_->setChar(0, n, displayRAM_[pointOfDRAM]);
					//Line2
					dotMatrix_->setChar(1, n, displayRAM_[pointOfDRAM+0x28]); //2 rivin paikka muistissa on 28H-> vaikka sitä osoitetaan 40H->
					//Cursor
					if((pointOfDRAM==cursorPosition_)&&cursorOn_C_)
						dotMatrix_->setCursor(0,n);
					if(((pointOfDRAM+0x28)==cursorPosition_)&&cursorOn_C_)
						dotMatrix_->setCursor(1,n);
			}
			else
			{
				if(pointOfDRAM>0x4F)
					pointOfDRAM=n;
				dotMatrix_->setChar(0, n, displayRAM_[pointOfDRAM]);
				//Cursor
				if((pointOfDRAM==cursorPosition_)&&cursorOn_C_)
					dotMatrix_->setCursor(0,n);
			}
		}
		changesInMatrix_=false;
	}
}

double Hd44780W::execute_VirtualPart(double executeTime)
{
	using namespace Hd44780W_NameS;
	
	double retVal=executeTime+1.0;
	//Katsotaan että EN on ollut tarpeeksi kauan ylhäällä.
	if(!lastENPinValue_&&pToENPin_->readValue(true)) //EN pinnin nousu
	{
		lowToHighENPinStartTime_=executeTime;
		retVal=lowToHighENPinStartTime_+ENABLEPULSEWIDTH;
		enablePinHighForStatus_=true;
	}

	bool enableEnoughUP=false;
	if((executeTime-lowToHighENPinStartTime_)>=ENABLEPULSEWIDTH)
		enableEnoughUP=true;

	if(!busy_&&enableEnoughUP) //Jos ei ole joku homma käynnissä
	{
		if(lastENPinValue_&&!pToENPin_->readValue(true)) //Katostaan meneekö EN alas
		{
			if(dataLength8_DL_) //Jos 8 bitidataleveys
			{
				if(!pToRSPin_->readValue(true)) //Jos instruction rekisteri
					instructionRegister_=readFromDatapins();
				else
					dataRegister_=readFromDatapins();
			}
			else //Jos 4 bitin dataleveys
			{
				if(!dataLength4NibbleManipulationCounterForInst_)
				{
					if(!pToRSPin_->readValue(true)) //Jos instruction rekisteri
						instructionRegister_=readFromDatapins()&0xF0;
					else
						dataRegister_=readFromDatapins()&0xF0;
				}
				else
				{
					unsigned char c=readFromDatapins();
					if(!pToRSPin_->readValue(true)) //Jos instruction rekisteri
						instructionRegister_|=(c>>4)&0x0F;
					else
						dataRegister_|=(c>>4)&0x0F;
				}
				dataLength4NibbleManipulationCounterForInst_++;
			}
			if(dataLength8_DL_||(!dataLength8_DL_&&dataLength4NibbleManipulationCounterForInst_>=2))
			{
				if(!pToRSPin_->readValue(true)) //Jos instruction rekisteri
				{
					if(!pToRWPin_->readValue(true))   //Jos kirjotus
					{
						doInstruction();
						executionStartStepTime_=executeTime;
						executionReadyTime_=executeTime+instructionTime_;
						busy_=true;
						changes_=true;
					}
				}
				else //Jos datarekisteri
				{
					if(pToRWPin_->readValue(true)) //Jos luku
					{
						if(dataReadyToRead_)
						{
							setDatapinsDirection(IOPin::OUTPUT);
							wasOutput_=true;
							writeToDatapins(dataRegister_);
						}

						workToDo_=READ_RAM;
						dataReadyToRead_=true; //seuraava data tulee valmiiksi
					}
					else //Jos kirjotus
						workToDo_=WRITE_RAM;
					executionStartStepTime_=executeTime;
					executionReadyTime_=executeTime+instructionTime_;
					busy_=true;
					changes_=true;
				}
				dataLength4NibbleManipulationCounterForInst_=0;
			}
		}
	}
	else   //Jos on homma käynnissä
	{
		if(executeTime>=executionReadyTime_) //Jos homman käsittely valmis
		{
			busy_=false;
			changes_=true;

			if(workToDo_)
				doWork();

			changesInMatrix_=true;

		}
	}

	//Katsotaan jos kurkitaan statusta
	//if(pToENPin_->readValue(true)&&pToRWPin_->readValue(true)&&!pToRSPin_->readValue(true)&&!dataLength4NibbleManipulationCounterForInst_)
	if(pToENPin_->readValue(true)&&pToRWPin_->readValue(true)&&!pToRSPin_->readValue(true)&&enableEnoughUP)
	{
		if(enablePinHighForStatus_)
		{
			unsigned char c=addressCounter_&0x7F; //Ylin bitti kuuluu bysylle
			if(busy_)
				c|=BF;
			instructionRegister_=c;
			if(!dataLength8_DL_) //Jos 4 bitidataleveys
			{
				if(!dataLength4FirstNibbleForStatus_)
				{
					dataLength4FirstNibbleForStatus_=true;
				}
				else
				{
					c=c<<4;
					dataLength4FirstNibbleForStatus_=false;
				}
			}
			if(!wasOutput_)
			{
				setDatapinsDirection(IOPin::OUTPUT);
				wasOutput_=true;
			}
			writeToDatapins(c);
			lastInstruction_=0;
			enablePinHighForStatus_=false;
		}
	}
	else
	{
		if(wasOutput_)
		{
			setDatapinsDirection(IOPin::INPUT);
			wasOutput_=false;
		}
	}


	if(blinkingOn_B_)
	{
		blinkingCursorTimer_+=(*pToLastStepTime_-blinkingCursorTimer_);
		if(blinkingCursorTimer_>.4)
		{
			if(blinkingState_)
				blinkingState_=false;
			else
				blinkingState_=true;
			blinkingCursorTimer_=*pToLastStepTime_;
		}
	}

	lastENPinValue_=pToENPin_->readValue(true);

	if(busy_)
		retVal=executionReadyTime_;

	return retVal;
}

void Hd44780W::doWork(void)
{
	using namespace Hd44780W_NameS;

	switch(workToDo_)
	{
		case CLEAR:
			for(int m=0; m<DISPLAY_RAM_SIZE; m++)
				displayRAM_[m]=' ';
			addressCounter_=0;
			shiftCounter_=0;
			cursorPosition_=0;
			shiftIncrement_ID_=true;
			break;
		case HOME:
			addressCounter_=0;
			shiftCounter_=0;
			cursorPosition_=0;
			break;
		case ONOFF:
			dotMatrix_->setDisplayONOFF(displayOn_D_);
			if(blinkingOn_B_)
				blinkingCursorTimer_=*pToLastStepTime_;
			break;
		case SHIFTRIGHT:
			shiftDisplay(false);
			break;
		case SHIFTLEFT:
			shiftDisplay(true);
			break;
		case MOVECURSORRIGHT:
			moveCursor(true);
			break;
		case MOVECURSORLEFT:
			moveCursor(false);
			break;
		case WRITE_RAM:
			if(usesDisplayRAM_)
			{
				displayRAM_[addressCounter_]=dataRegister_;
				moveCursor(shiftIncrement_ID_);
				if(shift_S_)
					shiftDisplay(shiftIncrement_ID_);
			}
			else
			{
				characterRAM_[addressCounter_]=dataRegister_;
				changeAddressCounter(shiftIncrement_ID_);
			}
			break;
		case READ_RAM:
			if(usesDisplayRAM_)
				dataRegister_=displayRAM_[addressCounter_];
			else
				dataRegister_=characterRAM_[addressCounter_];
			changeAddressCounter(shiftIncrement_ID_);
			break;
	}
	workToDo_=0;
}

void Hd44780W::doInstruction(void)
{
	using namespace Hd44780W_NameS;
	
	instructionTime_=DELAYTIME; //Lähes kaikilla käskyillä tämä aika
	lastInstruction_=0;
	dataReadyToRead_=false;
	for(lastInstruction_=0x80; lastInstruction_>0; )
	{
		if(instructionRegister_>=lastInstruction_)
			break;
		lastInstruction_=lastInstruction_>>1;
	}
	switch(lastInstruction_)
	{
		case 0x80: //Set DDRAM Address
			if(functionWasSet_)
			{
				int ac=instructionRegister_&0x7F;
				if(numLines2_N_)
				{
					if((ac>=0x40)&&(ac<=0x67))
						addressCounter_=ac-0x18;
					if((ac>=0x0)&&(ac<=0x27))
						addressCounter_=ac;
				}
				else
				{
					if((ac>=0x0)&&(ac<=0x4F))
						addressCounter_=ac;
				}
				cursorPosition_=addressCounter_;
				usesDisplayRAM_=true;
				dataReadyToRead_=true;
			}
			break;
		case 0x40: //Set CGRAM Address
			if(functionWasSet_)
			{
				addressCounter_=instructionRegister_&0x3F;
				usesDisplayRAM_=false;
				dataReadyToRead_=true;
			}
			break;
		case 0x20: //Function set
			if(!functionWasSet_)
			{
				dataLength8_DL_=instructionRegister_&DL;
				if(dataLength8_DL_||(!dataLength8_DL_&&dataLength4NibbleManipulationCounterForInst_))
				{
					numLines2_N_=instructionRegister_&N;
					fontSet5x10_F_=instructionRegister_&F;
					setCharacterMatrix();
					functionWasSet_=true;
				}
				usesDisplayRAM_=true;
			}
			break;
		case 0x10: //Cursor or display shift
			if(functionWasSet_)
			{
				//Display shift
				if(instructionRegister_&SC)
					if(instructionRegister_&RL)
						workToDo_=SHIFTRIGHT;
					else
						workToDo_=SHIFTLEFT;
				else
					//Cursor move
					if(instructionRegister_&RL)
						workToDo_=MOVECURSORRIGHT;
					else
						workToDo_=MOVECURSORLEFT;
			}
			break;
		case 0x08: //Display on/off control
			if(functionWasSet_)
			{
				displayOn_D_=instructionRegister_&D;
				cursorOn_C_=instructionRegister_&C;
				blinkingOn_B_=instructionRegister_&B;
				workToDo_=ONOFF;
			}
			break;
		case 0x04: //Entry mode set
			if(functionWasSet_)
			{
				shiftIncrement_ID_=instructionRegister_&ID;
				shift_S_=instructionRegister_&S;
			}
			break;
		case 0x02: //Return home
			if(functionWasSet_)
			{
				workToDo_=HOME;
				instructionTime_=0.00152;
			}
			break;
		case 0x01: //Clear
			if(functionWasSet_)
				workToDo_=CLEAR;
			break;
		default:;
	};
}

void Hd44780W::shiftDisplay(bool const  incrementDecrement)
{
	if(incrementDecrement) //Oikeelle
		shiftCounter_++;
	else
		shiftCounter_--;
	if(numLines2_N_)   //Jos 2 rivin näyttö
	{
		if(shiftCounter_>39)
			shiftCounter_=0;
		if(shiftCounter_<0)
			shiftCounter_=39;
	}
	else
	{
		if(shiftCounter_>79)
			shiftCounter_=0;
		if(shiftCounter_<0)
			shiftCounter_=79;
	}
}

void Hd44780W::moveCursor(bool const  incrementDecrement)
{
	if(incrementDecrement) //Oikeelle
		cursorPosition_++;
	else
		cursorPosition_--;
	if(cursorPosition_>79)
		cursorPosition_=0;
	if(cursorPosition_<0)
		cursorPosition_=79;

	addressCounter_=cursorPosition_; //Kursorin paikka määrää osoitteen paikan asetettaessa.
}

void Hd44780W::changeAddressCounter(bool const  incrementDecrement)
{
	if(incrementDecrement)
		addressCounter_++;
	else
		addressCounter_--;
	if(usesDisplayRAM_)
	{
		if(addressCounter_>79)
			addressCounter_=0;
		if(addressCounter_<0)
			addressCounter_=79;
	}
	else
	{
		if(addressCounter_>63)
			addressCounter_=0;
		if(addressCounter_<0)
			addressCounter_=63;
	}
}

unsigned char Hd44780W::readFromDatapins(void)
{
	IOPin* apuP=pToDB0Pin_;
	unsigned char retVal=0;
	for(int n=0;n<8;n++)
	{
		if(apuP->readValue(true))
			retVal|=(1<<n);
		apuP=apuP->next();
	}
	return retVal;
}

void Hd44780W::writeToDatapins(unsigned char const  value)
{
	IOPin* apuP=pToDB0Pin_;
	for(int n=0;n<8;n++)
	{
		if(value&(1<<n))
			apuP->writeValue(true, true);
		else
			apuP->writeValue(false, true);
		apuP=apuP->next();
	}
}

void Hd44780W::setDatapinsDirection(bool const  direction)
{
	IOPin* apuPinP=pToDB0Pin_;
	for(int n=0;n<8;n++)
	{
		apuPinP->setDirection(direction);
		apuPinP=apuPinP->next();
	}
}

void Hd44780W::setCharacterMatrix(void)
{
	if(firstTimeToSetMatrix_)
	{
		dotMatrix_->show();
		numColumnsQSlider_->show();
		firstTimeToSetMatrix_=false;
	}

	if(numLines2_N_)
	{
		characterMatrixSize_.setHeight(2);
		if(numColumnsQSlider_->maxValue()>40)
			numColumnsQSlider_->setRange(1, 40);
	}
	else
	{
		characterMatrixSize_.setHeight(1);
		if(numColumnsQSlider_->maxValue()<80)
			numColumnsQSlider_->setRange(1, 80);
	}
	characterMatrixSize_.setWidth(numColumnsQSlider_->value());
	numColumnsQSliderMoved(numColumnsQSlider_->value());

	dotSize_=QSize(4,4);
	spaceBetweenMatrixies_=4;
	if(characterMatrixSize_.width()>8)
	{
		dotSize_=QSize(3,3);
		spaceBetweenMatrixies_=3;
	}
	if(characterMatrixSize_.width()>16)
	{
		dotSize_=QSize(2,2);
		spaceBetweenMatrixies_=2;
	}

	if(fontSet5x10_F_)
		dotMatrix_->recreate(characterMatrixSize_, QSize(5,10), dotSize_, spaceBetweenMatrixies_, &hd44780CharSet5x10_[0]);
	else
		dotMatrix_->recreate(characterMatrixSize_, QSize(5,8), dotSize_, spaceBetweenMatrixies_, &hd44780CharSet5x8_[0]);

	setWindowSize();
}

void Hd44780W::numColumnsQSliderMoved(int value)
{
	QString qstr1,qstr2;
	setPlainCaption(componentName_+" ("+qstr1.setNum(value,10)+"x"+qstr2.setNum(characterMatrixSize_.height(),10)+")");
}

void Hd44780W::numColumnsQSliderReleased()
{
	if(numColumnsQSlider_->value()!=characterMatrixSize_.width())
	{
		characterMatrixSize_.setWidth(numColumnsQSlider_->value());
		setCharacterMatrix();
	}
	update_VirtualPart();
}

void Hd44780W::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);

	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void Hd44780W::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			//case X:
			//	readFileStream>>typeReadData>>Data_xxx_;
			//	break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}


void Hd44780W::loadCharacterSet(void)
{
	//Charactersetti 5x8 ja 5x10
	static QString CharSet5x10[]={
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","01000","01100","01110","01111","01110","01100","01000","00000","00000",
	"00000","00010","00110","01110","11110","01110","00110","00010","00000","00000",
	"00000","01001","10010","11011","00000","00000","00000","00000","00000","00000",
	"00000","11011","01001","10010","00000","00000","00000","00000","00000","00000",
	"00000","00100","01110","11111","00000","00100","01110","11111","00000","00000",
	"00000","11111","01110","00100","00000","11111","01110","00100","00000","00000",
	"00000","00000","01110","11111","11111","11111","01110","00000","00000","00000",
	"00000","00001","00001","00101","01001","11111","01000","00100","00000","00000",
	"00000","00100","01110","10101","00100","00100","00100","00100","00000","00000",
	"00000","00100","00100","00100","00100","10101","01110","00100","00000","00000",
	"00000","00000","00100","00010","11111","00010","00100","00000","00000","00000",
	"00000","00000","00100","01000","11111","01000","00100","00000","00000","00000",
	"00000","00010","00100","01000","00100","00010","00000","11111","00000","00000",
	"00000","01000","00100","00010","00100","01000","00000","11111","00000","00000",
	"00000","00000","00100","00100","01110","01110","11111","00000","00000","00000",
	"00000","00000","11111","01110","01110","00100","00100","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00100","00100","00100","00100","00000","00000","00100","00000","00000",
	"00000","01010","01010","01010","00000","00000","00000","00000","00000","00000",
	"00000","01010","01010","11111","01010","11111","01010","01010","00000","00000",
	"00000","00100","01111","10100","01110","00101","11110","00100","00000","00000",
	"00000","11000","11001","00010","00100","01000","10011","00011","00000","00000",
	"00000","01100","10010","10100","01000","10101","10010","01101","00000","00000",
	"00000","01100","00100","01000","00000","00000","00000","00000","00000","00000",
	"00000","00010","00100","01000","01000","01000","00100","00010","00000","00000",
	"00000","01000","00100","00010","00010","00010","00100","01000","00000","00000",
	"00000","00000","00100","10101","01110","10101","00100","00000","00000","00000",
	"00000","00000","00100","00100","11111","00100","00100","00000","00000","00000",
	"00000","00000","00000","00000","00000","01100","00100","01000","00000","00000",
	"00000","00000","00000","00000","11111","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","01100","01100","00000","00000",
	"00000","00000","00001","00010","00100","01000","10000","00000","00000","00000",
	"00000","01110","10001","10011","10101","11001","10001","01110","00000","00000",
	"00000","00100","01100","00100","00100","00100","00100","01110","00000","00000",
	"00000","01110","10001","00001","00010","00100","01000","11111","00000","00000",
	"00000","11111","00010","00100","00010","00001","10001","01110","00000","00000",
	"00000","00010","00110","01010","10010","11111","00010","00010","00000","00000",
	"00000","11111","10000","11110","00001","00001","10001","01110","00000","00000",
	"00000","00110","01000","10000","11110","10001","10001","01110","00000","00000",
	"00000","11111","10001","00001","00010","00100","00100","00100","00000","00000",
	"00000","01110","10001","10001","01110","10001","10001","01110","00000","00000",
	"00000","01110","10001","10001","01111","00001","00010","01100","00000","00000",
	"00000","00000","01100","01100","00000","01100","01100","00000","00000","00000",
	"00000","00000","01100","01100","00000","01100","00100","01000","00000","00000",
	"00000","00010","00100","01000","10000","01000","00100","00010","00000","00000",
	"00000","00000","00000","11111","00000","00000","11111","00000","00000","00000",
	"00000","01000","00100","00010","00001","00010","00100","01000","00000","00000",
	"00000","01110","10001","00001","00010","00100","00000","00100","00000","00000",
	"00000","01110","10001","00001","01101","10101","10101","01110","00000","00000",
	"00000","00100","01010","10001","10001","11111","10001","10001","00000","00000",
	"00000","11110","10001","10001","11110","10001","10001","11110","00000","00000",
	"00000","01110","10001","10000","10000","10000","10001","01110","00000","00000",
	"00000","11100","10010","10001","10001","10001","10010","11100","00000","00000",
	"00000","11111","10000","10000","11110","10000","10000","11111","00000","00000",
	"00000","11111","10000","10000","11110","10000","10000","10000","00000","00000",
	"00000","01110","10001","10000","10111","10001","10001","01111","00000","00000",
	"00000","10001","10001","10001","11111","10001","10001","10001","00000","00000",
	"00000","01110","00100","00100","00100","00100","00100","01110","00000","00000",
	"00000","00111","00010","00010","00010","00010","10010","01100","00000","00000",
	"00000","10001","10010","10100","11000","10100","10010","10001","00000","00000",
	"00000","10000","10000","10000","10000","10000","10000","11111","00000","00000",
	"00000","10001","11011","10101","10101","10001","10001","10001","00000","00000",
	"00000","10001","10001","11001","10101","10011","10001","10001","00000","00000",
	"00000","01110","10001","10001","10001","10001","10001","01110","00000","00000",
	"00000","11110","10001","10001","11110","10000","10000","10000","00000","00000",
	"00000","01110","10001","10001","10001","10101","10010","01101","00000","00000",
	"00000","11110","10001","10001","11110","10100","10010","10001","00000","00000",
	"00000","01110","10001","10000","01110","00001","10001","01110","00000","00000",
	"00000","11111","00100","00100","00100","00100","00100","00100","00000","00000",
	"00000","10001","10001","10001","10001","10001","10001","01110","00000","00000",
	"00000","10001","10001","10001","10001","10001","01010","00100","00000","00000",
	"00000","10001","10001","10001","10101","10101","10101","01010","00000","00000",
	"00000","10001","10001","01010","00100","01010","10001","10001","00000","00000",
	"00000","10001","10001","10001","01010","00100","00100","00100","00000","00000",
	"00000","11111","00001","00010","00100","01000","10000","11111","00000","00000",
	"00000","01110","01000","01000","01000","01000","01000","01110","00000","00000",
	"00000","00000","10000","01000","00100","00010","00001","00000","00000","00000",
	"00000","01110","00010","00010","00010","00010","00010","01110","00000","00000",
	"00000","00100","01010","10001","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","11111","00000","00000",
	"00000","01000","00100","00010","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","01110","00001","01111","10001","01111","00000","00000",
	"00000","10000","10000","10110","11001","10001","10001","11110","00000","00000",
	"00000","00000","00000","01110","10000","10000","10001","01110","00000","00000",
	"00000","00001","00001","01101","10011","10001","10001","01111","00000","00000",
	"00000","00000","00000","01110","10001","11111","10000","01110","00000","00000",
	"00000","00110","01001","01000","11100","01000","01000","01000","00000","00000",
	"00000","00000","00000","01111","10001","01111","00001","01110","00000","00000",
	"00000","10000","10000","10110","11001","10001","10001","10001","00000","00000",
	"00000","00100","00000","00100","01100","00100","00100","01110","00000","00000",
	"00000","00010","00000","00110","00010","00010","10010","01100","00000","00000",
	"00000","10000","10000","10010","10100","11000","10100","10010","00000","00000",
	"00000","01100","00100","00100","00100","00100","00100","01110","00000","00000",
	"00000","00000","00000","11010","10101","10101","10101","10101","00000","00000",
	"00000","00000","00000","10110","11001","10001","10001","10001","00000","00000",
	"00000","00000","00000","01110","10001","10001","10001","01110","00000","00000",
	"00000","00000","00000","11110","10001","11110","10000","10000","00000","00000",
	"00000","00000","00000","01101","10011","01111","00001","00001","00000","00000",
	"00000","00000","00000","10110","11001","10000","10000","10000","00000","00000",
	"00000","00000","00000","01110","10000","01110","00001","11110","00000","00000",
	"00000","01000","01000","11100","01000","01000","01001","00110","00000","00000",
	"00000","00000","00000","10001","10001","10001","10011","01101","00000","00000",
	"00000","00000","00000","10001","10001","10001","01010","00100","00000","00000",
	"00000","00000","00000","10001","10001","10101","10101","01010","00000","00000",
	"00000","00000","00000","10001","01010","00100","01010","10001","00000","00000",
	"00000","00000","00000","10001","10001","01111","00001","01110","00000","00000",
	"00000","00000","00000","11111","00010","00100","01000","11111","00000","00000",
	"00000","00010","00100","00100","01000","00100","00100","00010","00000","00000",
	"00000","00100","00100","00100","00100","00100","00100","00100","00000","00000",
	"00000","01000","00100","00100","00010","00100","00100","01000","00000","00000",
	"00000","00000","00000","00000","01101","10010","00000","00000","00000","00000",
	"00000","00100","01010","10001","10001","10001","11111","00000","00000","00000",
	"00000","11111","10001","10000","11110","10001","10001","11110","00000","00000",
	"01111","00101","00101","01001","10001","11111","10001","10001","00000","00000",
	"00000","10101","10101","10101","01110","10101","10101","10101","00000","00000",
	"00000","11110","00001","00001","00110","00001","00001","11110","00000","00000",
	"00000","10001","10001","10011","10101","11001","10001","10001","00000","00000",
	"01010","00100","10001","10001","10011","10101","11001","10001","00000","00000",
	"00000","01111","00101","00101","00101","00101","10101","01001","00000","00000",
	"00000","11111","10001","10001","10001","10001","10001","10001","00000","00000",
	"00000","10001","10001","10001","01010","00100","01000","10000","00000","00000",
	"00000","10001","10001","10001","10001","10001","11111","00001","00000","00000",
	"00000","10001","10001","10001","01111","00001","00001","00001","00000","00000",
	"00000","00000","10101","10101","10101","10101","10101","11111","00000","00000",
	"00000","10101","10101","10101","10101","10101","11111","00001","00000","00000",
	"00000","11000","01000","01000","01110","01001","01001","01110","00000","00000",
	"00000","10001","10001","10001","11001","10101","10101","11001","00000","00000",
	"00000","01110","10001","00101","01011","00001","10001","01110","00000","00000",
	"00000","00000","00000","01001","10101","10010","10010","01101","00000","00000",
	"00000","00100","00110","00101","00101","00100","11100","11100","00000","00000",
	"00000","11111","10001","10000","10000","10000","10000","10000","00000","00000",
	"00000","00000","00000","11111","01010","01010","01010","10011","00000","00000",
	"00000","11111","10000","01000","00100","01000","10000","11111","00000","00000",
	"00000","00000","00000","01111","10010","10010","10010","01100","00000","00000",
	"00110","00101","00111","00101","00101","11101","11011","00011","00000","00000",
	"00000","00000","00001","01110","10100","00100","00100","00010","00000","00000",
	"00000","00100","01110","01110","01110","11111","00100","00000","00000","00000",
	"00000","01110","10001","10001","11111","10001","10001","01110","00000","00000",
	"00000","00000","01110","10001","10001","10001","01010","11011","00000","00000",
	"00000","00110","01001","00100","01010","10001","10001","01110","00000","00000",
	"00000","00000","00000","01011","10101","11010","00000","00000","00000","00000",
	"00000","00000","01010","11111","11111","11111","01110","00100","00000","00000",
	"00000","00000","00000","01110","10000","01100","10001","01110","00000","00000",
	"00000","01110","10001","10001","10001","10001","10001","10001","00000","00000",
	"00000","11011","11011","11011","11011","11011","11011","11011","00000","00000",
	"00000","00100","00000","00000","00100","00100","00100","00100","00000","00000",
	"00000","00100","01110","10100","10100","10101","01110","00100","00000","00000",
	"00000","00110","01000","01000","11100","01000","01001","10110","00000","00000",
	"00000","00000","10001","01110","01010","01110","10001","00000","00000","00000",
	"00000","10001","01010","11111","00100","11111","00100","00100","00000","00000",
	"00000","00100","00100","00100","00000","00100","00100","00100","00000","00000",
	"00000","00110","01001","00100","01010","00100","10010","01100","00000","00000",
	"00000","00010","00101","00100","11111","00100","10100","01000","00000","00000",
	"00000","11111","10001","10101","10111","10101","10001","11111","00000","00000",
	"00000","01110","00001","01111","10001","01110","00000","11111","00000","00000",
	"00000","00000","00101","01010","10100","01010","00101","00000","00000","00000",
	"00000","10010","10101","10101","11101","10101","10101","10010","00000","00000",
	"00000","01111","10001","10001","01111","00101","01001","10001","00000","00000",
	"00000","11111","10001","10101","10001","10011","10101","11111","00000","00000",
	"00000","00100","01000","01100","00000","00000","00000","00000","00000","00000",
	"01100","10010","10010","10010","01100","00000","00000","00000","00000","00000",
	"00000","00100","00100","11111","00100","00100","00000","11111","00000","00000",
	"01100","10010","00100","01000","11110","00000","00000","00000","00000","00000",
	"11100","00010","01100","00010","11100","00000","00000","00000","00000","00000",
	"11100","10010","11100","10000","10010","10111","10010","00011","00000","00000",
	"00000","10001","10001","10001","10011","11101","10000","10000","00000","00000",
	"00000","01111","10011","10011","01111","00011","00011","00011","00000","00000",
	"00000","00000","00000","00000","01100","01100","00000","00000","00000","00000",
	"00000","00000","00000","01010","10001","10001","10101","01010","00000","00000",
	"01000","11000","01000","01000","11100","00000","00000","00000","00000","00000",
	"00000","01110","10001","10001","10001","01110","00000","11111","00000","00000",
	"00000","00000","10100","01010","00101","01010","10100","00000","00000","00000",
	"10001","10010","10100","01010","10110","01010","01111","00010","00000","00000",
	"10001","10010","10100","01010","10101","00001","00010","00111","00000","00000",
	"11000","01000","11000","01001","11011","00101","00111","00001","00000","00000",
	"00000","00100","00000","00100","01000","10000","10001","01110","00000","00000",
	"01000","00100","00100","01010","10001","11111","10001","10001","00000","00000",
	"00010","00100","00100","01010","10001","11111","10001","10001","00000","00000",
	"00100","01010","00000","01110","10001","10001","11111","10001","00000","00000",
	"01101","10010","00000","01110","10001","11111","10001","10001","00000","00000",
	"01010","00000","00100","01010","10001","11111","10001","10001","00000","00000",
	"00100","01010","00100","01110","10001","11111","10001","10001","00000","00000",
	"00000","00111","01100","10100","10111","11100","10100","10111","00000","00000",
	"01110","10001","10000","10000","10001","01110","00010","00110","00000","00000",
	"01000","00100","00000","11111","10000","11110","10000","11111","00000","00000",
	"00010","00100","00000","11111","10000","11110","10000","11111","00000","00000",
	"00100","01010","00000","11111","10000","11110","10000","11111","00000","00000",
	"00000","01010","00000","11111","10000","11110","10000","11111","00000","00000",
	"01000","00100","00000","01110","00100","00100","00100","01110","00000","00000",
	"00010","00100","00000","01110","00100","00100","00100","01110","00000","00000",
	"00100","01010","00000","01110","00100","00100","00100","01110","00000","00000",
	"00000","01010","00000","01110","00100","00100","00100","01110","00000","00000",
	"00000","01110","01001","01001","11101","01001","01001","01110","00000","00000",
	"01101","10010","00000","10001","11001","10101","10011","10001","00000","00000",
	"01000","00100","01110","10001","10001","10001","10001","01110","00000","00000",
	"00010","00100","01110","10001","10001","10001","10001","01110","00000","00000",
	"00100","01010","00000","01110","10001","10001","10001","01110","00000","00000",
	"01101","10010","00000","01110","10001","10001","10001","01110","00000","00000",
	"01010","00000","01110","10001","10001","10001","10001","01110","00000","00000",
	"00000","00000","10001","01010","00100","01010","10001","00000","00000","00000",
	"00000","01110","00100","01110","10101","01110","00100","01110","00000","00000",
	"01000","00100","10001","10001","10001","10001","10001","01110","00000","00000",
	"00010","00100","10001","10001","10001","10001","10001","01110","00000","00000",
	"00100","01010","00000","10001","10001","10001","10001","01110","00000","00000",
	"01010","00000","10001","10001","10001","10001","10001","01110","00000","00000",
	"00010","00100","10001","01010","00100","00100","00100","00100","00000","00000",
	"11000","01000","01110","01001","01001","01110","01000","11100","00000","00000",
	"00000","00110","01001","01001","01110","01001","01001","10110","00000","00000",
	"01000","00100","00000","01110","00001","01111","10001","01111","00000","00000",
	"00010","00100","00000","01110","00001","01111","10001","01111","00000","00000",
	"00100","01010","00000","01110","00001","01111","10001","01111","00000","00000",
	"01101","10010","00000","01110","00001","01111","10001","01111","00000","00000",
	"00000","01010","00000","01110","00001","01111","10001","01111","00000","00000",
	"00100","01010","00100","01110","00001","01111","10001","01111","00000","00000",
	"00000","00000","11010","00101","01111","10100","10101","01010","00000","00000",
	"00000","00000","01110","10000","10001","01110","00100","01100","00000","00000",
	"01000","00100","00000","01110","10001","11111","10000","01110","00000","00000",
	"00010","00100","00000","01110","10001","11110","10000","01110","00000","00000",
	"00100","01010","00000","01110","10001","11111","10000","01110","00000","00000",
	"00000","01010","00000","01110","10001","11111","10000","01110","00000","00000",
	"01000","00100","00000","00100","01100","00100","00100","01110","00000","00000",
	"00010","00100","00000","00100","01100","00100","00100","01110","00000","00000",
	"00100","01010","00000","00100","01100","00100","00100","01110","00000","00000",
	"00000","01010","00000","00100","01100","00100","00100","01110","00000","00000",
	"00000","10100","01000","10100","00010","01111","10001","01110","00000","00000",
	"01101","10010","00000","10110","11001","10001","10001","10001","00000","00000",
	"01000","00100","00000","01110","10001","10001","10001","01110","00000","00000",
	"00010","00100","00000","01110","10001","10001","10001","01110","00000","00000",
	"00000","00100","01010","00000","01110","10001","10001","01110","00000","00000",
	"00000","01101","10010","00000","01110","10001","10001","01110","00000","00000",
	"00000","01010","00000","01110","10001","10001","10001","01110","00000","00000",
	"00000","00000","00100","00000","11111","00000","00100","00000","00000","00000",
	"00000","00010","00100","01110","10101","01110","00100","01000","00000","00000",
	"01000","00100","00000","10001","10001","10001","10011","01101","00000","00000",
	"00010","00100","00000","10001","10001","10001","10011","01101","00000","00000",
	"00100","01010","00000","10001","10001","10001","10011","01101","00000","00000",
	"00000","01010","00000","10001","10001","10001","10011","01101","00000","00000",
	"00000","00010","00100","10001","10001","01111","00001","01110","00000","00000",
	"00000","01100","00100","00110","00101","00110","00100","01110","00000","00000",
	"00000","01010","00000","10001","10001","01111","00001","01110","00000","00000"
	};
	hd44780CharSet5x10_=&CharSet5x10[0];

	static QString CharSet5x8[]={
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"01000","01100","01110","01111","01110","01100","01000","00000",
	"00010","00110","01110","11110","01110","00110","00010","00000",
	"01001","10010","11011","00000","00000","00000","00000","00000",
	"11011","01001","10010","00000","00000","00000","00000","00000",
	"00100","01110","11111","00000","00100","01110","11111","00000",
	"11111","01110","00100","00000","11111","01110","00100","00000",
	"00000","01110","11111","11111","11111","01110","00000","00000",
	"00001","00001","00101","01001","11111","01000","00100","00000",
	"00100","01110","10101","00100","00100","00100","00100","00000",
	"00100","00100","00100","00100","10101","01110","00100","00000",
	"00000","00100","00010","11111","00010","00100","00000","00000",
	"00000","00100","01000","11111","01000","00100","00000","00000",
	"00010","00100","01000","00100","00010","00000","11111","00000",
	"01000","00100","00010","00100","01000","00000","11111","00000",
	"00000","00100","00100","01110","01110","11111","00000","00000",
	"00000","11111","01110","01110","00100","00100","00000","00000",
	"00000","00000","00000","00000","00000","00000","00000","00000",
	"00100","00100","00100","00100","00000","00000","00100","00000",
	"01010","01010","01010","00000","00000","00000","00000","00000",
	"01010","01010","11111","01010","11111","01010","01010","00000",
	"00100","01111","10100","01110","00101","11110","00100","00000",
	"11000","11001","00010","00100","01000","10011","00011","00000",
	"01100","10010","10100","01000","10101","10010","01101","00000",
	"01100","00100","01000","00000","00000","00000","00000","00000",
	"00010","00100","01000","01000","01000","00100","00010","00000",
	"01000","00100","00010","00010","00010","00100","01000","00000",
	"00000","00100","10101","01110","10101","00100","00000","00000",
	"00000","00100","00100","11111","00100","00100","00000","00000",
	"00000","00000","00000","00000","01100","00100","01000","00000",
	"00000","00000","00000","11111","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","01100","01100","00000",
	"00000","00001","00010","00100","01000","10000","00000","00000",
	"01110","10001","10011","10101","11001","10001","01110","00000",
	"00100","01100","00100","00100","00100","00100","01110","00000",
	"01110","10001","00001","00010","00100","01000","11111","00000",
	"11111","00010","00100","00010","00001","10001","01110","00000",
	"00010","00110","01010","10010","11111","00010","00010","00000",
	"11111","10000","11110","00001","00001","10001","01110","00000",
	"00110","01000","10000","11110","10001","10001","01110","00000",
	"11111","10001","00001","00010","00100","00100","00100","00000",
	"01110","10001","10001","01110","10001","10001","01110","00000",
	"01110","10001","10001","01111","00001","00010","01100","00000",
	"00000","01100","01100","00000","01100","01100","00000","00000",
	"00000","01100","01100","00000","01100","00100","01000","00000",
	"00010","00100","01000","10000","01000","00100","00010","00000",
	"00000","00000","11111","00000","00000","11111","00000","00000",
	"01000","00100","00010","00001","00010","00100","01000","00000",
	"01110","10001","00001","00010","00100","00000","00100","00000",
	"01110","10001","00001","01101","10101","10101","01110","00000",
	"00100","01010","10001","10001","11111","10001","10001","00000",
	"11110","10001","10001","11110","10001","10001","11110","00000",
	"01110","10001","10000","10000","10000","10001","01110","00000",
	"11100","10010","10001","10001","10001","10010","11100","00000",
	"11111","10000","10000","11110","10000","10000","11111","00000",
	"11111","10000","10000","11110","10000","10000","10000","00000",
	"01110","10001","10000","10111","10001","10001","01111","00000",
	"10001","10001","10001","11111","10001","10001","10001","00000",
	"01110","00100","00100","00100","00100","00100","01110","00000",
	"00111","00010","00010","00010","00010","10010","01100","00000",
	"10001","10010","10100","11000","10100","10010","10001","00000",
	"10000","10000","10000","10000","10000","10000","11111","00000",
	"10001","11011","10101","10101","10001","10001","10001","00000",
	"10001","10001","11001","10101","10011","10001","10001","00000",
	"01110","10001","10001","10001","10001","10001","01110","00000",
	"11110","10001","10001","11110","10000","10000","10000","00000",
	"01110","10001","10001","10001","10101","10010","01101","00000",
	"11110","10001","10001","11110","10100","10010","10001","00000",
	"01110","10001","10000","01110","00001","10001","01110","00000",
	"11111","00100","00100","00100","00100","00100","00100","00000",
	"10001","10001","10001","10001","10001","10001","01110","00000",
	"10001","10001","10001","10001","10001","01010","00100","00000",
	"10001","10001","10001","10101","10101","10101","01010","00000",
	"10001","10001","01010","00100","01010","10001","10001","00000",
	"10001","10001","10001","01010","00100","00100","00100","00000",
	"11111","00001","00010","00100","01000","10000","11111","00000",
	"01110","01000","01000","01000","01000","01000","01110","00000",
	"00000","10000","01000","00100","00010","00001","00000","00000",
	"01110","00010","00010","00010","00010","00010","01110","00000",
	"00100","01010","10001","00000","00000","00000","00000","00000",
	"00000","00000","00000","00000","00000","00000","11111","00000",
	"01000","00100","00010","00000","00000","00000","00000","00000",
	"00000","00000","01110","00001","01111","10001","01111","00000",
	"10000","10000","10110","11001","10001","10001","11110","00000",
	"00000","00000","01110","10000","10000","10001","01110","00000",
	"00001","00001","01101","10011","10001","10001","01111","00000",
	"00000","00000","01110","10001","11111","10000","01110","00000",
	"00110","01001","01000","11100","01000","01000","01000","00000",
	"00000","00000","01111","10001","01111","00001","01110","00000",
	"10000","10000","10110","11001","10001","10001","10001","00000",
	"00100","00000","00100","01100","00100","00100","01110","00000",
	"00010","00000","00110","00010","00010","10010","01100","00000",
	"10000","10000","10010","10100","11000","10100","10010","00000",
	"01100","00100","00100","00100","00100","00100","01110","00000",
	"00000","00000","11010","10101","10101","10101","10101","00000",
	"00000","00000","10110","11001","10001","10001","10001","00000",
	"00000","00000","01110","10001","10001","10001","01110","00000",
	"00000","00000","11110","10001","11110","10000","10000","00000",
	"00000","00000","01101","10011","01111","00001","00001","00000",
	"00000","00000","10110","11001","10000","10000","10000","00000",
	"00000","00000","01110","10000","01110","00001","11110","00000",
	"01000","01000","11100","01000","01000","01001","00110","00000",
	"00000","00000","10001","10001","10001","10011","01101","00000",
	"00000","00000","10001","10001","10001","01010","00100","00000",
	"00000","00000","10001","10001","10101","10101","01010","00000",
	"00000","00000","10001","01010","00100","01010","10001","00000",
	"00000","00000","10001","10001","01111","00001","01110","00000",
	"00000","00000","11111","00010","00100","01000","11111","00000",
	"00010","00100","00100","01000","00100","00100","00010","00000",
	"00100","00100","00100","00100","00100","00100","00100","00000",
	"01000","00100","00100","00010","00100","00100","01000","00000",
	"00000","00000","00000","01101","10010","00000","00000","00000",
	"00100","01010","10001","10001","10001","11111","00000","00000",
	"11111","10001","10000","11110","10001","10001","11110","00000",
	"01111","00101","00101","01001","10001","11111","10001","10001",
	"10101","10101","10101","01110","10101","10101","10101","00000",
	"11110","00001","00001","00110","00001","00001","11110","00000",
	"10001","10001","10011","10101","11001","10001","10001","00000",
	"01010","00100","10001","10001","10011","10101","11001","10001",
	"01111","00101","00101","00101","00101","10101","01001","00000",
	"11111","10001","10001","10001","10001","10001","10001","00000",
	"10001","10001","10001","01010","00100","01000","10000","00000",
	"10001","10001","10001","10001","10001","11111","00001","00000",
	"10001","10001","10001","01111","00001","00001","00001","00000",
	"00000","10101","10101","10101","10101","10101","11111","00000",
	"10101","10101","10101","10101","10101","11111","00001","00000",
	"11000","01000","01000","01110","01001","01001","01110","00000",
	"10001","10001","10001","11001","10101","10101","11001","00000",
	"01110","10001","00101","01011","00001","10001","01110","00000",
	"00000","00000","01001","10101","10010","10010","01101","00000",
	"00100","00110","00101","00101","00100","11100","11100","00000",
	"11111","10001","10000","10000","10000","10000","10000","00000",
	"00000","00000","11111","01010","01010","01010","10011","00000",
	"11111","10000","01000","00100","01000","10000","11111","00000",
	"00000","00000","01111","10010","10010","10010","01100","00000",
	"00110","00101","00111","00101","00101","11101","11011","00011",
	"00000","00001","01110","10100","00100","00100","00010","00000",
	"00100","01110","01110","01110","11111","00100","00000","00000",
	"01110","10001","10001","11111","10001","10001","01110","00000",
	"00000","01110","10001","10001","10001","01010","11011","00000",
	"00110","01001","00100","01010","10001","10001","01110","00000",
	"00000","00000","01011","10101","11010","00000","00000","00000",
	"00000","01010","11111","11111","11111","01110","00100","00000",
	"00000","00000","01110","10000","01100","10001","01110","00000",
	"01110","10001","10001","10001","10001","10001","10001","00000",
	"11011","11011","11011","11011","11011","11011","11011","00000",
	"00100","00000","00000","00100","00100","00100","00100","00000",
	"00100","01110","10100","10100","10101","01110","00100","00000",
	"00110","01000","01000","11100","01000","01001","10110","00000",
	"00000","10001","01110","01010","01110","10001","00000","00000",
	"10001","01010","11111","00100","11111","00100","00100","00000",
	"00100","00100","00100","00000","00100","00100","00100","00000",
	"00110","01001","00100","01010","00100","10010","01100","00000",
	"00010","00101","00100","11111","00100","10100","01000","00000",
	"11111","10001","10101","10111","10101","10001","11111","00000",
	"01110","00001","01111","10001","01110","00000","11111","00000",
	"00000","00101","01010","10100","01010","00101","00000","00000",
	"10010","10101","10101","11101","10101","10101","10010","00000",
	"01111","10001","10001","01111","00101","01001","10001","00000",
	"11111","10001","10101","10001","10011","10101","11111","00000",
	"00100","01000","01100","00000","00000","00000","00000","00000",
	"01100","10010","10010","10010","01100","00000","00000","00000",
	"00100","00100","11111","00100","00100","00000","11111","00000",
	"01100","10010","00100","01000","11110","00000","00000","00000",
	"11100","00010","01100","00010","11100","00000","00000","00000",
	"11100","10010","11100","10000","10010","10111","10010","00011",
	"10001","10001","10001","10011","11101","10000","10000","00000",
	"01111","10011","10011","01111","00011","00011","00011","00000",
	"00000","00000","00000","01100","01100","00000","00000","00000",
	"00000","00000","01010","10001","10001","10101","01010","00000",
	"01000","11000","01000","01000","11100","00000","00000","00000",
	"01110","10001","10001","10001","01110","00000","11111","00000",
	"00000","10100","01010","00101","01010","10100","00000","00000",
	"10001","10010","10100","01010","10110","01010","01111","00010",
	"10001","10010","10100","01010","10101","00001","00010","00111",
	"11000","01000","11000","01001","11011","00101","00111","00001",
	"00100","00000","00100","01000","10000","10001","01110","00000",
	"01000","00100","00100","01010","10001","11111","10001","10001",
	"00010","00100","00100","01010","10001","11111","10001","10001",
	"00100","01010","00000","01110","10001","10001","11111","10001",
	"01101","10010","00000","01110","10001","11111","10001","10001",
	"01010","00000","00100","01010","10001","11111","10001","10001",
	"00100","01010","00100","01110","10001","11111","10001","10001",
	"00111","01100","10100","10111","11100","10100","10111","00000",
	"01110","10001","10000","10000","10001","01110","00010","00110",
	"01000","00100","00000","11111","10000","11110","10000","11111",
	"00010","00100","00000","11111","10000","11110","10000","11111",
	"00100","01010","00000","11111","10000","11110","10000","11111",
	"01010","00000","11111","10000","11110","10000","11111","00000",
	"01000","00100","00000","01110","00100","00100","00100","01110",
	"00010","00100","00000","01110","00100","00100","00100","01110",
	"00100","01010","00000","01110","00100","00100","00100","01110",
	"01010","00000","01110","00100","00100","00100","01110","00000",
	"01110","01001","01001","11101","01001","01001","01110","00000",
	"01101","10010","00000","10001","11001","10101","10011","10001",
	"01000","00100","01110","10001","10001","10001","10001","01110",
	"00010","00100","01110","10001","10001","10001","10001","01110",
	"00100","01010","00000","01110","10001","10001","10001","01110",
	"01101","10010","00000","01110","10001","10001","10001","01110",
	"01010","00000","01110","10001","10001","10001","10001","01110",
	"00000","10001","01010","00100","01010","10001","00000","00000",
	"01110","00100","01110","10101","01110","00100","01110","00000",
	"01000","00100","10001","10001","10001","10001","10001","01110",
	"00010","00100","10001","10001","10001","10001","10001","01110",
	"00100","01010","00000","10001","10001","10001","10001","01110",
	"01010","00000","10001","10001","10001","10001","10001","01110",
	"00010","00100","10001","01010","00100","00100","00100","00100",
	"11000","01000","01110","01001","01001","01110","01000","11100",
	"00110","01001","01001","01110","01001","01001","10110","00000",
	"01000","00100","00000","01110","00001","01111","10001","01111",
	"00010","00100","00000","01110","00001","01111","10001","01111",
	"00100","01010","00000","01110","00001","01111","10001","01111",
	"01101","10010","00000","01110","00001","01111","10001","01111",
	"01010","00000","01110","00001","01111","10001","01111","00000",
	"00100","01010","00100","01110","00001","01111","10001","01111",
	"00000","11010","00101","01111","10100","10101","01010","00000",
	"00000","01110","10000","10001","01110","00100","01100","00000",
	"01000","00100","00000","01110","10001","11111","10000","01110",
	"00010","00100","00000","01110","10001","11110","10000","01110",
	"00100","01010","00000","01110","10001","11111","10000","01110",
	"01010","00000","01110","10001","11111","10000","01110","00000",
	"01000","00100","00000","00100","01100","00100","00100","01110",
	"00010","00100","00000","00100","01100","00100","00100","01110",
	"00100","01010","00000","00100","01100","00100","00100","01110",
	"01010","00000","00100","01100","00100","00100","01110","00000",
	"10100","01000","10100","00010","01111","10001","01110","00000",
	"01101","10010","00000","10110","11001","10001","10001","10001",
	"01000","00100","00000","01110","10001","10001","10001","01110",
	"00010","00100","00000","01110","10001","10001","10001","01110",
	"00100","01010","00000","01110","10001","10001","01110","00000",
	"01101","10010","00000","01110","10001","10001","01110","00000",
	"01010","00000","01110","10001","10001","10001","01110","00000",
	"00000","00100","00000","11111","00000","00100","00000","00000",
	"00010","00100","01110","10101","01110","00100","01000","00000",
	"01000","00100","00000","10001","10001","10001","10011","01101",
	"00010","00100","00000","10001","10001","10001","10011","01101",
	"00100","01010","00000","10001","10001","10001","10011","01101",
	"01010","00000","10001","10001","10001","10011","01101","00000",
	"00010","00100","10001","10001","01111","00001","01110","00000",
	"01100","00100","00110","00101","00110","00100","01110","00000",
	"01010","00000","10001","10001","01111","00001","01110","00000",
	};
	hd44780CharSet5x8_=&CharSet5x8[0];

}
