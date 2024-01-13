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
#include "ExternalComponentBaseW.h"
#include <qtooltip.h>
#include "../ksimu51.h"


//Pelkkä IO tyyppinen komponentti
ExternalComponentBaseW::ExternalComponentBaseW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType):ShareMainW(parent, name, wflags)
{
	atIOSpace_=true;

	atExternalDataSpace_=false;
	externalDataSpaceValueT_=new externalDataMemoryTable(1); //Vähintään yhden alkion muistisolu

	atExternalCodeSpace_=false;
	externalCodeSpaceValueT_=new externalDataMemoryTable(1);

	constructorsCommon(componentNumber, componentType);
}

//Ext DataArea. Can have IO too only adding ioPin in constructor.
ExternalComponentBaseW::ExternalComponentBaseW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType, unsigned int const  externalDataSize):ShareMainW(parent, name, wflags)
{
	atIOSpace_=false;

	atExternalCodeSpace_=false;
	externalCodeSpaceValueT_=new externalDataMemoryTable(1);

	externalDataSpaceValueT_=new externalDataMemoryTable(externalDataSize);
	atExternalDataSpace_=true;

	constructorsCommon(componentNumber, componentType);
}

//Ext CodeArea OR Both Data & Code. Can have IO too only adding ioPin in constructor.
ExternalComponentBaseW::ExternalComponentBaseW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType, unsigned int const  externalDataSize, unsigned int const  externalCodeSize):ShareMainW(parent, name, wflags)
{
	atIOSpace_=false;

	if(!externalDataSize)
	{
		atExternalDataSpace_=false;
		externalDataSpaceValueT_=new externalDataMemoryTable(1); //Vähintään yhden alkion muistisolu
	}
	else
	{
		externalDataSpaceValueT_=new externalDataMemoryTable(externalDataSize);
		atExternalDataSpace_=true;
	}

	atExternalCodeSpace_=true;
	externalCodeSpaceValueT_=new externalDataMemoryTable(externalCodeSize);

	constructorsCommon(componentNumber, componentType);
}

//Konstruktorien yhteiset alustukset
void ExternalComponentBaseW::constructorsCommon(unsigned int const  componentNumber, unsigned int const  componentType)
{
	type_=componentType;
	number_=componentNumber;
	componentName_.setNum(componentNumber,10);
	componentName_+=(":");

	clearWFlags(KMainWindow::WStyle_MinMax);
	setFocusPolicy(QWidget::StrongFocus);

	//Connectiot external ikkunaan(käyttäjäluokkaan). Siellä pitää määrittää SLOT funktiot.
	connect(this, SIGNAL(message_Created(const QString& )), this->parent()->parent()->parent(), SLOT(messageFromComponent_Created(const QString& )));
	connect(this, SIGNAL(message_Destroyed(const QString& )), this->parent()->parent()->parent(), SLOT(messageFromComponent_Destroyed(const QString& )));
	connect(this, SIGNAL(message_PinConnect(const QString&, const int, const QString& )), this->parent()->parent()->parent(), SLOT(messageFromComponent_PinConnect(const QString&, const int, const QString& )));
	connect(this, SIGNAL(message_PinUnconnect(const QString& )), this->parent()->parent()->parent(), SLOT(messageFromComponent_PinUnconnect(const QString& )));
	connect(this, SIGNAL(message_PinConnected(const QString&, const int , const QString& )), this->parent()->parent()->parent(), SLOT(messageFromComponent_PinConnected(const QString&, const int , const QString& )));
	connect(this, SIGNAL(message_PinUnconnected(const QString&, const int , const QString& )), this->parent()->parent()->parent(), SLOT(messageFromComponent_PinUnconnected(const QString&, const int , const QString& )));
	connect(this, SIGNAL(message_PinCopy(const QString&, const int )), this->parent()->parent()->parent(), SLOT(messageFromComponent_PinCopy(const QString&, const int )));
	connect(this, SIGNAL(message_MemoryValueChanged(const QString&, const unsigned long, const int)), this->parent()->parent()->parent(), SLOT(messageFromComponent_MemoryValueChanged(const QString&, const unsigned long, const int)));
	connect(this, SIGNAL(message_FocusProgramMemoryAddress(const QString&, const unsigned long )), this->parent()->parent()->parent(), SLOT(messageFromComponent_FocusProgramMemoryAddress(const QString&, const unsigned long )));
	
	//Input/bidirLeft toolbaari
	QString iopinTooltip("If connection allowed:\nLeft clic, connect.\nRight click, unconnect.");
	leftKToolBar_ = new KToolBar( this, "leftKToolBar_" );
	QToolTip::add(leftKToolBar_,iopinTooltip);
	addToolBar( leftKToolBar_, i18n( "inputPins" ), DockLeft, FALSE );
	leftKToolBar_->setMovingEnabled(false);
	leftKToolBar_->hide();

	//Output/BidirRight toolbaari
	rightKToolBar_ = new KToolBar( this, "rightKToolBar_" );
	QToolTip::add(rightKToolBar_,iopinTooltip);
	addToolBar( rightKToolBar_, i18n( "outputPins" ), DockRight, FALSE );
	rightKToolBar_->setMovingEnabled(false);
	rightKToolBar_->hide();

	//Delay time inputti
	delayTimeTypNumInput_=new KDoubleNumInput(rightKToolBar_);
	rightKToolBar_->setWidget(delayTimeTypNumInput_);
	delayTimeTypNumInput_->setRange(0.01, 999.9, .1, false);
	delayTimeTypNumInput_->setSuffix("ns");
	delayTimeTypNumInput_->setPrefix("Ttpd:");
	connect(delayTimeTypNumInput_, SIGNAL(valueChanged(double)), this, SLOT(delayTimeTypValueChanged(double)));

	//Delay time random inputti
	delayTimeRandomNumInput_=new KDoubleNumInput(rightKToolBar_);
	rightKToolBar_->setWidget(delayTimeRandomNumInput_);
	delayTimeRandomNumInput_->setRange(0.0, 999.9, .1, false);
	delayTimeRandomNumInput_->setSuffix("ns");
	delayTimeRandomNumInput_->setPrefix("Rtpd:");
	delayTimeRandomNumInput_->setValue(5.0); //0-5ns
	connect(delayTimeRandomNumInput_, SIGNAL(valueChanged(double)), this, SLOT(delayTimeRandomValueChanged(double)));

	pToSomeDatapointerValueChanged_=new bool(); *pToSomeDatapointerValueChanged_=false;
	pToDPTRValue_=new unsigned int(); *pToDPTRValue_=0;
	pToR0Value_=new unsigned int(); *pToR0Value_=0;
	pToR1Value_=new unsigned int(); *pToR1Value_=0;
	pToExternalCycle_=new ExternalCycle();
	pToLastStepTime_=new double(); *pToLastStepTime_=0.0;
	pToLastMachineCycles_=new I8031_NameS::machineCycles_Type(); *pToLastMachineCycles_=0;
	pToProgramCounterValue_=new I8031_NameS::programCounter_Type; *pToProgramCounterValue_=0;
	
	externalDataSpaceStartAddress_=0;
	externalCodeSpaceStartAddress_=0;
	clearDatapointers();

	dptrMark_.load("pics/KS51hi8x16DPTRMark.png", 0, KPixmap::Auto);
	r0Mark_.load("pics/KS51hi8x16R0Mark.png", 0, KPixmap::Auto);
	r1Mark_.load("pics/KS51hi8x16R1Mark.png", 0, KPixmap::Auto);
	dptrR0Mark_.load("pics/KS51hi8x16DPTRR0Mark.png", 0, KPixmap::Auto);
	dptrR1Mark_.load("pics/KS51hi8x16DPTRR1Mark.png", 0, KPixmap::Auto);
	r0r1Mark_.load("pics/KS51hi8x16R0R1Mark.png", 0, KPixmap::Auto);
	dptrR0R1Mark_.load("pics/KS51hi8x16DPTRR0R1Mark.png", 0, KPixmap::Auto);

	inputLedOnIcon_.load("pics/KS51hi16BlueLedOn.png", 0, KPixmap::Auto);
	inputLedOffIcon_.load("pics/KS51hi16BlueLedOff.png", 0, KPixmap::Auto);
	outputLedOnIcon_.load("pics/KS51hi16RedLedOn.png", 0, KPixmap::Auto);
	outputLedOffIcon_.load("pics/KS51hi16RedLedOff.png", 0, KPixmap::Auto);
	hizLedIcon_.load("pics/KS51hi16BlackLedOn.png", 0, KPixmap::Auto);

	setCentralWidget(new QWidget( this, "externalComponentCentralWidget" ));

	emit message_Created(this->name());

	inputPinCounter_=0;
	outputPinCounter_=0;
	bidirPinCounterInLeftKToolBar_=0;
	bidirPinCounterInRightKToolBar_=0;
	IOPin aPin;
	pinToConnectUnconnectID_=aPin.NO_ID(); //Ei kytketty pinnilabeli

	//Liittyy päivityksiin.
	memoryAreaChanged_=false;
	changes_=false;
	isExecuting_=false;

	firsTimeToExecute_=true;

	executionStartStepTime_=0.0;
	executionReadyTime_=0.0;
	delayTimeTyp_=DEFAULT_DELAYTIME_TYP; //Oletusviive
	delayTimeRandom_=0.0; //Oletuksena ei lisäviivettä
}

ExternalComponentBaseW::~ExternalComponentBaseW()
{
	//Unkonnektoidaan kytkennät:
	if(inputPinCounter_)
	{
		for(int n=0; n<inputPinKListBox_->numRows(); n++)
		{
			pinToConnectUnconnectID_.id_=INPUTPIN_ID_OFFSET+n;
			unconnectPin();
		}
	}
	if(outputPinCounter_)
	{
		for(int n=0; n<outputPinKListBox_->numRows(); n++)
		{
			pinToConnectUnconnectID_.id_=OUTPUTPIN_ID_OFFSET+n;
			unconnectPin();
		}
	}
	if(bidirPinCounterInLeftKToolBar_)
	{
		for(int n=0; n<bidirPinKListBoxInLeftKToolBar_->numRows(); n++)
		{
			pinToConnectUnconnectID_.id_=BIDIRPIN_ID_OFFSET_LEFT+n;
			unconnectPin();
		}
	}
	if(bidirPinCounterInRightKToolBar_)
	{
		for(int n=0; n<bidirPinKListBoxInRightKToolBar_->numRows(); n++)
		{
			pinToConnectUnconnectID_.id_=BIDIRPIN_ID_OFFSET_RIGHT+n;
			unconnectPin();
		}
	}
}


void ExternalComponentBaseW::setPlainCaption( const QString &caption )
{
	QString curr_caption = caption;
	int l = curr_caption.find( " [" ); //' [' pitää olla nimessä viimeisenä, niin laitetaan kokonaisviive captiooniin..
	if( l > 1 )
	{
		QString tpdFull_txt;
		tpdFull_txt.setNum(delayTime_*1000000000.0, 'f', 2); //ns
		curr_caption.replace(l, 14, " ["+tpdFull_txt+"ns]");
	}
	KMainWindow::setPlainCaption(curr_caption);
}


//Clearaa kaiken; uudelleenmääriteltävissä.
void ExternalComponentBaseW::clear(void)
{
	clearExternalDataSpace();
	clearDatapointers();
	clearExternalCodeSpace();
}

void ExternalComponentBaseW::reset(void)
{
	firsTimeToExecute_=true;
	reset_VirtualPart();
}

//Muistiavaruus -----------------------------------------------------------------------------------
//DATA
//Luetaan muistiavaruuden koko
unsigned long ExternalComponentBaseW::readExternalDataSpaceSize(void)
{
	return externalDataSpaceValueT_->readSize();
}

void ExternalComponentBaseW::clearExternalDataSpace(void)
{
	externalDataSpaceValueT_->clear();
}

//Asetaetaan arvo muuttaen change bitti.
void ExternalComponentBaseW::setExternalDataSpaceValue(unsigned int const  address, unsigned char const  value)
{
	if((address>=externalDataSpaceStartAddress_) && (address<(externalDataSpaceValueT_->readSize()+externalDataSpaceStartAddress_)))
	{
		externalDataSpaceValueT_->setValue(address-externalDataSpaceStartAddress_, value);
		memoryAreaChanged_=true;
	}
}

//Kirjoitetaan arvo muuttamatta change bittiä.
void ExternalComponentBaseW::writeExternalDataSpaceValue(unsigned int const  address, unsigned char const  value, bool const  changedAfterClearBit)
{
	if((address>=externalDataSpaceStartAddress_) && (address<(externalDataSpaceValueT_->readSize()+externalDataSpaceStartAddress_)))
	{
		externalDataSpaceValueT_->writeValue(address-externalDataSpaceStartAddress_, value, changedAfterClearBit);
		memoryAreaChanged_=true;
	}
}

//Voidaan luke onko arvo muuttunut, jos oli kirjoitettu setValue() -funktiolla.
bool ExternalComponentBaseW::isExternalDataSpaceValueChanged(unsigned int const  address)
{
	if((address>=externalDataSpaceStartAddress_) && (address<(externalDataSpaceValueT_->readSize()+externalDataSpaceStartAddress_)))
		return externalDataSpaceValueT_->isValueChanged(address-externalDataSpaceStartAddress_);
	else
		return false;
}

//Voidaan luke onko arvo muuttunut, jos oli kirjoitettu setValue() -funktiolla.
//Antaa muuttuneen minimi ja maksimiosoitteen.
bool ExternalComponentBaseW::isExternalDataSpaceValueChanged(unsigned int& minAddress, unsigned int& maxAddress)
{
	unsigned int retMinA, retMaxA;
	bool retVal=externalDataSpaceValueT_->isValueChanged(retMinA, retMaxA);
	minAddress=retMinA+externalDataSpaceStartAddress_;
	maxAddress=retMaxA+externalDataSpaceStartAddress_;
	return retVal;
}

//Luetaan arvo.
unsigned char ExternalComponentBaseW::readExternalDataSpaceValue(unsigned int const  address)
{
	if((address>=externalDataSpaceStartAddress_) && (address<(externalDataSpaceValueT_->readSize()+externalDataSpaceStartAddress_)))
		return externalDataSpaceValueT_->readValue(address-externalDataSpaceStartAddress_);
	else
		return 0;
}

void ExternalComponentBaseW::clearDatapointers(void)
{
	isDPTR_=false;
	isR0_=false;
	isR1_=false;
}

//Laittaa ne datapointterit päälle joide arvo sattuu
void ExternalComponentBaseW::setDatapointers(void)
{
	unsigned long size=externalDataSpaceValueT_->readSize();
	if((*pToDPTRValue_>=externalDataSpaceStartAddress_) && (*pToDPTRValue_<(externalDataSpaceStartAddress_+size)))
		isDPTR_=true;
	else
		isDPTR_=false;

	if((*pToR0Value_>=externalDataSpaceStartAddress_) && (*pToR0Value_<(externalDataSpaceStartAddress_+size)))
		isR0_=true;
	else
		isR0_=false;

	if((*pToR1Value_>=externalDataSpaceStartAddress_) && (*pToR1Value_<(externalDataSpaceStartAddress_+size)))
		isR1_=true;
	else
		isR1_=false;
}

void ExternalComponentBaseW::writePointers(ExternalCycle* pToExternalCycle, bool* pToSomeDatapointerValueChanged, unsigned int* pToDPTRValue, unsigned int* pToR0Value, unsigned int* pToR1Value, double* pToLastStepTime, I8031_NameS::machineCycles_Type *pToLastMachineCycles, I8031_NameS::programCounter_Type *pToProgramCounterValue)
//void ExternalComponentBaseW::writePointers(bool* datapointersValueChanged, unsigned int* dptr, unsigned int* r0, unsigned int* r1, double* lastStepT, unsigned long* lastMachineCs)
{
	//deletoidaan ensin konstruktorissa varatut
	delete pToSomeDatapointerValueChanged_;
	delete pToDPTRValue_;
	delete pToR0Value_;
	delete pToR1Value_;
	delete pToExternalCycle_; //Laitetaan pointteri external Cycleen
	delete pToLastStepTime_;
	delete pToLastMachineCycles_;
	delete pToProgramCounterValue_;
	//Uudet osoittimet
	pToSomeDatapointerValueChanged_=pToSomeDatapointerValueChanged;
	pToDPTRValue_=pToDPTRValue;
	pToR0Value_=pToR0Value;
	pToR1Value_=pToR1Value;
	pToExternalCycle_=pToExternalCycle; //Laitetaan pointteri external Cycleen
	pToLastStepTime_=pToLastStepTime;
	pToLastMachineCycles_=pToLastMachineCycles;
	pToProgramCounterValue_=pToProgramCounterValue;
}

//Voidaan tuoda esiin se komponentti johon pointteri osoittaa.
void ExternalComponentBaseW::showPointerDPTR(void)
{
	if(isDPTR_)
	{
		setFocus();
	}
}

void ExternalComponentBaseW::showPointerR0(void)
{
	if(isR0_)
	{
		setFocus();
	}
}

void ExternalComponentBaseW::showPointerR1(void)
{
	if(isR1_)
	{
		setFocus();
	}
}

void ExternalComponentBaseW::updateComponent(bool const  updateIO)
{
	if(atIOSpace_&&updateIO)
	{
		//Haetaan IO pinnilistojen highlighted itemit
		int inputPinListCurrentItem, bidirPinListCurrentItemInLeftKToolBar, bidirPinListCurrentItemInRightKToolBar, outputPinListCurrentItem;
		if(inputPinCounter_)
			inputPinListCurrentItem=inputPinKListBox_->currentItem();
		if(outputPinCounter_)
			outputPinListCurrentItem=outputPinKListBox_->currentItem();
		if(bidirPinCounterInLeftKToolBar_)
			bidirPinListCurrentItemInLeftKToolBar=bidirPinKListBoxInLeftKToolBar_->currentItem();
		if(bidirPinCounterInRightKToolBar_)
			bidirPinListCurrentItemInRightKToolBar=bidirPinKListBoxInRightKToolBar_->currentItem();

		//Päivitetään IO ledit
		IOPin* apuPin=ioList_.pToFirstPin();
		int inputPinListBoxIndex=0;
		int outputPinListBoxIndex=0;
		int bidirPinListBoxIndexInLeftKToolBar=0;
		int bidirPinListBoxIndexInRightKToolBar=0;
		while(apuPin!=0)
		{
			KPixmap pix;
			if(!apuPin->isInHizState())
			{
				if(apuPin->isIn())
					pix=inputLedOffIcon_;
				else
					pix=outputLedOffIcon_;
				if(apuPin->readValue(false))
				{
					if(apuPin->isIn())
						pix=inputLedOnIcon_;
					else
						pix=outputLedOnIcon_;
				}
			}
			else
			{
				pix=hizLedIcon_;
			}
			QString text;
			bool left;
			if(apuPin->isIn() && !apuPin->isOut() && !apuPin->isBidir(left))
			{
				text=inputPinKListBox_->text(inputPinListBoxIndex);
				inputPinKListBox_->changeItem(pix, text, inputPinListBoxIndex);
				inputPinListBoxIndex++;
			}
			if(apuPin->isOut() && !apuPin->isIn() && !apuPin->isBidir(left))
			{
				text=outputPinKListBox_->text(outputPinListBoxIndex);
				outputPinKListBox_->changeItem(pix, text, outputPinListBoxIndex);
				outputPinListBoxIndex++;
			}
			if(apuPin->isBidir(left))
			{
				if(left)//Left
				{
					text=bidirPinKListBoxInLeftKToolBar_->text(bidirPinListBoxIndexInLeftKToolBar);
					bidirPinKListBoxInLeftKToolBar_->changeItem(pix, text, bidirPinListBoxIndexInLeftKToolBar);
					bidirPinListBoxIndexInLeftKToolBar++;
				}
				else //Right
				{
					text=bidirPinKListBoxInRightKToolBar_->text(bidirPinListBoxIndexInRightKToolBar);
					bidirPinKListBoxInRightKToolBar_->changeItem(pix, text, bidirPinListBoxIndexInRightKToolBar);
					bidirPinListBoxIndexInRightKToolBar++;
				}
			}
			apuPin=apuPin->next();
		}

		//Palutetaan fokusoidut itemit
		if(inputPinCounter_)
		{
			inputPinKListBox_->setCurrentItem(inputPinListCurrentItem);
			inputPinKListBox_->ensureCurrentVisible();
		}
		if(outputPinCounter_)
		{
			outputPinKListBox_->setCurrentItem(outputPinListCurrentItem);
			outputPinKListBox_->ensureCurrentVisible();
		}
		if(bidirPinCounterInLeftKToolBar_)
		{
			bidirPinKListBoxInLeftKToolBar_->setCurrentItem(bidirPinListCurrentItemInLeftKToolBar);
			bidirPinKListBoxInLeftKToolBar_->ensureCurrentVisible();
		}
		if(bidirPinCounterInRightKToolBar_)
		{
			bidirPinKListBoxInRightKToolBar_->setCurrentItem(bidirPinListCurrentItemInRightKToolBar);
			bidirPinKListBoxInRightKToolBar_->ensureCurrentVisible();
		}
	}

	if(atExternalDataSpace_)
	{
		setDatapointers();
		updateDatapointers();
	}

	update_VirtualPart(); //Tehdään perivän luokan virtuaaliosa.
}

bool ExternalComponentBaseW::hasChanges(void)
{
	bool retVal=changes_;
	changes_=false;
	return retVal;
}

double ExternalComponentBaseW::executeComponent(double executeTime)
{
	double nextTime=execute_VirtualPart(executeTime);   //Tehdään virtuaaliosa
	if(atExternalDataSpace_ && (memoryAreaChanged_ || *pToSomeDatapointerValueChanged_))
	{
		memoryAreaChanged_=false;
		changes_=true;
	}
	if(atIOSpace_ && ioList_.ifSomeInPinChanged(false))
	{
		changes_=true;
	}
	return nextTime; //Paluttaa seuraavan stepTimen jolloin suoritus tehty.
	//Jos arvo>=executeTime+1.0 ei muutoksia enään tässä prosesorin stepissä: pitää muistaa antaa virtuaaliosassa.
}

double ExternalComponentBaseW::execute_VirtualPart_ExecuteInputChange(double executeTime)//Standardi toiminto inputin muutoksen mukaan toimimiseen. Käytä execute_VirtualPartissa
{
	double retVal=executeTime+1.0;
	if(ioList_.ifSomeInPinChanged(true)||firsTimeToExecute_)
	{
		executionStartStepTime_=executeTime;
		executionReadyTime_=executeTime+delayTime_;
		isExecuting_=true;
		changes_=true;
		firsTimeToExecute_=false;
	}
	if(isExecuting_&&(executeTime>=executionReadyTime_))
	{
		execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart();//Määritettävä perivässä luokassa, jos käyttää tätä yleis inputin tutkailua

		changes_=true;
		isExecuting_=false;
	}
	if(isExecuting_)
		retVal=executionReadyTime_;

	return retVal;
}

//Code space------------------------------
void ExternalComponentBaseW::clearExternalCodeSpace(void)
{
	externalCodeSpaceValueT_->clear();
}

unsigned long ExternalComponentBaseW::readExternalCodeSpaceSize(void)
{
	return externalCodeSpaceValueT_->readSize();
}

//Asetaetaan arvo muuttaen change bitti.
void ExternalComponentBaseW::setExternalCodeSpaceValue(unsigned int const  address, unsigned char const  value)
{
	if((address>=externalCodeSpaceStartAddress_) && (address<(externalCodeSpaceValueT_->readSize()+externalCodeSpaceStartAddress_)))
	{
		externalCodeSpaceValueT_->setValue(address-externalCodeSpaceStartAddress_, value);
	}
}

//Voidaan luke onko arvo muuttunut, jos oli kirjoitettu setValue() -funktiolla.
bool ExternalComponentBaseW::isExternalCodeSpaceValueChanged(unsigned int const  address)
{
	if((address>=externalCodeSpaceStartAddress_) && (address<(externalCodeSpaceValueT_->readSize()+externalCodeSpaceStartAddress_)))
		return externalCodeSpaceValueT_->isValueChanged(address-externalCodeSpaceStartAddress_);
	else
		return false;
}

//Voidaan lukea onko arvo muuttunut, jos oli kirjoitettu setValue() -funktiolla.
//Antaa muuttuneen minimi ja maksimiosoitteen.
bool ExternalComponentBaseW::isExternalCodeSpaceValueChanged(unsigned int& minAddress, unsigned int& maxAddress)
{
	unsigned int retMinA, retMaxA;
	bool retVal=externalCodeSpaceValueT_->isValueChanged(retMinA, retMaxA);
	minAddress=retMinA+externalCodeSpaceStartAddress_;
	maxAddress=retMaxA+externalCodeSpaceStartAddress_;
	return retVal;
}

//Luetaan arvo.
unsigned char ExternalComponentBaseW::readExternalCodeSpaceValue(unsigned int const  address)
{
	if((address>=externalCodeSpaceStartAddress_) && (address<(externalCodeSpaceValueT_->readSize()+externalCodeSpaceStartAddress_)))
		return externalCodeSpaceValueT_->readValue(address-externalCodeSpaceStartAddress_);
	else
		return 0;
}

//IO Pinniavaruus----------------------------------------------------------------------------------------------
//Lisätään pinni. Perivä luokka käyttää.
void ExternalComponentBaseW::addInputPin(char const *pinName)
{
	addPin(pinName, IOPin::PINTYPE_INPUT);
}

void ExternalComponentBaseW::addOutputPin(char const *pinName)
{
	addPin(pinName, IOPin::PINTYPE_OUTPUT);
}

void ExternalComponentBaseW::addBidirPinInLeftKToolBar(char const *pinName)
{
	addPin(pinName, IOPin::PINTYPE_BIDIR_LEFT);
}

void ExternalComponentBaseW::addBidirPinInRightKToolBar(char const *pinName)
{
	addPin(pinName, IOPin::PINTYPE_BIDIR_RIGHT);
}

void ExternalComponentBaseW::addPin(char const *pinName, IOPin::PinTYPE const pinType)
{
	int id, index;
	QString ioPinName(pinName);
	IOPin aPin;
	ioList_.addPin(aPin.NO_ID(), pinName, false, pinType); //Id:tä muutetaan kytkettäessä.
	if(pinType==IOPin::PINTYPE_INPUT)
	{
		if(!inputPinCounter_&&!bidirPinCounterInLeftKToolBar_)
		{
			leftKToolBar_->show();
		}
		if(!inputPinCounter_)
		{
			inputPinKListBox_ = new KListBox(leftKToolBar_, "inputPinKListBox_", 0);
			QFont inputPinKListBox_font(inputPinKListBox_->font());
			inputPinKListBox_font.setFamily(FONTFAMILY);
			inputPinKListBox_font.setPointSize(10);
			inputPinKListBox_->setFont(inputPinKListBox_font);
			inputPinKListBox_->setFrameShape( QTable::GroupBoxPanel );
			inputPinKListBox_->setSelectionMode(QListBox::Multi);
			inputPinKListBox_->setBottomScrollBar(false);
			index=leftKToolBar_->insertWidget(-1, 200, inputPinKListBox_,-1);
			id=leftKToolBar_->idAt(index);
			leftKToolBar_->setItemAutoSized(id, true);

			connect(inputPinKListBox_, SIGNAL(mouseButtonClicked(int, QListBoxItem*, const QPoint&)), this, SLOT(inputPinListMouseButtonClicked(int, QListBoxItem*, const QPoint&)));
		}
		ioPinName.prepend("xxxx>");
		inputPinKListBox_->insertItem(inputLedOffIcon_, ioPinName, -1);
		inputPinKListBox_->setMinimumWidth(inputPinKListBox_->maxItemWidth());
		inputPinKListBox_->setFixedHeight(inputPinKListBox_->itemHeight(0)*inputPinKListBox_->numRows()+4);
		inputPinCounter_++;
	}
	if(pinType==IOPin::PINTYPE_OUTPUT)
	{
		if(!outputPinCounter_&&!bidirPinCounterInRightKToolBar_)
		{
			rightKToolBar_->show();

			outputPinKListBox_ = new KListBox(rightKToolBar_, "outputPinKListBox_", 0);
			QFont outputPinKListBox_font(outputPinKListBox_->font());
			outputPinKListBox_font.setFamily(FONTFAMILY);
			outputPinKListBox_font.setPointSize(10);
			outputPinKListBox_->setFont(outputPinKListBox_font);
			outputPinKListBox_->setFrameShape( QTable::GroupBoxPanel );
			outputPinKListBox_->setSelectionMode(QListBox::Multi);
			outputPinKListBox_->setBottomScrollBar(false);
			index=rightKToolBar_->insertWidget(-1, 200, outputPinKListBox_,-1);
			id=rightKToolBar_->idAt(index);
			rightKToolBar_->setItemAutoSized(id, true);

			connect(outputPinKListBox_, SIGNAL(mouseButtonClicked(int, QListBoxItem*, const QPoint&)), this, SLOT(outputPinListMouseButtonClicked(int, QListBoxItem*, const QPoint&)));
		}
		ioPinName.append(">xxxx");
		outputPinKListBox_->insertItem(outputLedOffIcon_, ioPinName, -1);
		outputPinKListBox_->setMinimumWidth(outputPinKListBox_->maxItemWidth());
		outputPinKListBox_->setFixedHeight(outputPinKListBox_->itemHeight(0)*outputPinKListBox_->numRows()+4);
		outputPinCounter_++;
	}
	//BiDirLeft pin
	if(pinType==IOPin::PINTYPE_BIDIR_LEFT)
	{
		if(!inputPinCounter_&&!bidirPinCounterInLeftKToolBar_)
		{
			leftKToolBar_->show();
		}
		if(!bidirPinCounterInLeftKToolBar_)
		{

			bidirPinKListBoxInLeftKToolBar_ = new KListBox(leftKToolBar_, "bidirPinKListBoxLeft", 0);
			QFont bidirPinKListBoxLeft_font(bidirPinKListBoxInLeftKToolBar_->font());
			bidirPinKListBoxLeft_font.setFamily(FONTFAMILY);
			bidirPinKListBoxLeft_font.setPointSize(10);
			bidirPinKListBoxInLeftKToolBar_->setFont(bidirPinKListBoxLeft_font);
			bidirPinKListBoxInLeftKToolBar_->setFrameShape( QTable::GroupBoxPanel );
			bidirPinKListBoxInLeftKToolBar_->setSelectionMode(QListBox::Multi);
			bidirPinKListBoxInLeftKToolBar_->setBottomScrollBar(false);
			index=leftKToolBar_->insertWidget(-1, 200, bidirPinKListBoxInLeftKToolBar_,-1);
			id=leftKToolBar_->idAt(index);
			leftKToolBar_->setItemAutoSized(id, true);

			connect(bidirPinKListBoxInLeftKToolBar_, SIGNAL(mouseButtonClicked(int, QListBoxItem*, const QPoint&)), this, SLOT(bidirPinListMouseButtonClickedInLeftKToolBar(int, QListBoxItem*, const QPoint&)));
		}
		ioPinName.prepend("xxxx<>");
		bidirPinKListBoxInLeftKToolBar_->insertItem(inputLedOffIcon_, ioPinName, -1);
		bidirPinKListBoxInLeftKToolBar_->setMinimumWidth(bidirPinKListBoxInLeftKToolBar_->maxItemWidth());
		bidirPinKListBoxInLeftKToolBar_->setFixedHeight(bidirPinKListBoxInLeftKToolBar_->itemHeight(0)*bidirPinKListBoxInLeftKToolBar_->numRows()+4);
		bidirPinCounterInLeftKToolBar_++;
	}
	//BiDirRight pin
	if(pinType==IOPin::PINTYPE_BIDIR_RIGHT)
	{
		if(!outputPinCounter_&&!bidirPinCounterInRightKToolBar_)
		{
			rightKToolBar_->show();
		}
		if(!bidirPinCounterInRightKToolBar_)
		{

			bidirPinKListBoxInRightKToolBar_ = new KListBox(rightKToolBar_, "bidirPinKListBoxRight", 0);
			QFont bidirPinKListBoxRight_font(bidirPinKListBoxInRightKToolBar_->font());
			bidirPinKListBoxRight_font.setFamily(FONTFAMILY);
			bidirPinKListBoxRight_font.setPointSize(10);
			bidirPinKListBoxInRightKToolBar_->setFont(bidirPinKListBoxRight_font);
			bidirPinKListBoxInRightKToolBar_->setFrameShape( QTable::GroupBoxPanel );
			bidirPinKListBoxInRightKToolBar_->setSelectionMode(QListBox::Multi);
			bidirPinKListBoxInRightKToolBar_->setBottomScrollBar(false);
			index=rightKToolBar_->insertWidget(-1, 200, bidirPinKListBoxInRightKToolBar_,-1);
			id=rightKToolBar_->idAt(index);
			rightKToolBar_->setItemAutoSized(id, true);

			connect(bidirPinKListBoxInRightKToolBar_, SIGNAL(mouseButtonClicked(int, QListBoxItem*, const QPoint&)), this, SLOT(bidirPinListMouseButtonClickedInRightKToolBar(int, QListBoxItem*, const QPoint&)));
		}
		ioPinName.append("<>xxxx");
		bidirPinKListBoxInRightKToolBar_->insertItem(inputLedOffIcon_, ioPinName, -1);
		bidirPinKListBoxInRightKToolBar_->setMinimumWidth(bidirPinKListBoxInRightKToolBar_->maxItemWidth());
		bidirPinKListBoxInRightKToolBar_->setFixedHeight(bidirPinKListBoxInRightKToolBar_->itemHeight(0)*bidirPinKListBoxInRightKToolBar_->numRows()+4);
		bidirPinCounterInRightKToolBar_++;
	}

	atIOSpace_=true;
}

//Luetaan pinnilista
IOPinList* ExternalComponentBaseW::readIOPinsList(void)
{
	return &ioList_;
}

//Kirjoitetaan pinnilista. Tarvitaan kopioitaessa ja luettaessa työtiedot.
void ExternalComponentBaseW::copyIOPins(IOPinList* listToWrite)
{
	int inputPinCount=0, outputPinCount=0, bidirPinCountInLeftKToolBar=0, bidirPinCountInRightKToolBar=0;
	IOPin* pinToCo=listToWrite->pToFirstPin();
	while(pinToCo!=0)
	{
		IOPin::PinID id=pinToCo->readID();
		bool leftRight;
		if(pinToCo->isIn() && !pinToCo->isOut() && !pinToCo->isBidir(leftRight))
			pinToConnectUnconnectID_.id_=inputPinCount++;
		else
			if(!pinToCo->isIn() && pinToCo->isOut()&& !pinToCo->isBidir(leftRight))
			{
				pinToConnectUnconnectID_.id_=outputPinCount++;
				pinToConnectUnconnectID_.id_+=OUTPUTPIN_ID_OFFSET;
			}
			else
			{
				if(leftRight) //Left
				{
					pinToConnectUnconnectID_.id_=bidirPinCountInLeftKToolBar++;
					pinToConnectUnconnectID_.id_+=BIDIRPIN_ID_OFFSET_LEFT;
				}
				else   //Right
				{
					pinToConnectUnconnectID_.id_=bidirPinCountInRightKToolBar++;
					pinToConnectUnconnectID_.id_+=BIDIRPIN_ID_OFFSET_RIGHT;
				}
			}
		if(id.id_>=0)
			emit message_PinCopy(this->name(), id.id_);

		pinToCo=pinToCo->next();
	}
}


//Muutetaan pinnin arvoa nimellä
void ExternalComponentBaseW::writeInputPinValue(QString& ioPinName, bool const  value, bool const  setFlag)
{
	if(atIOSpace_)
		ioList_.writeInputPin(ioPinName, value, setFlag);
}

//Muutateen arvoa indeksillä
void ExternalComponentBaseW::writeInputPinValue(IOPin* pointerToPin, bool const  value, bool const  setFlag)
{
	if(atIOSpace_)
		ioList_.writeInputPin(pointerToPin, value, setFlag);
}

void ExternalComponentBaseW::inputPinListMouseButtonClicked(int button, QListBoxItem* item, const QPoint & pos)
{
	if(item)
	{
		QListBox* apuQLB=item->listBox();
		pinToConnectUnconnectID_.id_=apuQLB->currentItem();
		QString pinText=apuQLB->currentText();
		QString msgTxt=pinText.section('>', 0, 0);
		if(button==Qt::Qt::LeftButton)
			emit message_PinConnect(this->name(), pinToConnectUnconnectID_.id_, msgTxt);
		if(button==Qt::Qt::RightButton)
			emit message_PinUnconnect(this->name());
	}
	else
	{
		if(button==Qt::Qt::RightButton)
			inputPinListEmptyItemRightMouseButtonClicked();
	}
}

void ExternalComponentBaseW::outputPinListMouseButtonClicked(int button, QListBoxItem* item, const QPoint & pos)
{
	if(item)
	{
		QListBox* apuQLB=item->listBox();
		pinToConnectUnconnectID_.id_=apuQLB->currentItem();
		pinToConnectUnconnectID_.id_+=OUTPUTPIN_ID_OFFSET;
		QString pinText=apuQLB->currentText();
		QString msgTxt=pinText.section('>', 1, 1);
		if(button==Qt::Qt::LeftButton)   //Konnektoidaan pinni
			emit message_PinConnect(this->name(), pinToConnectUnconnectID_.id_, msgTxt);
		if(button==Qt::Qt::RightButton)  //Unconnektoidaan pinni
			emit message_PinUnconnect(this->name());
	}
	else
	{
		if(button==Qt::Qt::RightButton)
			outputPinListEmptyItemRightMouseButtonClicked();
	}
}

//Bidir Left
void ExternalComponentBaseW::bidirPinListMouseButtonClickedInLeftKToolBar(int button, QListBoxItem* item, const QPoint & pos)
{
	if(item)
	{
		QListBox* apuQLB=item->listBox();
		pinToConnectUnconnectID_.id_=apuQLB->currentItem();
		pinToConnectUnconnectID_.id_+=BIDIRPIN_ID_OFFSET_LEFT;
		QString pinText=apuQLB->currentText();
		QString msgTxt=pinText.section('<', 0, 0);
		if(button==Qt::Qt::LeftButton)   //Konnektoidaan pinni
			emit message_PinConnect(this->name(), pinToConnectUnconnectID_.id_, msgTxt);
		if(button==Qt::Qt::RightButton)  //Unconnektoidaan pinni
			emit message_PinUnconnect(this->name());
	}
	else
	{
		if(button==Qt::Qt::RightButton)
			bidirPinListEmptyItemRightMouseButtonClickedInLeftKToolBar();
	}
}

//Bidir Right
void ExternalComponentBaseW::bidirPinListMouseButtonClickedInRightKToolBar(int button, QListBoxItem* item, const QPoint & pos)
{
	if(item)
	{
		QListBox* apuQLB=item->listBox();
		pinToConnectUnconnectID_.id_=apuQLB->currentItem();
		pinToConnectUnconnectID_.id_+=BIDIRPIN_ID_OFFSET_RIGHT;
		QString pinText=apuQLB->currentText();
		QString msgTxt=pinText.section('>', 1, 1);
		if(button==Qt::Qt::LeftButton)   //Konnektoidaan pinni
			emit message_PinConnect(this->name(), pinToConnectUnconnectID_.id_, msgTxt);
		if(button==Qt::Qt::RightButton)  //Unconnektoidaan pinni
			emit message_PinUnconnect(this->name());
	}
	else
	{
		if(button==Qt::Qt::RightButton)
			bidirPinListEmptyItemRightMouseButtonClickedInRightKToolBar();
	}
}

//Kytketään pinni. Tieto tulee parentilta
void ExternalComponentBaseW::connectPin(IOPin* pinToConnect)
{
	if(pinToConnectUnconnectID_.id_>=0)
	{
		if(pinToConnect)
		{
			bool hit=false;
			int inCount=0, outCount=0, bidirCountLeft=0, bidirCountRight=0;
			IOPin* componentPin;   //Haetaan osoite pinnilistan alkuun
			componentPin=ioList_.pToFirstPin();
			QString text;
			QString pText=pinToConnect->readName();
			//pText.truncate(4);
			bool leftRight;
			while(componentPin!=0)
			{
				if(pinToConnectUnconnectID_.id_>=BIDIRPIN_ID_OFFSET_LEFT)
				{
					if(componentPin->isBidir(leftRight))
					{
						KPixmap pix;
						bool val;
						if(componentPin->isIn())
						{
							val=pinToConnect->readValue(true);
							if(val)
								pix=inputLedOnIcon_;
							else
								pix=inputLedOffIcon_;
						}
						else
						{
							val=componentPin->readValue(false);
							if(val)
								pix=outputLedOnIcon_;
							else
								pix=outputLedOffIcon_;
						}
						if(leftRight)
						{
							if(bidirCountLeft==pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_LEFT)
							{
								componentPin->writeValue(val, true);
								text=componentPin->readName();
								componentPin->changeID(pinToConnect->readID());
								text.prepend(pText+"<>");
								bidirPinKListBoxInLeftKToolBar_->changeItem(pix, text, pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_LEFT);
								break;
							}
							bidirCountLeft++;
						}
						else
						{
							if(bidirCountRight==pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_RIGHT)
							{
								componentPin->writeValue(val, true);
								text=componentPin->readName();
								componentPin->changeID(pinToConnect->readID());
								text.append("<>"+pText);
								bidirPinKListBoxInRightKToolBar_->changeItem(pix, text, pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_RIGHT);
								break;
							}
							bidirCountRight++;
						}
					}
				}
				else
				{
					if(pinToConnectUnconnectID_.id_>=OUTPUTPIN_ID_OFFSET)
					{
						if(componentPin->isOut()&&!componentPin->isIn()&&!componentPin->isBidir(leftRight))
						{
							if(outCount==pinToConnectUnconnectID_.id_-OUTPUTPIN_ID_OFFSET)
							{
								componentPin->writeValue(componentPin->readValue(false),true); //Merkataan päivitettäväksi.
								KPixmap pix=outputLedOffIcon_;
								if(componentPin->readValue(false))
									pix=outputLedOnIcon_;
								text=componentPin->readName();
								componentPin->changeID(pinToConnect->readID());
								text.append(">"+pText);
								outputPinKListBox_->changeItem(pix, text, pinToConnectUnconnectID_.id_-OUTPUTPIN_ID_OFFSET);
								break;
							}
							outCount++;
						}
					}
					else
					{
						if(componentPin->isIn()&&!componentPin->isOut()&&!componentPin->isBidir(leftRight))
						{
							if(inCount==pinToConnectUnconnectID_.id_)
							{
								componentPin->changeID(pinToConnect->readID());
								componentPin->writeValue(pinToConnect->readValue(true), true);
								KPixmap pix=inputLedOffIcon_;
								if(componentPin->readValue(true))
									pix=inputLedOnIcon_;
								text=componentPin->readName();
								text.prepend(pText+">");
								inputPinKListBox_->changeItem(pix, text, pinToConnectUnconnectID_.id_);
								break;
							}
							inCount++;
						}
					}
				}
				componentPin=componentPin->next();
			}
			QString numberQStr;
			numberQStr.setNum(number_,10);
			numberQStr+=";";
			numberQStr+=componentPin->readName();
			IOPin::PinID cp=pinToConnect->readID();
			emit message_PinConnected(this->name(), cp.id_, numberQStr);
			reset();
		}
	}
	else
	{
	}
}

void ExternalComponentBaseW::unconnectPin(void)   //Unkonnektoitava on pinlistan aktiivinen pinni
{
	int inCount=0, outCount=0, bidirCountLeft=0, bidirCountRight=0;
	IOPin* apuPin;   //Haetaan osoite pinnilistan alkuun
	apuPin=ioList_.pToFirstPin();
	QString text;
	IOPin::PinID unconnectedID;
	bool leftRight;
	while(apuPin!=0)
	{
		if(pinToConnectUnconnectID_.id_>=BIDIRPIN_ID_OFFSET_LEFT)
		{
			if(apuPin->isBidir(leftRight))
			{
				if(leftRight)
				{
					if(bidirCountLeft==pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_LEFT)
					{
						unconnectedID=apuPin->readID();
						apuPin->changeID(apuPin->NO_ID());
						apuPin->writeValue(false, true);
						text=apuPin->readName();
						text.prepend("xxxx<>");
						KPixmap pix=inputLedOffIcon_;
						if(apuPin->isOut())
							pix=outputLedOffIcon_;
						bidirPinKListBoxInLeftKToolBar_->changeItem(pix, text, pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_LEFT);
						break;
					}
					bidirCountLeft++;
				}
				else
				{
					if(bidirCountRight==pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_RIGHT)
					{
						unconnectedID=apuPin->readID();
						apuPin->changeID(apuPin->NO_ID());
						apuPin->writeValue(false, true);
						text=apuPin->readName();
						text.append("<>xxxx");
						KPixmap pix=inputLedOffIcon_;
						if(apuPin->isOut())
							pix=outputLedOffIcon_;
						bidirPinKListBoxInRightKToolBar_->changeItem(pix, text, pinToConnectUnconnectID_.id_-BIDIRPIN_ID_OFFSET_RIGHT);
						break;
					}
					bidirCountRight++;
				}
			}
		}
		else
		{
			if(pinToConnectUnconnectID_.id_>=OUTPUTPIN_ID_OFFSET)
			{
				if(!apuPin->isIn() && apuPin->isOut() && !apuPin->isBidir(leftRight))
				{
					if(outCount==pinToConnectUnconnectID_.id_-OUTPUTPIN_ID_OFFSET)
					{
						unconnectedID=apuPin->readID();
						apuPin->changeID(apuPin->NO_ID());
						apuPin->writeValue(false, true);
						text=apuPin->readName();
						text.append(">xxxx");
						outputPinKListBox_->changeItem(outputLedOffIcon_, text, pinToConnectUnconnectID_.id_-OUTPUTPIN_ID_OFFSET);
						break;
					}
					outCount++;
				}
			}
			else
			{
				if(apuPin->isIn() && !apuPin->isOut() && !apuPin->isBidir(leftRight))
				{
					if(inCount==pinToConnectUnconnectID_.id_)
					{
						unconnectedID=apuPin->readID();
						apuPin->changeID(apuPin->NO_ID());
						apuPin->writeValue(false, true);
						text=apuPin->readName();
						text.prepend("xxxx>");
						inputPinKListBox_->changeItem(inputLedOffIcon_, text, pinToConnectUnconnectID_.id_);
						break;
					}
					inCount++;
				}
			}
		}
		apuPin=apuPin->next();
	}
	QString numberQStr;
	numberQStr.setNum(number_,10);
	numberQStr+=";";
	numberQStr+=apuPin->readName();
	emit message_PinUnconnected(this->name(), unconnectedID.id_, numberQStr);
	reset();
}

void ExternalComponentBaseW::showConnectedPin(const QString &connectedPinTextToShow)
{
	QString connectionText=connectedPinTextToShow;
	QString pinConnectionText;
	if(inputPinCounter_)
	{
		//inputPinKListBox_->setSelectionMode(QListBox::Multi);
		inputPinKListBox_->clearSelection();
		for(int n=0; n<inputPinKListBox_->numRows(); n++)
		{
			pinConnectionText=inputPinKListBox_->text(n);
			if(connectionText==pinConnectionText.section('>', 0, 0))
				inputPinKListBox_->setSelected(n, true);
		}
	}
	if(outputPinCounter_)
	{
		//outputPinKListBox_->setSelectionMode(QListBox::Multi);
		outputPinKListBox_->clearSelection();
		for(int n=0; n<outputPinKListBox_->numRows(); n++)
		{
			pinConnectionText=outputPinKListBox_->text(n);
			if(connectionText==pinConnectionText.section('>', 1, 1))
				outputPinKListBox_->setSelected(n, true);
		}
	}
	if(bidirPinCounterInLeftKToolBar_)
	{
		//bidirPinKListBoxInLeftKToolBar_->setSelectionMode(QListBox::Multi);
		bidirPinKListBoxInLeftKToolBar_->clearSelection();
		for(int n=0; n<bidirPinKListBoxInLeftKToolBar_->numRows(); n++)
		{
			pinConnectionText=bidirPinKListBoxInLeftKToolBar_->text(n);
			if(connectionText==pinConnectionText.section('<', 0, 0))
				bidirPinKListBoxInLeftKToolBar_->setSelected(n, true);
		}
	}
	if(bidirPinCounterInRightKToolBar_)
	{
		//bidirPinKListBoxInRightKToolBar_->setSelectionMode(QListBox::Multi);
		bidirPinKListBoxInRightKToolBar_->clearSelection();
		for(int n=0; n<bidirPinKListBoxInRightKToolBar_->numRows(); n++)
		{
			pinConnectionText=bidirPinKListBoxInRightKToolBar_->text(n);
			if(connectionText==pinConnectionText.section('>', 1, 1))
				bidirPinKListBoxInRightKToolBar_->setSelected(n, true);
		}
	}	
}


void ExternalComponentBaseW::delayTimeTypValueChanged(double timeNanoS)
{
	delayTimeTyp_=timeNanoS/1.0e9; //ns
	delayTimeCalculate();
}

void ExternalComponentBaseW::delayTimeRandomValueChanged(double timeNanoS)
{
	randomizeDelayTime(timeNanoS);
	delayTimeCalculate();
}


void ExternalComponentBaseW::randomizeDelayTime(double maxNanoS)
{
	double pikos=maxNanoS*1000;
	double j=pikos * (rand() / (RAND_MAX + 1.0));
	delayTimeRandom_=double(j/1.0e12);
}

void ExternalComponentBaseW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isDPTR_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isR0_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isR1_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)memoryAreaChanged_;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)changes_;
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isExecuting_;
	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)externalDataSpaceStartAddress_;
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)externalCodeSpaceStartAddress_;
	saveFileStream<<(Q_UINT16)8<<(Q_UINT8)KSimu51::DOUBLE<<(double)executionStartStepTime_;
	saveFileStream<<(Q_UINT16)9<<(Q_UINT8)KSimu51::DOUBLE<<(double)executionReadyTime_;
	saveFileStream<<(Q_UINT16)10<<(Q_UINT8)KSimu51::DOUBLE<<(double)delayTimeTyp_;
	saveFileStream<<(Q_UINT16)11<<(Q_UINT8)KSimu51::DOUBLE<<(double)(delayTimeRandomNumInput_?delayTimeRandomNumInput_->value():0.0);
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

	externalDataSpaceValueT_->saveWorkDataToFile(saveFileStream);
	externalCodeSpaceValueT_->saveWorkDataToFile(saveFileStream);

	if(atIOSpace_)
		ioList_.saveWorkDataToFile(saveFileStream);
}

void ExternalComponentBaseW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8;
	double valueD;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8;
				isDPTR_=(bool)value8;
				break;
			case 1:
				readFileStream>>typeReadData>>value8;
				isR0_=(bool)value8;
				break;
			case 2:
				readFileStream>>typeReadData>>value8;
				isR1_=(bool)value8;
				break;
			case 3:
				readFileStream>>typeReadData>>value8;
				memoryAreaChanged_=(bool)value8;
				break;
			case 4:
				readFileStream>>typeReadData>>value8;
				changes_=(bool)value8;
				break;
			case 5:
				readFileStream>>typeReadData>>value8;
				isExecuting_=(bool)value8;
				break;
			case 6:
				readFileStream>>typeReadData>>value32;
				externalDataSpaceStartAddress_=(unsigned int)value32;
				//Päivitetään ikkuna
				if(atExternalDataSpace_)
					writeExternalDataSpaceStartAddress(externalDataSpaceStartAddress_);
				break;
			case 7:
				readFileStream>>typeReadData>>value32;
				externalCodeSpaceStartAddress_=(unsigned int)value32;
				//Päivitetään ikkuna
				if(atExternalCodeSpace_)
					writeExternalCodeSpaceStartAddress(externalCodeSpaceStartAddress_);
				break;
			case 8:
				readFileStream>>typeReadData>>valueD;
				executionStartStepTime_=valueD;
				break;
			case 9:
				readFileStream>>typeReadData>>valueD;
				executionReadyTime_=valueD;
				break;
			case 10:
				readFileStream>>typeReadData>>valueD;
				delayTimeTyp_=valueD;
				if(delayTimeTypNumInput_)
					delayTimeTypNumInput_->setValue(delayTimeTyp_*1.0e9);
				delayTimeCalculate();
				break;
			case 11:
				readFileStream>>typeReadData>>valueD;
				if( delayTimeRandomNumInput_)
					delayTimeRandomNumInput_->setValue(valueD);
				randomizeDelayTime(valueD);
				delayTimeCalculate();
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);

	externalDataSpaceValueT_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	externalCodeSpaceValueT_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	if(atIOSpace_)
	{
		ioList_.readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
		copyIOPins(&ioList_);
	}
}


//EVENTS -----------------------------------------------------------------------------------------
void ExternalComponentBaseW::closeEvent(QCloseEvent *e)
{
	emit message_Destroyed(this->name());
	KMainWindow::closeEvent(e);
}

QSize ExternalComponentBaseW::getMaximumSize(void)
{
	return QSize(maxWSize_); //maxWSize_ tuodaan perivästä luokasta.

}


/*
bool ExternalComponentBaseW::event( QEvent * e) {
				if( e->type() == QEvent::MouseButtonPress )
					int a= 0;
				return ShareMainW::event(e);
			}
*/
