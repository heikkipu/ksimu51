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
#include "LogicGeneratorW.h"
#include "../../ksimu51.h"


LogicGeneratorW::LogicGeneratorW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType)
{
	componentName_+=" Logic Generator";
	
	delete delayTimeTypNumInput_; //Piilotetaan delay timen asetus
	delayTimeTypNumInput_=nullptr;
	delete delayTimeRandomNumInput_; //Piilotetaan delay timen random asetus
	delayTimeRandomNumInput_=nullptr;

	logicGeneratorQGridLayout_=new QGridLayout(centralWidget(), 2, 2, 2, 2, "analyzerDownQGridLoyaut" );
	//KDoubleNumInput(double lower, double upper, double value, double step=0.01, int precision=2, QWidget *  parent = 0, const char *name=0)
	highTimeKDoubleNumInput_=new KDoubleNumInput(0.0, 1.0, 1.0e-9, 1.0e-9, 9, centralWidget(),  "highTimeKDoubleNumInput");
	highTimeKDoubleNumInput_->setLabel("High t(s)",Qt::AlignLeft|Qt::AlignTop);
	logicGeneratorQGridLayout_->addWidget(highTimeKDoubleNumInput_, 0, 0);
	
	highRandomTimeKDoubleNumInput_=new KDoubleNumInput(0.0, 1.0, 0.0, 1.0e-9, 9, centralWidget(),  "highRandomTimeKDoubleNumInput");
	highRandomTimeKDoubleNumInput_->setLabel("High RND t(s)",Qt::AlignLeft|Qt::AlignTop);
	logicGeneratorQGridLayout_->addWidget(highRandomTimeKDoubleNumInput_, 0, 1);
	
	lowTimeKDoubleNumInput_=new KDoubleNumInput(0.0, 1.0, 1.0e-9, 1.0e-9, 9, centralWidget(),  "lowTimeKDoubleNumInput");
	lowTimeKDoubleNumInput_->setLabel("Low t(s)",Qt::AlignLeft|Qt::AlignTop);
	logicGeneratorQGridLayout_->addWidget(lowTimeKDoubleNumInput_, 1, 0);
	
	lowRandomTimeKDoubleNumInput_=new KDoubleNumInput(0.0, 1.0, 0.0, 1.0e-9, 9, centralWidget(),  "lowRandomTimeKDoubleNumInput");
	lowRandomTimeKDoubleNumInput_->setLabel("Low RND t(s)",Qt::AlignLeft|Qt::AlignTop);
	logicGeneratorQGridLayout_->addWidget(lowRandomTimeKDoubleNumInput_, 1, 1);
	
	addOutputPin(PINNAME_GENOUT);
	pToGenOutPin_=ioList_.pToPin(PINNAME_GENOUT);

	onOffIcon_.load("pics/KS51hi32OnOff.png", 0, KPixmap::Auto );
	
	KGuiItem kgui=KGuiItem("0", QString::null, "Set start level", QString::null);
	startLevelKPushButton_=new KPushButton(kgui, rightKToolBar_,"startLevelKPushButton");
	connect(startLevelKPushButton_, SIGNAL(clicked()), this, SLOT(startLevelKPushButtonClicked()));
	rightKToolBar_->insertWidget(-1, 20, startLevelKPushButton_,  -1);
			
	rightKToolBar_->insertLineSeparator(-1, -1);
	rightKToolBar_->insertLineSeparator(-1, -1);
	rightKToolBar_->insertLineSeparator(-1, -1);

	//On/Off
	kgui=KGuiItem("", onOffIcon_, "On/Off", QString::null);
	onOffKPushButton_ =new KPushButton(kgui, rightKToolBar_,"onOffKPushButton");
	onOffKPushButton_->setToggleButton(true);
	connect(onOffKPushButton_, SIGNAL(clicked()), this, SLOT(onOffKPushButtonClicked()));
	rightKToolBar_->insertWidget(-1, 20, onOffKPushButton_, -1);
	onOffKPushButton_->setOn(false);
	isOn_=onOffKPushButton_->isOn();
	
	startLevel_=false;
	executionStartStepTime_=0.0;
	executionReadyTime_=0.0;
	setWindowSize();
}

LogicGeneratorW::~LogicGeneratorW(void)
{
}

void LogicGeneratorW::reset_VirtualPart(void)
{
	if(!startLevel_)
		setHigh(0.0);
	else
		setLow(0.0);
	executionStartStepTime_=0.0;
	executionReadyTime_=0.0;
}

void LogicGeneratorW::update_VirtualPart(void)
{
}

double LogicGeneratorW::execute_VirtualPart(double executeTime)
{
	if(isOn_)
	{
		if(executeTime>=executionReadyTime_)
		{
			if(pToGenOutPin_->readValue(true))
				executionReadyTime_=setLow(executeTime);
			else
				executionReadyTime_=setHigh(executeTime);
			executionStartStepTime_=executeTime;
		}
		return executionReadyTime_;
	}
	return executeTime+1.0;
}

double LogicGeneratorW::setLow(double const startTime)
{
	pToGenOutPin_->writeValue(false, true);
	return startTime+lowTimeKDoubleNumInput_->value()+randomizeTime(lowRandomTimeKDoubleNumInput_->value());
}

double LogicGeneratorW::setHigh(double const startTime)
{
	pToGenOutPin_->writeValue(true, true);
	return startTime+highTimeKDoubleNumInput_->value()+randomizeTime(highRandomTimeKDoubleNumInput_->value());
}

double LogicGeneratorW::randomizeTime(double const scaleInSeconds)
{
	unsigned long nanos=scaleInSeconds*1.0e12;
	unsigned long j=(unsigned long) (nanos * (rand() / (RAND_MAX + 1.0)));
	double retTime=double(j/1.0e12);
	return retTime;
}

void LogicGeneratorW::setWindowSize(void)
{
	uint widthM=highTimeKDoubleNumInput_->frameGeometry().width()+highRandomTimeKDoubleNumInput_->frameGeometry().width()+outputPinKListBox_->maxItemWidth()+40;
	uint heightM=highTimeKDoubleNumInput_->frameGeometry().height()+lowTimeKDoubleNumInput_->frameGeometry().height()*3+4;
	maxWSize_=QSize(widthM, heightM);
	setGeometry(0, 0, 10, 10);
	setGeometry(0, 0, widthM+4, heightM);
	showMinimized();
	showNormal();
}

void LogicGeneratorW::startLevelKPushButtonClicked(void)
{
	if(startLevel_)
	{
		startLevel_=false;
		startLevelKPushButton_->setText("0");
	}
	else
	{
		startLevel_=true;
		startLevelKPushButton_->setText("1");
	}
//	reset_VirtualPart();
}

void LogicGeneratorW::onOffKPushButtonClicked(void)
{
	isOn_=onOffKPushButton_->isOn();
	if(isOn_)
		reset_VirtualPart();
}

void LogicGeneratorW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);   //Talletetaan kantaluokan jutut

	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void LogicGeneratorW::readWorkDataFromFile(QDataStream& readFileStream,
										   std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead); //Luetaan kantaluokan jutut

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


