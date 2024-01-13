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
#include "Keypad12W.h"
#include "../../ksimu51.h"

#include <qtooltip.h>

Keypad12W::Keypad12W( QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType)
{
	componentName_+=" KEYPAD12";

	keypadQGridLayout_= new QGridLayout(centralWidget(), 4, 3, 1, 1, "displayFormLayout");

	delete delayTimeTypNumInput_; //Piilotetaan delay timen asetus
	delayTimeTypNumInput_=nullptr;
	delete delayTimeRandomNumInput_; //Piilotetaan delay timen random asetus
	delayTimeRandomNumInput_=nullptr;

	//Laitetaan pinnit addPin on kantaluokan ohjelma.
	//Input
	addInputPin(PINNAME_C0);
	addInputPin(PINNAME_C1);
	addInputPin(PINNAME_C2);
	//Output
	addOutputPin(PINNAME_R0);
	addOutputPin(PINNAME_R1);
	addOutputPin(PINNAME_R2);
	addOutputPin(PINNAME_R3);
	//Apupointterit
	pointterC0_=ioList_.pToPin(PINNAME_C0);
	pointterR0_=ioList_.pToPin(PINNAME_R0);


	//Keys/Buttons
	number1KPushButton_ = new KPushButton("1", centralWidget(), "button1");
	pushButtonPointerTable_[0]=number1KPushButton_;
	keypadQGridLayout_->addWidget(number1KPushButton_, 0, 0);
	connect(number1KPushButton_, SIGNAL(pressed()), this, SLOT(button1Pressed()));
	connect(number1KPushButton_, SIGNAL(released()), this, SLOT(button1Released()));
	//connect(number1KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button1Toggled(bool)));
	number2KPushButton_ = new KPushButton("2", centralWidget(), "button2");
	pushButtonPointerTable_[1]=number2KPushButton_;
	keypadQGridLayout_->addWidget(number2KPushButton_, 0, 1);
	connect(number2KPushButton_, SIGNAL(pressed()), this, SLOT(button2Pressed()));
	connect(number2KPushButton_, SIGNAL(released()), this, SLOT(button2Released()));
	//connect(number2KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button2Toggled(bool)));
	number3KPushButton_ = new KPushButton("3", centralWidget(), "button3");
	pushButtonPointerTable_[2]=number3KPushButton_;
	keypadQGridLayout_->addWidget(number3KPushButton_, 0, 2);
	connect(number3KPushButton_, SIGNAL(pressed()), this, SLOT(button3Pressed()));
	connect(number3KPushButton_, SIGNAL(released()), this, SLOT(button3Released()));
	//connect(number3KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button3Toggled(bool)));
	number4KPushButton_ = new KPushButton("4", centralWidget(), "button4");
	pushButtonPointerTable_[3]=number4KPushButton_;
	keypadQGridLayout_->addWidget(number4KPushButton_, 1, 0);
	connect(number4KPushButton_, SIGNAL(pressed()), this, SLOT(button4Pressed()));
	connect(number4KPushButton_, SIGNAL(released()), this, SLOT(button4Released()));
	//connect(number4KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button4Toggled(bool)));
	number5KPushButton_ = new KPushButton("5", centralWidget(), "button5");
	pushButtonPointerTable_[4]=number5KPushButton_;
	keypadQGridLayout_->addWidget(number5KPushButton_, 1, 1);
	connect(number5KPushButton_, SIGNAL(pressed()), this, SLOT(button5Pressed()));
	connect(number5KPushButton_, SIGNAL(released()), this, SLOT(button5Released()));
	//connect(number5KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button5Toggled(bool)));
	number6KPushButton_ = new KPushButton("6", centralWidget(), "button6");
	pushButtonPointerTable_[5]=number6KPushButton_;
	keypadQGridLayout_->addWidget(number6KPushButton_, 1, 2);
	connect(number6KPushButton_, SIGNAL(pressed()), this, SLOT(button6Pressed()));
	connect(number6KPushButton_, SIGNAL(released()), this, SLOT(button6Released()));
	//connect(number6KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button6Toggled(bool)));
	number7KPushButton_ = new KPushButton("7", centralWidget(), "button7");
	pushButtonPointerTable_[6]=number7KPushButton_;
	keypadQGridLayout_->addWidget(number7KPushButton_, 2, 0);
	connect(number7KPushButton_, SIGNAL(pressed()), this, SLOT(button7Pressed()));
	connect(number7KPushButton_, SIGNAL(released()), this, SLOT(button7Released()));
	//connect(number7KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button7Toggled(bool)));
	number8KPushButton_ = new KPushButton("8", centralWidget(), "button8");
	pushButtonPointerTable_[7]=number8KPushButton_;
	keypadQGridLayout_->addWidget(number8KPushButton_, 2, 1);
	connect(number8KPushButton_, SIGNAL(pressed()), this, SLOT(button8Pressed()));
	connect(number8KPushButton_, SIGNAL(released()), this, SLOT(button8Released()));
	//connect(number8KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button8Toggled(bool)));
	number9KPushButton_ = new KPushButton("9", centralWidget(), "button9");
	pushButtonPointerTable_[8]=number9KPushButton_;
	keypadQGridLayout_->addWidget(number9KPushButton_, 2, 2);
	connect(number9KPushButton_, SIGNAL(pressed()), this, SLOT(button9Pressed()));
	connect(number9KPushButton_, SIGNAL(released()), this, SLOT(button9Released()));
	//connect(number9KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button9Toggled(bool)));
	risuaitaKPushButton_ = new KPushButton("&#", centralWidget(), "button#");
	pushButtonPointerTable_[9]=risuaitaKPushButton_;
	keypadQGridLayout_->addWidget(risuaitaKPushButton_, 3, 0);
	connect(risuaitaKPushButton_, SIGNAL(pressed()), this, SLOT(risuaitaPressed()));
	connect(risuaitaKPushButton_, SIGNAL(released()), this, SLOT(risuaitaReleased()));
	//connect(risuaitaKPushButton_, SIGNAL(toggled(bool)), this, SLOT(risuaitaToggled(bool)));
	number0KPushButton_ = new KPushButton("0", centralWidget(), "button0");
	pushButtonPointerTable_[10]=number0KPushButton_;
	keypadQGridLayout_->addWidget(number0KPushButton_, 3, 1);
	connect(number0KPushButton_, SIGNAL(pressed()), this, SLOT(button0Pressed()));
	connect(number0KPushButton_, SIGNAL(released()), this, SLOT(button0Released()));
	//connect(number0KPushButton_, SIGNAL(toggled(bool)), this, SLOT(button0Toggled(bool)));
	asteriskiKPushButton_ = new KPushButton("&*", centralWidget(), "button*");
	pushButtonPointerTable_[11]=asteriskiKPushButton_;
	keypadQGridLayout_->addWidget(asteriskiKPushButton_, 3, 2);
	connect(asteriskiKPushButton_, SIGNAL(pressed()), this, SLOT(asteriskiPressed()));
	connect(asteriskiKPushButton_, SIGNAL(released()), this, SLOT(asteriskiReleased()));
	//connect(asteriskiKPushButton_, SIGNAL(toggled(bool)), this, SLOT(asteriskiToggled(bool)));

	//Key Func: toggle or tavallinen
	keyFuncQCheckBox_ = new QCheckBox(leftKToolBar_,"keyFuncQCheckBox_");
	QFont keyFuncQCheckBox_font(keyFuncQCheckBox_->font());
	keyFuncQCheckBox_font.setFamily(FONTFAMILY);
	keyFuncQCheckBox_font.setPointSize(10);
	keyFuncQCheckBox_->setFont(keyFuncQCheckBox_font);
	keyFuncQCheckBox_->setText("Toggle");
	leftKToolBar_->insertWidget(-1, 200, keyFuncQCheckBox_,-1);
	connect(keyFuncQCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(keyFuncQCheckBoxStateChanged(int)));
	QToolTip::add(keyFuncQCheckBox_,"Togglebutton");
	toggleButtons_=false;

	//Output active level
	outputLevelQCheckBox_ = new QCheckBox(rightKToolBar_,"outputLevelQCheckBox_");
	QFont outputLevelQCheckBox_font(outputLevelQCheckBox_->font());
	outputLevelQCheckBox_font.setFamily(FONTFAMILY);
	outputLevelQCheckBox_font.setPointSize(10);
	outputLevelQCheckBox_->setFont(outputLevelQCheckBox_font);
	outputLevelQCheckBox_->setText("Pull up");
	rightKToolBar_->insertWidget(-1, 200, outputLevelQCheckBox_,-1);
	connect(outputLevelQCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(outputLevelQCheckBoxStateChanged(int)));
	QToolTip::add(outputLevelQCheckBox_,"Output tied high");
	outputActiveLevelLow_=false;

	maxWSize_=QSize(inputPinKListBox_->maxItemWidth()+outputPinKListBox_->maxItemWidth()+(number1KPushButton_->width()-20)*3, number1KPushButton_->height()*4)+QSize(0,22);

	buttonOn_=false;
	buttonOff_=false;
	buttonIsOn_=false;
}

Keypad12W::~Keypad12W()
{
}

void Keypad12W::keyFuncQCheckBoxStateChanged(int s)
{
	setButtonsOff(-1);
	if(s==QButton::On)
	{
		toggleButtons_=true;
		number1KPushButton_->setToggleButton(true);
		number2KPushButton_->setToggleButton(true);
		number3KPushButton_->setToggleButton(true);
		number4KPushButton_->setToggleButton(true);
		number5KPushButton_->setToggleButton(true);
		number6KPushButton_->setToggleButton(true);
		number7KPushButton_->setToggleButton(true);
		number8KPushButton_->setToggleButton(true);
		number9KPushButton_->setToggleButton(true);
		number0KPushButton_->setToggleButton(true);
		risuaitaKPushButton_->setToggleButton(true);
		asteriskiKPushButton_->setToggleButton(true);
	}
	else
	{
		toggleButtons_=false;
		number1KPushButton_->setToggleButton(false);
		number2KPushButton_->setToggleButton(false);
		number3KPushButton_->setToggleButton(false);
		number4KPushButton_->setToggleButton(false);
		number5KPushButton_->setToggleButton(false);
		number6KPushButton_->setToggleButton(false);
		number7KPushButton_->setToggleButton(false);
		number8KPushButton_->setToggleButton(false);
		number9KPushButton_->setToggleButton(false);
		number0KPushButton_->setToggleButton(false);
		risuaitaKPushButton_->setToggleButton(false);
		asteriskiKPushButton_->setToggleButton(false);
		buttonOff_=true;
	}
	update_VirtualPart();
}

void Keypad12W::outputLevelQCheckBoxStateChanged(int s)
{
	if(s==QButton::On)
		outputActiveLevelLow_=true;
	else
		outputActiveLevelLow_=false;
	setOutputsDeactiveState();
	update();
}

//Asettaa lähtörivin deaktiiviseen tilaan
void Keypad12W::setOutputsDeactiveState(void)
{
	IOPin* apuPin=pointterR0_;
	while(apuPin!=0)
	{
		if(outputActiveLevelLow_)
			apuPin->writeValue(true, true);
		else
			apuPin->writeValue(false, true);
		apuPin=apuPin->next();
	}
}

void Keypad12W::clear(void)
{
}

double Keypad12W::execute_VirtualPart(double executeTime)
{
	double retVal=executeTime+1.0;
	if(toggleButtons_)
	{
		if(buttonOff_||buttonOn_||ioList_.ifSomeInPinChanged(true)) //Jos tuli pulssi tai jokin inpinni muuttunut
		{
			int n,m,pointerTableCounter=0;
			setOutputsDeactiveState(); //Nollataan lähdöt
			activeButtonRow_=pointterR0_;
			for(n=0; n<4; n++)
			{
				activeButtonColumn_=pointterC0_;
				for(m=0; m<3 ;m++)
				{
					if(pushButtonPointerTable_[pointerTableCounter++]->isOn())
					{
						activeButtonRow_->writeValue(activeButtonColumn_->readValue(false), true);
					}
					activeButtonColumn_=activeButtonColumn_->next();
				}
				activeButtonRow_=activeButtonRow_->next();
			}
			buttonOn_=false;
			buttonOff_=false;
			changes_=true;
			retVal=executeTime;
		}
	}
	else
	{
		if(buttonOff_||buttonOn_||ioList_.ifSomeInPinChanged(false)) //Jos tuli pulssi
		{
			setOutputsDeactiveState();      //Nollataan lähdöt
			if(buttonOff_)
			{
				buttonOff_=false;
				buttonIsOn_=false;
			}
			if(buttonOn_||(ioList_.ifSomeInPinChanged(true)&&buttonIsOn_))   //On pulssilla päälle
			{
				activeButtonRow_=pointterR0_;
				activeButtonColumn_=pointterC0_;
				//Pointterit kohalleen
				int n;
				for(n=0;n<row_;n++)
					activeButtonRow_=activeButtonRow_->next();
				for(n=0;n<column_;n++)
					activeButtonColumn_=activeButtonColumn_->next();
				buttonOn_=false;
				activeButtonRow_->writeValue(activeButtonColumn_->readValue(false), true);
				buttonIsOn_=true;
			}
			changes_=true;
			retVal=executeTime;
		}
	}
	return retVal;
}

void Keypad12W::button1Pressed(void)
{
	buttonOn_=true;
	row_=0;
	column_=0;
}

void Keypad12W::button2Pressed(void)
{
	buttonOn_=true;
	row_=0;
	column_=1;
}

void Keypad12W::button3Pressed(void)
{
	buttonOn_=true;
	row_=0;
	column_=2;
}

void Keypad12W::button4Pressed(void)
{
	buttonOn_=true;
	row_=1;
	column_=0;
}

void Keypad12W::button5Pressed(void)
{
	buttonOn_=true;
	row_=1;
	column_=1;
}

void Keypad12W::button6Pressed(void)
{
	buttonOn_=true;
	row_=1;
	column_=2;
}

void Keypad12W::button7Pressed(void)
{
	buttonOn_=true;
	row_=2;
	column_=0;
}

void Keypad12W::button8Pressed(void)
{
	buttonOn_=true;
	row_=2;
	column_=1;
}

void Keypad12W::button9Pressed(void)
{
	buttonOn_=true;
	row_=2;
	column_=2;
}

void Keypad12W::risuaitaPressed(void)
{
	buttonOn_=true;
	row_=3;
	column_=0;
}

void Keypad12W::button0Pressed(void)
{
	buttonOn_=true;
	row_=3;
	column_=1;
}

void Keypad12W::asteriskiPressed(void)
{
	buttonOn_=true;
	row_=3;
	column_=2;
}

void Keypad12W::button1Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button2Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button3Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button4Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button5Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button6Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button7Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button8Released(void)
{
	buttonOff_=true;
}

void Keypad12W::button9Released(void)
{
	buttonOff_=true;
}

void Keypad12W::risuaitaReleased(void)
{
	buttonOff_=true;
}

void Keypad12W::button0Released(void)
{
	buttonOff_=true;
}

void Keypad12W::asteriskiReleased(void)
{
	buttonOff_=true;
}

void Keypad12W::setButtonsOff(int dontChangeButton)
{
	if(toggleButtons_)
	{
		if(dontChangeButton!=BN1)
			number1KPushButton_->setOn(false);
		if(dontChangeButton!=BN2)
			number2KPushButton_->setOn(false);
		if(dontChangeButton!=BN3)
			number3KPushButton_->setOn(false);
		if(dontChangeButton!=BN4)
			number4KPushButton_->setOn(false);
		if(dontChangeButton!=BN5)
			number5KPushButton_->setOn(false);
		if(dontChangeButton!=BN6)
			number6KPushButton_->setOn(false);
		if(dontChangeButton!=BN7)
			number7KPushButton_->setOn(false);
		if(dontChangeButton!=BN8)
			number8KPushButton_->setOn(false);
		if(dontChangeButton!=BN9)
			number9KPushButton_->setOn(false);
		if(dontChangeButton!=BN0)
			number0KPushButton_->setOn(false);
		if(dontChangeButton!=BRISUAITA)
			risuaitaKPushButton_->setOn(false);
		if(dontChangeButton!=BASTERISKI)
			asteriskiKPushButton_->setOn(false);
	}
}

void Keypad12W::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);   //Talletetaan kantaluokan jutut

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)keyFuncQCheckBox_->isOn();
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)outputLevelQCheckBox_->isOn();
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number1KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number2KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number3KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number4KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number5KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number6KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)8<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number7KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)9<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number8KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)10<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number9KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)11<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)number0KPushButton_->isOn();
	saveFileStream<<(Q_UINT16)12<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)risuaitaKPushButton_->isOn();
	saveFileStream<<(Q_UINT16)13<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)asteriskiKPushButton_->isOn();
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void Keypad12W::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead); //Luetaan kantaluokan jutut

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT8 value8;
	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8;
				if(value8&&(keyFuncQCheckBox_->state()==QButton::Off))
					keyFuncQCheckBox_->toggle();
				break;
			case 1:
				readFileStream>>typeReadData>>value8;
				if(value8&&(outputLevelQCheckBox_->state()==QButton::Off))
					outputLevelQCheckBox_->toggle();
				break;
			case 2:
				readFileStream>>typeReadData>>value8;
				number1KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 3:
				readFileStream>>typeReadData>>value8;
				number2KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 4:
				readFileStream>>typeReadData>>value8;
				number3KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 5:
				readFileStream>>typeReadData>>value8;
				number4KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 6:
				readFileStream>>typeReadData>>value8;
				number5KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 7:
				readFileStream>>typeReadData>>value8;
				number6KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 8:
				readFileStream>>typeReadData>>value8;
				number7KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 9:
				readFileStream>>typeReadData>>value8;
				number8KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 10:
				readFileStream>>typeReadData>>value8;
				number9KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 11:
				readFileStream>>typeReadData>>value8;
				number0KPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 12:
				readFileStream>>typeReadData>>value8;
				risuaitaKPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case 13:
				readFileStream>>typeReadData>>value8;
				asteriskiKPushButton_->setOn((bool)value8);
				if(value8)
					buttonOn_=true;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}

