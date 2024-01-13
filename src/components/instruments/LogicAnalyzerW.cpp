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
#include "LogicAnalyzerW.h"
#include "../../ksimu51.h"


//Class up down push button BEGIN
LeftRightClickableKPushButton::LeftRightClickableKPushButton(const KGuiItem &item, QWidget *parent, const char *name):KPushButton(item,parent,name)
{
}

LeftRightClickableKPushButton::~LeftRightClickableKPushButton()
{
}

void LeftRightClickableKPushButton::mouseReleaseEvent(QMouseEvent * me)
{
	ButtonState b=me->button();
	if(b==Qt::LeftButton)
		emit leftButtonClicked();
	if(b==Qt::RightButton)
		emit rightButtonClicked();
	KPushButton::mouseReleaseEvent(me);
}

void LeftRightClickableKPushButton::mousePressEvent(QMouseEvent * me)
{
	if(me->button()==Qt::RightButton)
		setDown(true);
	KPushButton::mousePressEvent(me);
}

//Class up down push button END

//class DataLogger BEGIN
DataLogger::DataLogger(unsigned int const  maxV)
{
	maxValues_=maxV;
	logicValueList_.push_back(false);
	listCount_=1;
}

DataLogger::~DataLogger()
{
}

void DataLogger::addValue(bool const  v)
{
	logicValueList_.push_back(v);
	listCount_++;
	if(listCount_>maxValues_)
	{
		logicValueList_.pop_front();
		listCount_--;
	}
}

_List_iterator<bool>DataLogger::begin(void)
{
	return logicValueList_.begin();
}

void DataLogger::clear(void)
{
	logicValueList_.clear();
	listCount_=0;
}
//class DataLogger END

LogicAnalyzerW::LogicAnalyzerW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType)
{
	delete delayTimeTypNumInput_; //Piilotetaan delay timen asetus
	delayTimeTypNumInput_=nullptr;
	delete delayTimeRandomNumInput_; //Piilotetaan delay timen random asetus
	delayTimeRandomNumInput_=nullptr;

	componentName_+=" Analyzer";

	analyzerQGridLayout_=new QGridLayout(centralWidget(), 2, 1, 1, -1, "displayQGridLayout");

	analyzerViewQListBox_ = new QListBox(centralWidget(), "analyzerViewQListBox_", 0);
	analyzerQGridLayout_->addWidget(analyzerViewQListBox_, 0, 0, 0);
	analyzerViewQListBox_->setHScrollBarMode(QScrollView::AlwaysOn);
	hScrollB_=analyzerViewQListBox_->horizontalScrollBar();
	connect(hScrollB_, SIGNAL(sliderReleased()), this, SLOT(hScrollBSliderReleased()));
	connect(hScrollB_, SIGNAL(nextPage()), this, SLOT(hScrollBSliderSliderPageMoved()));
	connect(hScrollB_, SIGNAL(prevPage()), this, SLOT(hScrollBSliderSliderPageMoved()));
	connect(hScrollB_, SIGNAL(nextLine()), this, SLOT(hScrollBSliderSliderPageMoved()));
	connect(hScrollB_, SIGNAL(prevLine()), this, SLOT(hScrollBSliderSliderPageMoved()));
	analyzerViewQListBox_->setMinimumWidth(20);

	updateAnalyzerViewQListBoxTimer_=new QTimer(this);
	connect(updateAnalyzerViewQListBoxTimer_, SIGNAL(timeout()), this, SLOT(updateAnalyzerViewQListBoxTimer_timeout()));

	//Laitetaan eka inputti analysaattoriin
	addInputPin("I000 ");
	pToFirstInputPin_=ioList_.pToPin("I000 ");
	timeList_=new list<double>();
	machineCyclesValueList_=new list<unsigned long>();
	programCounterValueList_=new list<unsigned long>();
	//Laitetaan myös itemi katselulistaan
	inputDataList_.append(new DataLogger(MAXLISTITEMS_));
	//ensimmäinen käppyräjana
	squareWaveViewQListBoxItem_ = new WaveViewQListBoxItem(inputPinKListBox_->itemHeight(0), &analyzerViewQListBox_Width_, timeList_, machineCyclesValueList_, programCounterValueList_, inputDataList_.last(), &timeAxelMultiplier_, &timeAxelSliderValue_, &rasterOnValue_, &pointedPointTime_, &hScrollBValue_, &startMachineCycle_);
	analyzerViewQListBox_->insertItem(squareWaveViewQListBoxItem_,-1);
	//Aikajana
	timeViewQListBoxItem_=new TimeViewQListBoxItem(inputPinKListBox_->itemHeight(0), &analyzerViewQListBox_Width_, timeList_, machineCyclesValueList_, programCounterValueList_, inputDataList_.last(), &timeAxelMultiplier_, &timeAxelSliderValue_, &rasterOnValue_, &pointedPointTime_, &hScrollBValue_, &startMachineCycle_);
	analyzerViewQListBox_->insertItem(timeViewQListBoxItem_,-1);
	//Ohjelmajana
	programViewQListBoxItem_=new ProgramViewQListBoxItem(inputPinKListBox_->itemHeight(0), &analyzerViewQListBox_Width_, timeList_, machineCyclesValueList_, programCounterValueList_, inputDataList_.last(), &timeAxelMultiplier_, &timeAxelSliderValue_, &rasterOnValue_, &pointedPointTime_, &hScrollBValue_, &startMachineCycle_);
	analyzerViewQListBox_->insertItem(programViewQListBoxItem_,-1);
	pToDataList=inputDataList_.last();

	connect(inputPinKListBox_, SIGNAL(selectionChanged()), this, SLOT(inputPinKListBoxSelectionChanged()));
	connect(inputPinKListBox_, SIGNAL(clicked(QListBoxItem *)), this, SLOT(inputPinKListBoxClicked(QListBoxItem *)));
	connect(analyzerViewQListBox_, SIGNAL(selectionChanged()), this, SLOT(analyzerViewQListBoxSelectionChanged()));
	connect(analyzerViewQListBox_, SIGNAL(clicked(QListBoxItem *, const QPoint &)), this, SLOT(analyzerViewQListBoxClicked(QListBoxItem *, const QPoint &)));
	connect(analyzerViewQListBox_, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)), this, SLOT(analyzerViewQListBoxRightButtonClicked(QListBoxItem *, const QPoint &)));
	
	clearAndSetIcon_.load("pics/KS51hi32ClearSerialList.png", 0, KPixmap::Auto );
	rasterOnIcon_.load("pics/KS51hi64RasterOn.png", 0, KPixmap::Auto );
	onOffIcon_.load("pics/KS51hi32OnOff.png", 0, KPixmap::Auto );

	bottomQGridLayout_=new QGridLayout( 1, 6, -1, "analyzerDownQGridLoyaut" );
	analyzerQGridLayout_->addLayout(bottomQGridLayout_, 1, 0);
	int bottomGridWidgetHPlace=0;
	KGuiItem kgui(QString::null, QString::null, QString::null, QString::null);
	//Error LED
	kgui=KGuiItem("3", QString::null, "Error LED", QString::null);
	analyzerViewQListBoxErrorKLed_=new KLed(Qt::red, KLed::Off, KLed::Sunken, KLed::Rectangular, centralWidget(), "analyzerViewQListBoxErrorKLed");
	bottomQGridLayout_->addWidget(analyzerViewQListBoxErrorKLed_, 0, bottomGridWidgetHPlace++, Qt::AlignAuto);
	//Update LED.
	kgui=KGuiItem("4", QString::null, "Update on LED", QString::null);
	analyzerViewQListBoxUpdateKLed_=new KLed(Qt::yellow, KLed::Off, KLed::Sunken, KLed::Rectangular, centralWidget(), "analyzerViewQListBoxUpdateKLed");
	bottomQGridLayout_->addWidget(analyzerViewQListBoxUpdateKLed_, 0, bottomGridWidgetHPlace++, Qt::AlignAuto);
	//Raster on/off
	kgui=KGuiItem("", rasterOnIcon_, "Rasters On/Off", QString::null);
	rasterOnOffKPushButton_=new KPushButton(kgui, centralWidget(),"setRasterOnOff");
	rasterOnOffKPushButton_->setToggleButton(true);
	connect(rasterOnOffKPushButton_, SIGNAL(clicked()), this, SLOT(setRasterOnOffKPushButtonClicked()));
	bottomQGridLayout_->addWidget(rasterOnOffKPushButton_, 0, bottomGridWidgetHPlace++, Qt::AlignLeft);
	rasterOnValue_=rasterOnOffKPushButton_->isOn();
	//ntime axel decade multiplier
	kgui=KGuiItem("1", QString::null, "Time decade multipler", QString::null);
	multiplierDecadeLeftRightClickableKPushButton_=new LeftRightClickableKPushButton(kgui, centralWidget(),"multiplierDecade");
	connect(multiplierDecadeLeftRightClickableKPushButton_, SIGNAL(leftButtonClicked()), this, SLOT(multiplierDecadeLeftRightClickableKPushButtonLeftButtonClicked()));
	connect(multiplierDecadeLeftRightClickableKPushButton_, SIGNAL(rightButtonClicked()), this, SLOT(multiplierDecadeLeftRightClickableKPushButtonRightButtonClicked()));
	bottomQGridLayout_->addWidget(multiplierDecadeLeftRightClickableKPushButton_, 0, bottomGridWidgetHPlace++, Qt::AlignLeft);
	//Time axel slider
	kgui=KGuiItem("2", QString::null, "Time axel slide multipler", QString::null);
	timeAxelQSlider_=new QSlider(0, TIMEAXEL_MAXVALUE_, 1, 0,Qt::Horizontal, centralWidget(), "timeAxelQSlider");
	bottomQGridLayout_->addWidget(timeAxelQSlider_, 0, bottomGridWidgetHPlace++, Qt::AlignAuto);
	connect(timeAxelQSlider_, SIGNAL(valueChanged(int)), this, SLOT(timeAxelQSliderValueChanged(int)));
	//clear
	kgui=KGuiItem("", clearAndSetIcon_, "Clear", QString::null);
	clearAndSetKPushButton_=new KPushButton(kgui, centralWidget(),"clearView");
	connect(clearAndSetKPushButton_, SIGNAL(clicked()), this, SLOT(clearAndSetKPushButtonClicked()));
	bottomQGridLayout_->addWidget(clearAndSetKPushButton_, 0, bottomGridWidgetHPlace++, Qt::AlignRight);
	//On/Off
	kgui=KGuiItem("", onOffIcon_, "On/Off", QString::null);
	onOffKPushButton_ =new KPushButton(kgui, centralWidget(),"onOffAnalsator");
	onOffKPushButton_->setToggleButton(true);
	connect(onOffKPushButton_, SIGNAL(clicked()), this, SLOT(onOffKPushButtonClicked()));
	bottomQGridLayout_->addWidget(onOffKPushButton_, 0, bottomGridWidgetHPlace++, Qt::AlignRight);
	isOn_=onOffKPushButton_->isOn();

	multiplierDecade_=3;
	multiplierDecadeLeftRightClickableKPushButtonLeftButtonClicked();
	timeAxelMultiplier_=clculateTimeAxelMultiplier();
	
	timeCount_=0.0;
	lastAddTime_=0.0;
	if(componentType>InstrumentsComponents::LOGICANALYZER40)
		type_=InstrumentsComponents::LOGICANALYZER40;
	if(componentType<InstrumentsComponents::LOGICANALYZER8)
		type_=InstrumentsComponents::LOGICANALYZER8;
	//Lisätään loput inputit: componentType 1,2,3,4,5 = inputs 8,16,24,32,40
	for(int n=1; n<componentType*8; n++)
		addAnalyzerInput();
	listCount_=0;
	clearPointedPoint();
	machineCycleTime_=.0;
	paintStart_x_=0;
	paintEnd_x_=0;
	
	progarmCounterValuesKListBox_=new KListBox(leftKToolBar_, "progarmCounterValuesKListBox_", 0);
	QFont progarmCounterValuesKListBox_font(progarmCounterValuesKListBox_->font());
	progarmCounterValuesKListBox_font.setFamily(FONTFAMILY);
	progarmCounterValuesKListBox_font.setPointSize(10);
	progarmCounterValuesKListBox_->setFont(progarmCounterValuesKListBox_font);
	progarmCounterValuesKListBox_->setFrameShape( QTable::GroupBoxPanel );
	progarmCounterValuesKListBox_->setSelectionMode(QListBox::Single);
	//progarmCounterValuesKListBox_->setMinimumHeight(inputPinKListBox_->itemHeight(0)*3+5);
	int index=leftKToolBar_->insertWidget(-1, 198, progarmCounterValuesKListBox_,-1);
	int id=leftKToolBar_->idAt(index);
	leftKToolBar_->alignItemRight(id, true);
	leftKToolBar_->setItemAutoSized(id, true);
	connect(progarmCounterValuesKListBox_, SIGNAL(pressed(QListBoxItem*)), this, SLOT(progarmCounterValuesKListBoxPressed(QListBoxItem *)));
	connect(progarmCounterValuesKListBox_, SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)), this, SLOT(progarmCounterValuesKListBoxRightButtonPressed(QListBoxItem *, const QPoint &)));
	progarmCounterValuesKListBox_CurrentItem_=0;
	
	setWindowSize();
	rasterOnValue_=false;
	hScrollBValue_=hScrollB_->value();
	updateAnalyzerViewQListBoxTwice_=false;
	reset_VirtualPart();
	
}

LogicAnalyzerW::~LogicAnalyzerW(void)
{
}

void LogicAnalyzerW::reset_VirtualPart(void)
{
	setStartTime(*pToLastStepTime_);
}

void LogicAnalyzerW::updateProgarmCounterValuesKListBox(void)
{
	if(!timeList_->empty())
	{
		progarmCounterValuesKListBox_->clear();
		uint numTimes=timeList_->size();
		list<unsigned long>::iterator machineC_it=machineCyclesValueList_->begin();
		list<unsigned long>::iterator programC_it=programCounterValueList_->begin();
		unsigned long mCycle;
		unsigned long pCounter;
		unsigned long lastMCycle=numeric_limits<unsigned long>::max();
		unsigned int cycleCount=0;
		QString ctx, txt;
		machineC_it++;
		programC_it++;
		for(uint indexC=1; indexC<numTimes; indexC++)
		{
			mCycle=*machineC_it++;
			pCounter=*programC_it++;
			if(mCycle!=lastMCycle)
			{
				if(pointedMachineCycleValue_)
					if(pointedMachineCycleValue_==mCycle)
						progarmCounterValuesKListBox_CurrentItem_=cycleCount;
				ctx.setNum(cycleCount+1, 10);
				ctx+=":";
				txt.setNum(pCounter, 16);
				ctx+=txt.upper();
				ctx+=":";
				txt.setNum(mCycle, 10);
				ctx+=txt;
				progarmCounterValuesKListBox_->insertItem(ctx);
				lastMCycle=mCycle;
				cycleCount++;
			}
		}
		progarmCounterValuesKListBox_->setCurrentItem(progarmCounterValuesKListBox_CurrentItem_);
		progarmCounterValuesKListBox_->ensureCurrentVisible();
	}
}

void LogicAnalyzerW::updateAnalyzerViewQListBox(bool const updateTwice)
{
	updateAnalyzerViewQListBoxTwice_=updateTwice;
	analyzerViewQListBoxUpdateKLed_->setState(KLed::On);
	programViewQListBoxItem_->painted(true); //Resetoidaan painttaus valmis lippu
	analyzerViewQListBox_->triggerUpdate(true);
	updateAnalyzerViewQListBoxTimer_->start(10, false);
}

//Loput updatesta tehdään täällä, kun on saatu painttaus valmiiksi.
void LogicAnalyzerW::updateAnalyzerViewQListBoxTimer_timeout(void)
{
	bool redi=programViewQListBoxItem_->painted(false);
	if(redi)
	{
		updateAnalyzerViewQListBoxTimer_->stop();
		analyzerViewQListBoxUpdateKLed_->setState(KLed::Off);
		if(pointedPointTime_)
		{
			QListBoxItem *i=analyzerViewQListBox_->firstItem();
			unsigned long pointedPointX=dynamic_cast<WaveViewQListBoxItem*>(i)->pointedPointPainted_x();
			unsigned long prevPointedPointX=dynamic_cast<WaveViewQListBoxItem*>(i)->prevPointedPointPainted_x();
			int avqlbW=analyzerViewQListBox_->width();
			if(pointedPointX)
				hScrollBValue_=pointedPointX-avqlbW/2;
			if(prevPointedPointX)
			{
				unsigned long pp, epp;
				if(prevPointedPointX>pointedPointX)
				{
					epp=prevPointedPointX-pointedPointX;
					pp=pointedPointX+epp/2;
				}
				else
				{
					epp=pointedPointX-prevPointedPointX;
					pp=prevPointedPointX+epp/2;
				}
				if(epp<(avqlbW-50))
					hScrollBValue_=pp-avqlbW/2;
			}
		}
		hScrollB_->setValue(hScrollBValue_);
		if(updateAnalyzerViewQListBoxTwice_)
			updateAnalyzerViewQListBox(false);
	}
}

void LogicAnalyzerW::update_VirtualPart(void)
{
	if(isOn_)
	{
		updateProgarmCounterValuesKListBox();
		updateAnalyzerViewQListBox(false);
	}
}

double LogicAnalyzerW::execute_VirtualPart(double executeTime)
{
	if((executeTime>*pToLastStepTime_)&&isOn_)
	{
		IOPin* apuP=pToFirstInputPin_;
		DataLogger* l=inputDataList_.first();
		if(lastAddTime_>executeTime) //resettitilanne
			lastAddTime_=0.0;
		timeCount_+=executeTime-lastAddTime_;
		timeList_->push_back(timeCount_);
		machineCyclesValueList_->push_back(*pToLastMachineCycles_);
		programCounterValueList_->push_back(*pToProgramCounterValue_);
		while(apuP!=0)
		{
			bool v=apuP->readValue(false);
			l->addValue(v);
			l=inputDataList_.next();
			apuP=apuP->next();
		}
		listCount_++;
		if(listCount_>MAXLISTITEMS_)
		{
			timeList_->pop_front();
			machineCyclesValueList_->pop_front();
			programCounterValueList_->pop_front();
			listCount_--;
		}
	}
	lastAddTime_=executeTime;
	return executeTime+1.0; //Tämä seurailee vain inputteja ei tee omiaan joten hommat on aina tehty.
}

void LogicAnalyzerW::setWindowSize(void)
{
	analyzerViewQListBox_Width_=analyzerViewQListBox_->width();
	uint widthM=inputPinKListBox_->maxItemWidth()+analyzerViewQListBox_Width_+1000;
	uint heightM=analyzerViewQListBox_->itemHeight(0)*(analyzerViewQListBox_->numRows()+2)+onOffKPushButton_->height()+15;
	maxWSize_=QSize(widthM, heightM);
	setGeometry(0, 0, 10, 10);
	setGeometry(0, 0, widthM+4, heightM);
	showMinimized();
	showNormal();
	setMinimumHeight(height());
}

void LogicAnalyzerW::addAnalyzerInput(void)
{
	QString aStr;
	int nRows=inputPinKListBox_->numRows();
	aStr.setNum(nRows,10);
	if(nRows>99)
		aStr.prepend("I");
	else
		if(nRows>9)
			aStr.prepend("I0");
		else
			aStr.prepend("I00");
	addInputPin(aStr);

	//Poistetaan aikajna ja ohjelmajana
	analyzerViewQListBox_->removeItem(analyzerViewQListBox_->count()-1);
	analyzerViewQListBox_->removeItem(analyzerViewQListBox_->count()-1);

	inputDataList_.append(new DataLogger(MAXLISTITEMS_));
	//Lisätään käppyräjan
	squareWaveViewQListBoxItem_=new WaveViewQListBoxItem(inputPinKListBox_->itemHeight(nRows), &analyzerViewQListBox_Width_, timeList_, machineCyclesValueList_, programCounterValueList_,  inputDataList_.last(), &timeAxelMultiplier_, &timeAxelSliderValue_, &rasterOnValue_, &pointedPointTime_, &hScrollBValue_, &startMachineCycle_);
	analyzerViewQListBox_->insertItem(squareWaveViewQListBoxItem_, -1);
	//Palautetaan aikajana
	timeViewQListBoxItem_=new TimeViewQListBoxItem(inputPinKListBox_->itemHeight(nRows), &analyzerViewQListBox_Width_, timeList_, machineCyclesValueList_, programCounterValueList_,  inputDataList_.last(), &timeAxelMultiplier_, &timeAxelSliderValue_, &rasterOnValue_, &pointedPointTime_, &hScrollBValue_, &startMachineCycle_);
	analyzerViewQListBox_->insertItem(timeViewQListBoxItem_, -1);
	//Palautetaan ohjelmajana
	programViewQListBoxItem_=new ProgramViewQListBoxItem(inputPinKListBox_->itemHeight(nRows), &analyzerViewQListBox_Width_, timeList_, machineCyclesValueList_, programCounterValueList_,  inputDataList_.last(), &timeAxelMultiplier_, &timeAxelSliderValue_, &rasterOnValue_, &pointedPointTime_, &hScrollBValue_, &startMachineCycle_);
	analyzerViewQListBox_->insertItem(programViewQListBoxItem_, -1);
	
	setWindowSize();
}

unsigned long long LogicAnalyzerW::clculateTimeAxelMultiplier(void)
{
	timeAxelSliderValue_=timeAxelQSlider_->value();
	unsigned long long n=pow(10.0, multiplierDecade_)*TIMEAXELMULBASE_;
	unsigned long long m=long(timeAxelSliderValue_)*(n/TIMEAXEL_MAXVALUE_)*10+n;
	return m;
}

void LogicAnalyzerW::clearPointedPoint(void)
{
	pointedPointTime_=.0;
	prevPointedPointTime_=.0;
	pointedMachineCycleValue_=0;
}

void LogicAnalyzerW::setStartTime(double const  time)
{
	timeCount_=0.0;
	listCount_=1;
	timeList_->clear();
	machineCyclesValueList_->clear();
	programCounterValueList_->clear();
	timeList_->push_back(timeCount_);
	machineCyclesValueList_->push_back(*pToLastMachineCycles_);
	programCounterValueList_->push_back(*pToProgramCounterValue_);

	IOPin* apuP=pToFirstInputPin_;
	DataLogger* l=inputDataList_.first();
	while(apuP!=0)
	{
		bool v=apuP->readValue(false);
		l->clear();
		l->addValue(v);
		l=inputDataList_.next();
		apuP=apuP->next();
	}

	clearPointedPoint();
	progarmCounterValuesKListBox_CurrentItem_=0;
	hScrollBValue_=0;
	update_VirtualPart();
}

void LogicAnalyzerW::readPaint_x_Values(void)
{
	QListBoxItem *i=analyzerViewQListBox_->firstItem();
	unsigned long cycleItemNum=progarmCounterValuesKListBox_CurrentItem_;
	machineCycleTime_=dynamic_cast<WaveViewQListBoxItem*>(i)->machineCycleTime(cycleItemNum);
	paintStart_x_=dynamic_cast<WaveViewQListBoxItem*>(i)->paintStart_x();
	paintEnd_x_=dynamic_cast<WaveViewQListBoxItem*>(i)->paintEnd_x();
}

void LogicAnalyzerW::inputPinKListBoxSelectionChanged(void)
{
	analyzerViewQListBox_->clearSelection();
}

void LogicAnalyzerW::inputPinKListBoxClicked(QListBoxItem *item)
{
}

void LogicAnalyzerW::analyzerViewQListBoxSelectionChanged(void)
{
	analyzerViewQListBox_->clearSelection();
}

void LogicAnalyzerW::analyzerViewQListBoxClicked(QListBoxItem *item, const QPoint &pnt)
{
	analyzerViewQListBoxErrorKLed_->setState(KLed::Off);
	analyzerViewQListBox_Width_=analyzerViewQListBox_->width();
	QPoint p=analyzerViewQListBox_->mapFromGlobal(pnt);
	int pointedPoint_x_=p.x()+hScrollB_->value();
	QListBoxItem *i=analyzerViewQListBox_->firstItem();
	if(dynamic_cast<WaveViewQListBoxItem*>(i)->paintEnd_x()<pointedPoint_x_)
	{
		clearPointedPoint();
		analyzerViewQListBoxErrorKLed_->setState(KLed::On);
	}
	pointedPointTime_=dynamic_cast<WaveViewQListBoxItem*>(i)->pointedXTime(pointedPoint_x_);
	if((pointedPointTime_==prevPointedPointTime_)&&pointedPointTime_&&pointedPoint_x_)
		pointedPointTime_=dynamic_cast<WaveViewQListBoxItem*>(i)->pointedXPrevTime(pointedPoint_x_);
	prevPointedPointTime_=pointedPointTime_;
	
	pointedMachineCycleValue_=dynamic_cast<WaveViewQListBoxItem*>(i)->pointedXMachineCycle(pointedPoint_x_);
	if(!pointedMachineCycleValue_)
		progarmCounterValuesKListBox_CurrentItem_=0;

	readPaint_x_Values();
	
	updateAnalyzerViewQListBox(false);
	updateProgarmCounterValuesKListBox();
	sendProgramCounterValuesKListBoxCurrentItemAddressToParent();
}

void LogicAnalyzerW::analyzerViewQListBoxRightButtonClicked(QListBoxItem *item, const QPoint &pnt)
{
	clearPointedPoint();
	updateAnalyzerViewQListBox(false);
}

void LogicAnalyzerW::sendProgramCounterValuesKListBoxCurrentItemAddressToParent(void)
{
	QString txt=progarmCounterValuesKListBox_->text(progarmCounterValuesKListBox_CurrentItem_);
	QString stx=txt.section(':', 1, 1);
	bool ok;
	unsigned int address=stx.toUInt(&ok, 16);
	emit message_FocusProgramMemoryAddress(this->name(), address);
	//emit messageToEDMW(this->name(), COMPONENT_FOCUS_PROGRAMMEMORY_ADDRESS, stx, address, 0.0);
}

void LogicAnalyzerW::progarmCounterValuesKListBoxPressed(QListBoxItem *item)
{
	if(item)
	{
		progarmCounterValuesKListBox_CurrentItem_=progarmCounterValuesKListBox_->index(item);
		sendProgramCounterValuesKListBoxCurrentItemAddressToParent();
		//QListBoxItem *i=analyzerViewQListBox_->firstItem();
		readPaint_x_Values();
		
		pointedPointTime_=machineCycleTime_;
		
		if(!progarmCounterValuesKListBox_CurrentItem_)
			hScrollBValue_=0;

		updateAnalyzerViewQListBox(true);
	}
}

void LogicAnalyzerW::progarmCounterValuesKListBoxRightButtonPressed(QListBoxItem * item, const QPoint & point)
{
	if(item)
	{
	}
}

void LogicAnalyzerW::multiplierDecadeLeftRightClickableKPushButtonLeftButtonClicked(void)
{
	multiplierDecade_++;
	if(multiplierDecade_>TIMEAXELDECADEMULTIPLIER_MAXVALUE_)
		multiplierDecade_=TIMEAXELDECADEMULTIPLIER_MAXVALUE_;
	
	QString txt;
	txt.setNum(multiplierDecade_, 10);
	multiplierDecadeLeftRightClickableKPushButton_->setText(txt);
	timeAxelMultiplier_=clculateTimeAxelMultiplier();

	updateAnalyzerViewQListBox(false);
}

void LogicAnalyzerW::multiplierDecadeLeftRightClickableKPushButtonRightButtonClicked(void)
{
	multiplierDecade_-=2;
	if(multiplierDecade_<0)
		multiplierDecade_=0;
	multiplierDecadeLeftRightClickableKPushButtonLeftButtonClicked();
}

void LogicAnalyzerW::clearAndSetKPushButtonClicked(void)
{
	setStartTime(*pToLastStepTime_);
}

void LogicAnalyzerW::setRasterOnOffKPushButtonClicked(void)
{
	rasterOnValue_=rasterOnOffKPushButton_->isOn();
	updateAnalyzerViewQListBox(false);
}

void LogicAnalyzerW::onOffKPushButtonClicked(void)
{
	isOn_=onOffKPushButton_->isOn();
}

void LogicAnalyzerW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);   //Talletetaan kantaluokan jutut

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::DOUBLE<<(double)timeCount_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::DOUBLE<<(double)lastAddTime_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void LogicAnalyzerW::readWorkDataFromFile(QDataStream& readFileStream,
										  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead); //Luetaan kantaluokan jutut

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	double doubleV;
	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>doubleV;
				timeCount_=doubleV;
				break;
			case 1:
				readFileStream>>typeReadData>>doubleV;
				lastAddTime_=doubleV;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);

	setStartTime(*pToLastStepTime_);
}

void LogicAnalyzerW::analyzerViewQListBoxOnItem(QListBoxItem * i)
{
	int xt=timeViewQListBoxItem_->width(analyzerViewQListBox_);
	int yt=analyzerViewQListBox_->height();
	int xm=analyzerViewQListBox_->width();
	analyzerViewQListBox_->center(xt-xm+xm/2,yt/2);
}

void LogicAnalyzerW::timeAxelQSliderValueChanged(int value)
{
	timeAxelMultiplier_=clculateTimeAxelMultiplier();
	updateAnalyzerViewQListBox(false);
}

void LogicAnalyzerW::hScrollBSliderSliderPageMoved(void)
{
	hScrollBValue_=hScrollB_->value();
}

void LogicAnalyzerW::hScrollBSliderReleased(void)
{
	hScrollBValue_=hScrollB_->value();
}

