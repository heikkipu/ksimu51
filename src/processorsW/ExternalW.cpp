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
#include "ExternalW.h"
#include "I51BaseW.h"

#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kpixmap.h>

#include <qlayout.h>
#include <qlabel.h>
#include <string>
#include <qtooltip.h>
#include "../ksimu51.h"



//ExternalW-------------------------------------------------------------------------
ExternalW::ExternalW(I8031* processor, QWidget* parent, char const * name, int wflags)
	: WorkspaceMainW( parent, name, wflags)
{
	clearWFlags(KMainWindow::WStyle_MinMax);
	setFocusPolicy(QWidget::StrongFocus);

	connect(this,SIGNAL(message_MemoryValueChanged(const QString&, const unsigned long, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_MemoryValueChanged(const QString&, const unsigned long, const int)));
	connect(this, SIGNAL(message_ToggleBit(const QString&, const QString&, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_ToggleBit(const QString&, const QString&, const int )));
	connect(this, SIGNAL(message_WindowLevel(const QString&, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_WindowLevel(const QString&, const int )));
	connect(this, SIGNAL(message_Update(const QString& )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_Update(const QString& )));
	connect(this, SIGNAL(message_FocusProgramMemoryAddress(const QString&, const unsigned long )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_FocusProgramMemoryAddress(const QString&, const unsigned long )));

	connect(this, SIGNAL(workspaceWindowActivated()), this, SLOT(componentActivated()));

	statusBar()->setSizeGripEnabled(true);
	statusBar()->message( i18n("Right click, add components."));

	setGeometry (0, 0, parent->width()/2,parent->height()/2);

	addComponentIconKPixmap_.load("pics/KS51hi32NewComponent.png", 0, KPixmap::Auto );
	connectUnconnectPinsIconKPixmap_.load("pics/KS51hi32ConnectUnconnect.png", 0, KPixmap::Auto );

	R0MarkKPixmap_.load("pics/KS51hi16BlueLedOn.png", 0, KPixmap::Auto );
	R1MarkKPixmap_.load("pics/KS51hi16YellowLedOn.png", 0, KPixmap::Auto );
	dptrMarkKPixmap_.load("pics/KS51hi16RedLedOn.png", 0, KPixmap::Auto );


	ledOnIconKPixmap_.load("pics/KS51hi16GreenLedOn.png", 0, KPixmap::Auto );
	ledOffIconKPixmap_.load("pics/KS51hi16GreenLedOff.png", 0, KPixmap::Auto );

	//animateCycleIconKPixmap.load("pics/KS51hi32StepProcessor.png", 0, KPixmap::Auto );
	showComponentChangesWhenRunningKPixmap_.load("pics/KS51hi32ShowComponentChanges.png", 0, KPixmap::Auto );
	showIOChangesWhenRunningKPixmap_.load("pics/KS51hi32ShowIOChanges.png", 0, KPixmap::Auto );

	pToProcessor_=processor;
	addComponentsPopupMenu();

	externalDataMemoryAreaInT_=new MemoryTable(I8031_NameS::EXTERNAL_DATA_MEMORY_SIZE);
	externalDataMemoryAreaOutT_=new MemoryTable(I8031_NameS::EXTERNAL_DATA_MEMORY_SIZE);
	externalCodeMemoryAreaOutT_=new MemoryTable(I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE);

	lastNetPinList_=new IOPinList();

	firstTimeToCreatePortPin_=true;
	portPinCounter_=PORTPIN_START_ID;
	netPinsCreated_=false;
	netPinCounter_=NETPIN_START_ID;
	createIOPinsAndPortsToolBar();

	connectUnconnectPinsIsOn_=false;
	nextPinToConnect_=0;

	horizontalTileDivider_=2;
	doExternalCycleCounter_=0;
	lastStepTime_=0.0;
	lastMachineCycles_=0;
	lastClockTicks_=0;
//   fastestComponentDelayTime=1.0;
	showComponentChangesWhenRunning_=false;
	showIOChangesWhenRunning_=false;

	//Luodaan 0 ja 1 out tyypit nettipinneihin
	addNetPin(IOPin::PINTYPE_OUTPUT, false); //Logic 0
	addNetPin(IOPin::PINTYPE_OUTPUT, true); //Logic 1

	setPlainCaption(i18n("EXTERNAL AREA"));
	//Lisätään pinnit
	addPins(processor->readPins());
	//Lisätään portit External data alueeseen
	//Skennataan koko SFR alue läpi ja jos  portti lisätään
	string apuStr;
	bool value;
	int m=0;

	for(int address=I8031_NameS::MIN_SFR_ADDRESS;address<=I8031_NameS::MAX_SFR_ADDRESS;address++)
	{
		//Jos portti käytettävissä
		if((processor->isInternalDataMemoryLocationInUse(address))&&(processor->isSFRRegisterIOPort(address)))
		{
			for(int n=0;n<8;n++)
			{
				value=false;
				apuStr="P";
				apuStr+=decToString(m, 1, ' ');
				apuStr+=".";
				apuStr+=decToString(n, 1, ' ');
				if(apuStr!=processor->readSFRBitName(address, n))
				{
					apuStr+=";";
					apuStr+=processor->readSFRBitName(address, n);
				}
				if(processor->readInternalDataMemory(address)&(1<<n))
					value=true;
				unsigned int addressBit=((address<<8)|n);//Ylöosa osoite alaosa bittinumero
				addPortPin(addressBit, apuStr, value);
			}
			m++;
		}
	}
}

ExternalW::~ExternalW()
{
}

//Tiedonsiirto kutsuvan ohjelman ja RAM ikkunan välillä.
void ExternalW::writeExternalDataMemoryAreaTable(unsigned int const  address, unsigned int const  value)
{
	wAddress_=address;
	externalDataMemoryAreaInT_->setValue(address,value);
}

bool ExternalW::isExternalDataMemoryAreaTableValueChanged(unsigned int& minAddress, unsigned int& maxAddress)
{
	return externalDataMemoryAreaOutT_->isValueChanged(minAddress, maxAddress);
}

bool ExternalW::isExternalDataMemoryAreaTableValueChanged(unsigned int const  address)
{
	return externalDataMemoryAreaOutT_->isValueChanged(address);
}

unsigned int ExternalW::readExternalDataMemoryAreaTable(unsigned int const  address)
{
	return externalDataMemoryAreaOutT_->readValue(address);
}

//Tyhjätään ne external alueen ikkunat
void ExternalW::clearComponents(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	for ( int i = 0; i < int(windows.count()); ++i )
	{
		QWidget *window = windows.at(i);
		ExternalComponentBaseW *e = (ExternalComponentBaseW*)windows.at( i );
		e->clear();
	}
}

void ExternalW::resetComponents(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	for ( int i = 0; i < int(windows.count()); ++i )
	{
		QWidget *window = windows.at(i);
		ExternalComponentBaseW *e = (ExternalComponentBaseW*)windows.at( i );
		e->reset();
	}
}

//CODE taulu
bool ExternalW::isExternalCodeMemoryAreaTableValueChanged(unsigned int& minAddress, unsigned int& maxAddress)
{
	return externalCodeMemoryAreaOutT_->isValueChanged(minAddress, maxAddress);
}

bool ExternalW::isExternalCodeMemoryAreaTableValueChanged(unsigned int const  address)
{
	return externalCodeMemoryAreaOutT_->isValueChanged(address);
}

unsigned int ExternalW::readExternalCodeMemoryAreaTable(unsigned int const  address)
{
	return externalCodeMemoryAreaOutT_->readValue(address);
}

//Päivitetään komponentit molemminsuuntaisesti.
void ExternalW::executeAndUpdateExternalAreaComponents(double const  stepTime, bool const  cycle, bool const  doNotExecute, bool const  show)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	unsigned int startAddress, size, address, value;
	//Tehdään ulkoisen muistialueen tiedon muuttumisesta välitallennus, jotta voidaan käyttää tietoa kaikissa ikkunoissa.
	bool prosuToExternalValueChanged;
	if(!cycle) //Välitallennus vasta jos ei tehdä cycleä.
		prosuToExternalValueChanged=externalDataMemoryAreaInT_->isValueChanged(wAddress_);
	ExternalComponentBaseW *e;
	bool lastRound=false;
	double stepTimeCounter=lastStepTime_;
	//double fastestComponentDelayTime=0.0;
	//Käydään komponentit lävi niin monta kertaa, että steppiaika täyttyy
	do
	{
		double nextTime=stepTime;
		//stepTimeCounter+=fastestComponentDelayTime;
		if(stepTimeCounter>=nextTime)
		{
			stepTimeCounter=nextTime;
			lastRound=true;
		}
		for(int g=0; g<2; g++)
		{
			for(int h=0; h<2; h++)         //Tehään 2 kertaa jotta IO:t päivittyy totaalisesti
			{
				for(int i=0; i<int(windows.count()); i++)
				{
					e=(ExternalComponentBaseW*)windows.at(i);

					if(!h&&!doNotExecute)//Eka kerralla tehään execute
					//if(!doNotExecute)
					{
						double nextComponentTime=e->executeComponent(stepTimeCounter);
						if(nextComponentTime<nextTime)
							//Jos seuraava komponentin asettumisaika pienempi kuin edellinen aika niin seuraava askellus sen mukaan.
							nextTime=nextComponentTime;
					}

					//Jos IO-Kytkentää päivitetään joka kerta
					if(e->isAtIOSpace())
					{

						IOPinList* pList=e->readIOPinsList();
						IOPin* componentPin=pList->pToFirstPin();
						IOPin::PinID pinID;
						int processorPinIndex,n;
						bool v;
						while(componentPin!=0)
						{
							pinID=componentPin->readID();
							if(pinID.id_>=NETPIN_START_ID) //Nettiin kytketty pinni
							{
								IOPin* netPin=netPinList_.pToFirstPin();
								for(n=NETPIN_START_ID; n<pinID.id_; n++) //Ajetaan pointteri kohalleen
									netPin=netPin->next();
								if(componentPin->isOut())   //IO->net eli external komponentin outputit
								{
									v=componentPin->readValue(true);
									netPin->writeValue(v, true);
								}
								if(componentPin->isIn())   //net->IO eli external komponentin inputit
								{
									v=netPin->readValue(false);
									e->writeInputPinValue(componentPin, v, true);
								}
							}
							else
							{
								if(pinID.id_>=PORTPIN_START_ID) //Porttiin kytketty pinni
								{
									IOPortPin* processorPortPin=portPinList_;
									for(n=PORTPIN_START_ID; n<pinID.id_; n++) //Ajetaan pointteri kohalleen
										processorPortPin=processorPortPin->next();
									if(componentPin->isOut())   //IO->prosuPort eli external komponentin outputit
									{
										v=componentPin->readValue(true);
										processorPortPin->writeValue(v, true);
									}
									if(componentPin->isIn())   //prosuPort->IO eli external komponentin inputit
									{
										v=processorPortPin->readValue(false);
										e->writeInputPinValue(componentPin, v, true);
									}
								}
								else
								{
									if(pinID.id_>=PIN_START_ID) //Erikoispinniin kytketty pinni
									{
										IOPin* pinProcessor=ioPinListProcessor_->pToFirstPin();
										IOPin* lastPin=lastIOPinList_->pToFirstPin();
										for(n=PIN_START_ID; n<pinID.id_; n++) //Ajetaan pointterit kohalleen
										{
											pinProcessor=pinProcessor->next();
											lastPin=lastPin->next();
										}
										if(componentPin->isOut())   //IO->prosuPin eli external komponentin outputit
										{
											v=componentPin->readValue(true);
											pinProcessor->writeValue(v, true);
											lastPin->writeValue(v, true);
										}
										if(componentPin->isIn())     //prosuPin->IO eli external komponentin inputit
										{
											v=lastPin->readValue(false);
											e->writeInputPinValue(componentPin, v, true);
										}
									}
								}
							}
							componentPin=componentPin->next();
						}
					}

					if(lastRound&&h)
					{
						//Jos komponentti kytketty CODE muistialueelle. Tiedonsiirto processoriin päin ulkoiselle CODE alueelle.
						if(e->isAtExternalCodeSpace() && !cycle)
						{
							//Ulkoisen muistialueen komponentti->prosu
							unsigned int minAddress, maxAddress;
							if(e->isExternalCodeSpaceValueChanged(minAddress, maxAddress))
							{
								for(address=minAddress;address<=maxAddress;address++)
								{
									if(e->isExternalCodeSpaceValueChanged(address))
									{
										externalCodeMemoryAreaOutT_->setValue(address, e->readExternalCodeSpaceValue(address));
									}
								}
							}
						}

						//Jos komponentti kytketty Data muistialueelle
						if(e->isAtExternalDataSpace() && !cycle)
						{

							//Ulkoisen muistialueen komponentti->prosu
							unsigned int minAddress, maxAddress;
							if(e->isExternalDataSpaceValueChanged(minAddress, maxAddress))
							{
								for(address=minAddress;address<=maxAddress;address++)
								{
									if(e->isExternalDataSpaceValueChanged(address))
									{
										externalDataMemoryAreaOutT_->setValue(address, e->readExternalDataSpaceValue(address));
									}
								}
							}

							//prosu->Ulkoisen muistialueen komponentti
							startAddress=e->readExternalDataSpaceStartAddress();
							size=e->readExternalDataSpaceSize();
							if(prosuToExternalValueChanged)
							{
								if((startAddress<=wAddress_)&&((startAddress+size)>wAddress_))
								{
									value=externalDataMemoryAreaInT_->readValue(wAddress_);
									e->setExternalDataSpaceValue(wAddress_, value);
								}
							}
							//Ulkoiseen muistialueeseen pointterien tietojen asetus.
							e->setDatapointers();
						}

						//Päivitys näyttöön jos näytetään.
						if(lastRound && g && h)
						{
							if(showComponentChangesWhenRunning_&&e->hasChanges())
								e->updateComponent(showIOChangesWhenRunning_);
							if(show)
								e->updateComponent(true);
						}
					}
				}
			}
		}

		if(showIOChangesWhenRunning_)
			updateNets();

		if(!lastRound)
			stepTimeCounter=nextTime;
	}
	while(!lastRound);

	if(!cycle)
	{
		if(show||showIOChangesWhenRunning_)
		{
			updatePinKListBox(ioPinsKListBox_, ioPinListProcessor_->pToFirstPin());
			updatePinKListBox(portsKListBox_, portPinList_);
			if(netPinsCreated_)
				updatePinKListBox(netKListBox_, netPinList_.pToFirstPin());
			selectConnection(selectedConnectionTxt_);
		}
	}
	lastStepTime_=stepTimeCounter;
}

void ExternalW::updateAllComponents(const ExternalCycle* extCyc, double const  stepTime, I8031_NameS::machineCycles_Type const  machineCycles, I8031_NameS::clockTicks_Type const  clockTicks, I8031_NameS::programCounter_Type programCounterValue, bool const  show)
{
	//Päivitetään external cycle
	externalCyc_=*extCyc;
	doExternalCycleCounter_=0;
/*   if(show)
	{
		QString strNumCyc;
		strNumCyc.setNum(externalCyc_.readNumCycles());
		ioPinsTools_AnimateCycleButton->setTextLabel("0/"+strNumCyc);
	}*/
	int cycles=externalCyc_.readNumCycles_FromVec();
	if((stepTime>lastStepTime_)&&(lastMachineCycles_!=machineCycles))
	{
		unsigned char cycleCount=0;
		if(cycles)
		{
			double wStepT=stepTime-lastStepTime_;
			//Tehdään päivitys external cyclenä jos cyclejä
			do
			{
				//int countsOfState=externalCyc_.readOneStateCycles_FromVec(cycleCount);

				double divider=double(clockTicks-lastClockTicks_);
				if(externalCyc_.isHalfCLKPulse())
					divider*=2;
				//double stepTAdd=double(countsOfState)*(wStepT/divider);
				double stepTAdd=double(wStepT/divider);
				doExternalCycle(cycleCount, show);
				
				if(cycles>1)
					executeAndUpdateExternalAreaComponents(lastStepTime_+stepTAdd, true, false, false);
				else
					executeAndUpdateExternalAreaComponents(stepTime, false, false, show); //Viimeisellä kerralla tehdään päivitys normaalina

				//Inputs sample
				unsigned int ioPinsToSample=externalCyc_.readIOPinSampleVecCount_FromVec(cycleCount);
				if(ioPinsToSample)
				{
					for(unsigned int n=0; n<ioPinsToSample; n++)
					{
						IOPinList::PinIndex pinIndex=externalCyc_.readIOPinIndexToSample_FromVec(cycleCount, n);
						bool pinValue=ioPinListProcessor_->readValue(pinIndex, false);
						IOPin* sPin=ioPinListSampleInputsList_->pToPin(pinIndex);
						sPin->writeValue(pinValue, false);
					}
				}
				/*if(sampleP0Inputs) OLDII
				{
					copyIOPinsListValues(ioPinListSampleInputsList_->pToFirstPin(), ioPinListProcessor_);
					copyPortPinsListValues(portPinListSampleInputs_, portPinList_, 0, 0);
				}
				if(samplePxInputs)
				{
					copyPortPinsListValues(portPinListSampleInputs_, portPinList_, 1, 100);
				}*/
				unsigned int portsToSample=externalCyc_.readPortSampleVecCount_FromVec(cycleCount);
				if(portsToSample)
				{
					for(unsigned int n=0; n<portsToSample; n++)
					{
						unsigned int portNumber;
						unsigned int portAddress=externalCyc_.readPortAddressToSample_FromVec(cycleCount, n, portNumber);
						copyPortPinsListValues(portPinListSampleInputs_, portPinList_, portNumber, portNumber);
					}
				}
				
				cycles--;
				cycleCount++;
			}
			while(cycles);
		}
		else //Tehdään päivitys normaalisti.
			executeAndUpdateExternalAreaComponents(stepTime, false, false, show);
	}
	else
		executeAndUpdateExternalAreaComponents(stepTime, false, false, show);

	updatePinKListBox(ioPinsKListBox_, ioPinListProcessor_->pToFirstPin());

	lastMachineCycles_=machineCycles;
	lastClockTicks_=clockTicks;
	programCounterValue_=programCounterValue;
}

void ExternalW::doExternalCycle(unsigned char cycleCount, bool show)
{
	//Jos pinnejä
	unsigned int ioPinsToDo=externalCyc_.readIOPinOutVecCount_FromVec(cycleCount);
	if(ioPinsToDo)
	{
		//Kopioidaan ensin prosessorin pinnien tila
		ioPinsCycleList_->copyListValues(ioPinListProcessor_);
		for(unsigned int n=0; n<ioPinsToDo; n++)
		{
			bool pinValue;
			IOPinList::PinIndex pinIndex=externalCyc_.readIOPinIndexAndValue_FromVec(cycleCount, n, pinValue);
			IOPin* wPin=ioPinsCycleList_->pToPin(pinIndex);
			wPin->writeValue(pinValue, false);
		}
		
	}

	//Ports
	unsigned int ioPortsToDo=externalCyc_.readPortOutVecCount_FromVec(cycleCount);
	if(ioPortsToDo)
	{
		for(unsigned int n=0; n<ioPortsToDo; n++)
		{
			unsigned char portMask;
			unsigned char portAddress;
			unsigned char portValue=externalCyc_.readPortValue_FromVec(cycleCount, n, portAddress, portMask);
			updatePort(portAddress, portValue, portMask, show);
		}
	}

	//Päivitetään pinnit näkyviin.
	updatePins(false, show);

}

void ExternalW::setDatapointerValues(unsigned int const  dptrAddress, unsigned char const  r0Address, unsigned char const  r1Address)
{
	someDataPointerValueChanged_=false;
	if((dptrValue_!=dptrAddress) || (r0Value_!=r0Address) || (r1Value_!=r1Address))
		someDataPointerValueChanged_=true;
	dptrValue_=dptrAddress;
	r0Value_=r0Address;
	r1Value_=r1Address;
}

void ExternalW::showPointerDPTR(void)
{
	activatePointerWindow(ACTIVATE_DPTR);
}

void ExternalW::showPointerR0(void)
{
	activatePointerWindow(ACTIVATE_R0);
}

void ExternalW::showPointerR1(void)
{
	activatePointerWindow(ACTIVATE_R1);
}

//Näytetään se komponentti ikkuna johon pointteri osoittaa.
void ExternalW::activatePointerWindow(unsigned char const  pointer)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if(!windows.count())
		return;
	ExternalComponentBaseW *e;
	for ( int i = 0; i < int(windows.count()); ++i )
	{
		e = (ExternalComponentBaseW*)windows.at( i );
		if(e->isAtExternalDataSpace())
		{
			backPreviousWindowState();
			switch(pointer)
			{
				case ACTIVATE_DPTR:
					e->showPointerDPTR();
					break;
				case ACTIVATE_R0:
					e->showPointerR0();
					break;
				case ACTIVATE_R1:
					e->showPointerR1();
					break;
			}
			centerActiveWindow();
		}
	}
}

void ExternalW::createIOPinsAndPortsToolBar(void)
{
	int id,index, idCount=0;

	//Työkalupalkki
	toolsTools_ = new KToolBar(this, "toolsTools_");
	addToolBar(toolsTools_, i18n("Tools Tools"), DockTop, TRUE);

	toolsTools_->insertButton(addComponentIconKPixmap_, idCount++, addKPopupMenu, true, i18n("Add component"),-1);
	toolsTools_->insertLineSeparator(); idCount++;
	toolsTools_ConnectUnconnectPins_ID_=toolsTools_->insertButton(connectUnconnectPinsIconKPixmap_, idCount++, SIGNAL(clicked()), this, SLOT(connectUnconnectPinsButtonPressed()), false, i18n("Connect/Unconnect Pins"),-1);
	toolsTools_->setToggle(idCount-1,true);
	toolsTools_->insertLineSeparator(); idCount++;
	toolsTools_ShowComponentChangesWhenRunning_ID_=toolsTools_->insertButton(showComponentChangesWhenRunningKPixmap_, idCount++, SIGNAL(clicked()), this, SLOT(showComponentChangesWhenRunningButtonPressed()), false, i18n("Show component changes when running"),-1);
	toolsTools_->setToggle(idCount-1,true);
	toolsTools_ShowIOChangesWhenRunning_ID_=toolsTools_->insertButton(showIOChangesWhenRunningKPixmap_, idCount++, SIGNAL(clicked()), this, SLOT(showIOChangesWhenRunningButtonPressed()), true, i18n("Show IO changes when running"),-1);
	toolsTools_->setToggle(idCount-1,true);

	QString connectTip("If connection allowed:\nLeft clic, select pin to connect component pin.");
	//IO-Pinnit
	idCount=0;
	ioPinsTools_=new KToolBar( this, "ioPinsTools_" );
	QToolTip::add(ioPinsTools_,"Dpl click, toggle input bit.\n"+connectTip);
	addToolBar(ioPinsTools_, i18n( "I/O Pins" ), DockLeft, FALSE );
	ioPinsTools_->setMovingEnabled(false);
	ioPinsTools_->setMinimumWidth(300);
	ioPinsTools_->setMinimumHeight(300);
	//Labeli toolbaariin
	QLabel *ioPinsLabel=new QLabel(ioPinsTools_, "ioPinsLabel", 0);
	QFont ioPinsLabel_font(ioPinsLabel->font());
	ioPinsLabel_font.setFamily(FONTFAMILY);
	ioPinsLabel_font.setPointSize(10);
	ioPinsLabel->setFont(ioPinsLabel_font);
	ioPinsLabel->setText(i18n("    I/O-Pins    "));
	ioPinsTools_->insertWidget(idCount++, 200, ioPinsLabel,-1);

	ioPinsKListBox_ = new KListBox(ioPinsTools_, "ioPinsKListBox_", 0);
	QFont ioPinsKListBox_font(ioPinsKListBox_->font());
	ioPinsKListBox_font.setFamily(FONTFAMILY);
	ioPinsKListBox_font.setPointSize(10);
	ioPinsKListBox_->setFont(ioPinsKListBox_font);
	ioPinsKListBox_->setFrameShape( QTable::GroupBoxPanel );
	ioPinsKListBox_->setMaximumWidth(200);
	ioPinsKListBox_->setMinimumHeight(50);
	ioPinsKListBox_->setSelectionMode(QListBox::Single);
	index=ioPinsTools_->insertWidget(idCount++, 200, ioPinsKListBox_,-1);
	id=ioPinsTools_->idAt(index);
	ioPinsTools_->setItemAutoSized(id, true);

	connect(ioPinsKListBox_, SIGNAL(doubleClicked (QListBoxItem* , const QPoint&)), this, SLOT(ioPinsKListBoxDoubleClicked(QListBoxItem* , const QPoint&)));
	connect(ioPinsKListBox_, SIGNAL(clicked (QListBoxItem* , const QPoint&)), this, SLOT(ioPinsKListBoxClicked(QListBoxItem* , const QPoint&)));

	//IO portit
	portsKListBox_ = new KListBox(ioPinsTools_, "portsKListBox_", 0);
	QToolTip::add(portsKListBox_,"Dpl click, toggle bit.\n"+connectTip);
	QFont portsKListBox_font(portsKListBox_->font());
	portsKListBox_font.setFamily(FONTFAMILY);
	portsKListBox_font.setPointSize(10);
	portsKListBox_->setFont(portsKListBox_font);
	portsKListBox_->setFrameShape( QTable::GroupBoxPanel );
	portsKListBox_->setMaximumWidth(200);
	portsKListBox_->setMinimumHeight(50);
	portsKListBox_->setSelectionMode(QListBox::Single);
	index=ioPinsTools_->insertWidget(idCount++, 200, portsKListBox_,-1);
	id=ioPinsTools_->idAt(index);
	ioPinsTools_->setItemAutoSized(id, true);

	connect(portsKListBox_, SIGNAL(doubleClicked (QListBoxItem* , const QPoint&)), this, SLOT(portsKListBoxDoubleClicked(QListBoxItem* , const QPoint&)));
	connect(portsKListBox_, SIGNAL(clicked (QListBoxItem* , const QPoint&)), this, SLOT(portsKListBoxClicked(QListBoxItem* , const QPoint&)));

	//Net toolbaari
	idCount=0;
	netTools_=new KToolBar( this, "netTools_" );
	addToolBar(netTools_, i18n( "Net Pins" ), DockRight, FALSE );
	netTools_->setMovingEnabled(false);
	netTools_->setMinimumWidth(300);
	netTools_->setMinimumHeight(300);
	//Labeli toolbaariin
	QLabel *netLabel= new QLabel(netTools_, "netLabel", 0);
	QFont netLabel_font(ioPinsLabel->font());
	netLabel_font.setFamily(FONTFAMILY);
	netLabel_font.setPointSize(10);
	netLabel->setFont(netLabel_font);
	netLabel->setText(i18n(" Helper Connections "));
	netTools_->insertWidget(idCount++, 200, netLabel,-1);
	//ListBoxi
	netKListBox_ = new KListBox(netTools_, "netKListBox_", 0);
	QToolTip::add(netKListBox_,"Right clic, new net.\nIf connection allowed:\nLeft clic, select net to connect component pin.");
	QFont netKListBox_font(netKListBox_->font());
	netKListBox_font.setFamily(FONTFAMILY);
	netKListBox_font.setPointSize(10);
	netKListBox_->setFont(netKListBox_font);
	netKListBox_->setFrameShape( QTable::GroupBoxPanel );
	netKListBox_->setMaximumWidth(200);
	netKListBox_->setMinimumHeight(100);
	netKListBox_->setSelectionMode(QListBox::Single);
	index=netTools_->insertWidget(idCount++, 200, netKListBox_,-1);
	id=netTools_->idAt(index);
	netTools_->setItemAutoSized(id, true);
	//Net Kytkennät
	connect(netKListBox_, SIGNAL(doubleClicked (QListBoxItem* , const QPoint&)), this, SLOT(netKListBoxDoubleClicked(QListBoxItem* , const QPoint&)));
	connect(netKListBox_, SIGNAL(mouseButtonClicked(int, QListBoxItem* , const QPoint&)), this, SLOT(netKListBoxMouseButtonClicked(int, QListBoxItem* , const QPoint&)));
}

void ExternalW::addPins(IOPinList* pinList)
{
	KPixmap icon;
	QString pname;

	ioPinListProcessor_=pinList;   //Asetetaan ioPinnien pointteri osoittamaan prosessorin pinnieihin
	ioPinsCycleList_ = new IOPinList(pinList->pToFirstPin());   //Cyclen ioPinnien pointteri
	lastIOPinList_ = new IOPinList(pinList->pToFirstPin());   //Viimeisin ioPinnin tieto Päivitystä varten.
	ioPinListSampleInputsList_=new IOPinList(pinList->pToFirstPin()); //Sample inputs pinnilista

	//ioPin* apuPin=new ioPin();
	IOPin* apuPin;
	apuPin=pinList->pToFirstPin();
	while(apuPin!=0)
	{
		pname="";
		if(apuPin->isIn())
			pname+="<";
		if(apuPin->isOut())
			pname+=">";
		pname+=" ";
		pname+=apuPin->readName();
		if(apuPin->readValue(true))
			icon=ledOnIconKPixmap_;
		else
			icon=ledOffIconKPixmap_;
		ioPinsKListBox_->insertItem (icon, pname, -1 );
		apuPin=apuPin->next();
	}
}

IOPin* ExternalW::readPinsList(void)
{
	return ioPinListProcessor_->pToFirstPin();
}

IOPin* ExternalW::readPinsListSampleInputs(void)
{
	return ioPinListSampleInputsList_->pToFirstPin();
}

void ExternalW::writePins(bool const  show)
{
	updatePins(true, show);
}

void ExternalW::updatePins(bool const  processorPointer, bool const  show)
{
	int currentI;
	bool selectedI=false;
	if(show||showIOChangesWhenRunning_)
		if(ioPinsKListBox_->selectedItem())
		{
			currentI=ioPinsKListBox_->currentItem();
			selectedI=true;
		}
	int index=0;
	IOPin* sourPin;
	IOPin* destPin=lastIOPinList_->pToFirstPin();
	//Valitaan kumpaa pointteria käytetään. Ja päivitetään tiedot sen mukaan
	if(processorPointer)
		sourPin=ioPinListProcessor_->pToFirstPin();   //Prosessoriin osoittava pointteri
	else
		sourPin=ioPinsCycleList_->pToFirstPin(); //Animate cyclen pointteri
	while(sourPin!=0)
	{
		bool sValue=sourPin->readValue(false);
		if(sValue!=destPin->readValue(false))
		{
			if(destPin->isOut())	//Write only outputs
				destPin->writeValue(sValue,true);
			if(show||showIOChangesWhenRunning_)
			{
				KPixmap icon;
				QString pname=ioPinsKListBox_->text(index);
				if(sValue)
					icon=ledOnIconKPixmap_;
				else
					icon=ledOffIconKPixmap_;
				ioPinsKListBox_->changeItem(icon, pname, index);
			}
		}
		if(destPin->next())
			destPin=destPin->next();
		sourPin=sourPin->next();
		index++;
	}
	if(show||showIOChangesWhenRunning_)
	{
		if(selectedI)
		{
			if(currentI>-1)
			{
				ioPinsKListBox_->setCurrentItem(currentI);
				ioPinsKListBox_->ensureCurrentVisible();
			}
		}
		else
			ioPinsKListBox_->clearSelection();
	}
}


void ExternalW::ioPinsKListBoxClicked(QListBoxItem* listItem, const QPoint& mousePoint)
{
	if(listItem)
	{
		//Haetaan selectiota vastaava pinnitieto
		QListBox* itemListBox=listItem->listBox();
		int currentItem=itemListBox->currentItem();
		QString currentTxt=ioPinsKListBox_->currentText();
		int txtLength=currentTxt.length();
		selectedConnectionTxt_=currentTxt.right(txtLength-2);
		if(connectUnconnectPinsIsOn_)
		{
			if(currentItem>=0)
			{
				IOPin* apuPin;
				apuPin=ioPinListProcessor_->pToFirstPin();
				for(int n=0;n<currentItem;n++)      //Vieadään pointteri oikean itemin kohdalle
					apuPin=apuPin->next();
				nextPinToConnect_=apuPin;
			}
		}
		selectConnection(selectedConnectionTxt_);
	}
}

void ExternalW::ioPinsKListBoxDoubleClicked (QListBoxItem *item, const QPoint &pos)
{
	//Lähettää viestin Processor8051W ikkunalle pinnin tilanvaihdosta.
	if(item)
	{
		QString listText(item->text());
		string messageString("");
		//Muutetaan std stringiksi ja poistetaan suuntamerkki
		messageString=QChar(listText[2]);
		for(int n=3;n<listText.length();n++)
			messageString+=QChar(listText[n]);
		emit message_ToggleBit(this->name(), messageString, 0);
		//emit messageToProcessor8051W(this->name(), I51BaseW::TOGGLE_BIT, messageString, 0, 0.0);
	}
}

//------------------------------------------------------ PORT
//Lisätään pinni kerrallaan. Aloitettava portista 0 alkaen järjestyksessä
void ExternalW::addPortPin(unsigned int bitAddress, string const pinName, bool const  value)
{
	//Luodaan ensin jäsen listaan
	IOPortPin* portPinToAdd, *portPinToAddSampleInputs;
	IOPin::PinID pID;
	pID.id_=portPinCounter_;
	portPinToAdd = new IOPortPin(bitAddress, pinName, pID);
	portPinToAddSampleInputs = new IOPortPin(bitAddress, pinName, pID);
	if(firstTimeToCreatePortPin_) //Jos ensimmäinen kerta kutsuvaan ohjelmaan
	{
		portPinList_=new IOPortPin();
		portPinListLastMember_=new IOPortPin();
		portPinListLastMember_=portPinToAdd;
		portPinList_=portPinListLastMember_;

		portPinListSampleInputs_=new IOPortPin(); //Sample inputtia varten
		portPinListSampleInputsLastMember_=new IOPortPin(); //Sample inputtia varten
		portPinListSampleInputsLastMember_=portPinToAddSampleInputs;
		portPinListSampleInputs_=portPinListSampleInputsLastMember_;

		firstTimeToCreatePortPin_=false;
	}
	else
	{
		portPinListLastMember_->setNextPin(portPinToAdd);
		portPinListSampleInputsLastMember_->setNextPin(portPinToAddSampleInputs);
		portPinListLastMember_=portPinToAdd;
		portPinListSampleInputsLastMember_=portPinToAddSampleInputs;
	}
	portPinListLastMember_->writeValue(value, false);
	portPinListSampleInputsLastMember_->writeValue(value, false);
	portPinCounter_++;

	KPixmap icon=ledOffIconKPixmap_;
	if(value)
		icon=ledOnIconKPixmap_;
	portsKListBox_->insertItem (icon, pinName, -1 );
}

IOPortPin* ExternalW::readPortPinList(void)
{
	return portPinList_;
}

IOPortPin* ExternalW::readPortPinListSampleInputs(void)
{
	return portPinListSampleInputs_;
	//return portPinList_;
}

//Päivitetään tiedot porteittain.
void ExternalW::writePort(unsigned char const  portAddress, unsigned char const  portValue, bool const  show)
{
	if(!firstTimeToCreatePortPin_)
	{
		updatePort(portAddress, portValue, 0xff, show);
	}
}

//Päivitetään tiedot portin pinnilistaan.
void ExternalW::updatePort(unsigned char const  portAddress, unsigned char const  portValue, unsigned char const  updateMask, bool const  show)
{
	if(!firstTimeToCreatePortPin_)
	{
		//Käyttää ioPin luokkaa joka on bittilista
		IOPortPin* portPinListToSet;
//		IOPin* lastPortPinListToSet;
		KPixmap icon;
		int currentI;
		QString pname;
		bool selectedI=false;	
		if(show||showIOChangesWhenRunning_)
			if(portsKListBox_->selectedItem())
			{
				currentI=portsKListBox_->currentItem();
				selectedI=true;
			}
		for(int n=0;n<8;n++)
		{
			if((updateMask>>n)&1)
			{
				unsigned int portPinAddressToChange=((portAddress<<8)|n);
				bool portPinValue=bool((portValue>>n)&0x01);
				unsigned int index=0;
				portPinListToSet=portPinList_;
				while(portPinListToSet!=0)
				{
					unsigned int portPinAddressInList=portPinListToSet->readAddress();
					//Jos listan bitin osoite sattuu päivitettävälle alueelle
					if(portPinAddressToChange==portPinAddressInList)
					{
						portPinListToSet->writeValue(portPinValue, false);
						if(show||showIOChangesWhenRunning_)
						{
							if(portPinValue)
								icon=ledOnIconKPixmap_;
							else
								icon=ledOffIconKPixmap_;
							pname=portsKListBox_->text(index);
							portsKListBox_->changeItem(icon, pname, index);
						}
						break; //Voidaan poistua seuraavaan bittiin
					}
					index++; //
					portPinListToSet=portPinListToSet->next();
				}
			}
		}
		if(show||showIOChangesWhenRunning_)
		{
			if(selectedI)
			{
				if(currentI>-1)
				{
					portsKListBox_->setCurrentItem(currentI);
					portsKListBox_->ensureCurrentVisible();
				}
			}
			else
				portsKListBox_->clearSelection();
		}
	}
}

//Kopioi portin listan tilat toiseen listaan
//Voidaan määrittää alkaen portista johonkin porttiin asti, tai kaikki loput jos toPort arvo tarpeeksi iso, vaikka 100.
void ExternalW::copyPortPinsListValues(IOPortPin* destinationPinList, IOPortPin* sourcePinList, unsigned char const  fromPort, unsigned char const  toPort)
{
	IOPortPin* sourPin=sourcePinList;
	IOPortPin* destPin=destinationPinList;
	unsigned char pinCounter=0, portCounter=0;
	while(destPin!=0)
	{
		if(portCounter>=fromPort)
			destPin->writeValue(sourPin->readValue(true), true);

		if(sourPin->next())
			sourPin=sourPin->next();
		else
			break;
		destPin=destPin->next();

		pinCounter++;
		if(pinCounter>7)
		{
			pinCounter=0;
			portCounter++;

			if(portCounter>toPort)
				break;
		}
	}
}

void ExternalW::portsKListBoxClicked(QListBoxItem* listItem, const QPoint& mousePoint)
{
	if(listItem)
	{
		//Haetaan portin konnektoitava pinni
		QListBox* itemListBox=listItem->listBox();
		int currentItem=itemListBox->currentItem();
		selectedConnectionTxt_=portsKListBox_->currentText();
		if(connectUnconnectPinsIsOn_)
		{
			//Haetaan selectiota vastaava pinnitieto
			if(currentItem>=0)
			{
				IOPortPin* apuPin;
				apuPin=portPinList_;
				for(int n=0;n<currentItem;n++)      //Vieadään pointteri oikean itemin kohdalle
					apuPin=apuPin->next();
				nextPinToConnect_=apuPin;
			}
		}
		selectConnection(selectedConnectionTxt_);
	}
}

void ExternalW::portsKListBoxDoubleClicked(QListBoxItem* item, const QPoint& pos)
{
	if(item)
	{
		QString listText(item->text());
		string messageString("");
		messageString=QChar(listText[0]);
		for(int n=1;n<listText.length();n++)
			messageString+=QChar(listText[n]);
		unsigned int bitAddress{0};
		//Etsitään nimeä vastaava bittiosoite
		IOPortPin* portPinListToFind;
		portPinListToFind=portPinList_;
		while(portPinListToFind!=0)
		{
			string portPinName=portPinListToFind->readName();
			if(messageString==portPinName)
			{
				bitAddress=portPinListToFind->readAddress();
				break;
			}
			portPinListToFind=portPinListToFind->next();
		}
		emit message_ToggleBit(this->name(), messageString, bitAddress);
		//emit messageToProcessor8051W(this->name(), I51BaseW::TOGGLE_BIT, messageString, bitAddress, 0.0);
	}
}

//--------------------------------------------- NET

void ExternalW::addNetPin(IOPin::PinTYPE const pinType, bool const  value)
{
	//Luodaan ensin jäsen listaan
	//Varmistetaan että kahden ekan jälkeen pinnityyppi on bidir,
	int netNumber = netPinCounter_-NETPIN_START_ID;
	IOPin::PinTYPE pintype = ( netNumber > 1 )? IOPin::PINTYPE_BIDIR_LEFT : pinType;
	//Kaksi ekaa listan pinniä on noll ja yksi vakiot.
	QString pName=(netNumber<2)? "C":"N"; //C=constant, N=net.
	pName+=decToString(netNumber, 3, '0');
	IOPin::PinID pid;
	pid.id_=netPinCounter_;
	netPinList_.addPin(pid, pName, value, pintype);
	netPinCounter_++;
	delete lastNetPinList_;   //Deletoidaan vanha, jotta voidaan luoda uutta.
	lastNetPinList_=new IOPinList(netPinList_.pToFirstPin());

	KPixmap icon=ledOffIconKPixmap_;
	if(value)
		icon=ledOnIconKPixmap_;
	netKListBox_->insertItem (icon, pName, -1 );
	netPinsCreated_=true;
}

//Päivittää nets listan. Päivittää vain muuttuneet.
void ExternalW::updateNets(void)
{
	if(netPinsCreated_)
	{
		KPixmap icon;
		QString pname;
		int currentI=netKListBox_->currentItem();
		int index=0;
		IOPin* sourPin=netPinList_.pToFirstPin();
		IOPin* destPin=lastNetPinList_->pToFirstPin();
		while(sourPin!=0)
		{
			bool value=sourPin->readValue(false);
			if(value!=destPin->readValue(false))
			{
				destPin->writeValue(value,true);
				if(value)
					icon=ledOnIconKPixmap_;
				else
					icon=ledOffIconKPixmap_;
				pname=netKListBox_->text(index);
				netKListBox_->changeItem(icon, pname, index);
			}
			if(destPin->next())
				destPin=destPin->next();
			sourPin=sourPin->next();
			index++;
		}
		if(currentI>-1)
		{
			netKListBox_->setCurrentItem(currentI);
			netKListBox_->ensureCurrentVisible();
		}
	}
}

void ExternalW::netKListBoxMouseButtonClicked(int button, QListBoxItem* listItem, const QPoint& mousePoint)
{
	if(button==QMouseEvent::LeftButton)
	{
		if(listItem)
		{
			//Haetaan konnektoitava yksittäinen pinni
			QListBox* itemListBox=listItem->listBox();
			int currentItem=itemListBox->currentItem();
			QString currentTxt=netKListBox_->currentText();
			selectedConnectionTxt_=currentTxt.left(4);
			if(connectUnconnectPinsIsOn_)
			{
				//Haetaan selectiota vastaava pinnitieto
				if(currentItem>=0)
				{
					IOPin* apuPin;
					apuPin=netPinList_.pToFirstPin();
					for(int n=0;n<currentItem;n++)      //Vieadään pointteri oikean itemin kohdalle
						apuPin=apuPin->next();
					nextPinToConnect_=apuPin;
				}
			}
			selectConnection(selectedConnectionTxt_);
		}
	}
	if(button==QMouseEvent::RightButton)
	{
		addNetPin(IOPin::PINTYPE_BIDIR_LEFT, false); //Luodaan vain kaksisuuntaisia nettejä
	}
}

void ExternalW::netKListBoxDoubleClicked (QListBoxItem *item, const QPoint &pos)
{
	if(item)
	{
	}
}

void ExternalW::addNets(IOPin* nets)
{
	IOPin* apuP=nets;
	apuP=apuP->next(); //0 netti
	apuP=apuP->next(); //1 netti
	while(apuP!=0)
	{
		addNetPin(apuP->readType(), apuP->readValue(false));
		apuP=apuP->next();
	}
}

void ExternalW::changeNetTextConnectingPin(int const  index, const QString& stringToAdd)
{
	//Lisätään nettipinnilistaan komponentti ja sen pinni 0;EN,2;/C
	QString netPinTextLine=netKListBox_->text(index);
	if(netPinTextLine.length()>5)
		netPinTextLine+=",";
	else
		netPinTextLine+=":";
	netPinTextLine+=stringToAdd;
	QPixmap px=*netKListBox_->pixmap(index);
	netKListBox_->changeItem(px, netPinTextLine, index);
}

void ExternalW::changeNetTextUnconnectingPin(int const  index, const QString& stringToRemove)
{
	QString netPinTextLine=netKListBox_->text(index);
	netPinTextLine.remove(stringToRemove);
	netPinTextLine.replace(",,", ",");
	int l=netPinTextLine.length();
	if(l>5)
	{
		if(netPinTextLine[l-1]==',')
			netPinTextLine.truncate(l-1);
		if(netPinTextLine[5]==',')
			netPinTextLine.remove(5, 1);
	}
	l=netPinTextLine.length();
	if(l==5)
		netPinTextLine.truncate(l-1);
	QPixmap px=*netKListBox_->pixmap(index);
	netKListBox_->changeItem(px, netPinTextLine, index);
}

//-------------------------END net

//Käy listan lävi ja päivittää ledit. Yhteinen pinListBoxin, portListBoxin ja netListboxin rutiini
void ExternalW::updatePinKListBox(KListBox* listBox, IOPin* pinList)
{
	KPixmap icon;
	QString pname;
	int currentI=listBox->currentItem();
	int index=0;
	IOPin* pinP=pinList;
	while(pinP!=0)
	{
		if(pinP->readValue(false))
			icon=ledOnIconKPixmap_;
		else
			icon=ledOffIconKPixmap_;
		pname=listBox->text(index);
		listBox->changeItem(icon, pname, index);
		pinP=pinP->next();
		index++;
	}
	if(currentI>-1)
	{
		listBox->setCurrentItem(currentI);
		listBox->ensureCurrentVisible();
	}
}

void ExternalW::showR0(void)
{
	activatePointerWindow(ACTIVATE_R0);
}

void ExternalW::showR1(void)
{
	activatePointerWindow(ACTIVATE_R1);
}

void ExternalW::showDPTR(void)
{
	activatePointerWindow(ACTIVATE_DPTR);
}

//Lisätään komponetit menu. Luo myös komponenttiluokan
void ExternalW::addComponentsPopupMenu(void)
{
	int id;
	contextKPopupMenu=new KPopupMenu(this);
	//Add
	addKPopupMenu = new KPopupMenu(this);
	contextKPopupMenu->insertItem(i18n("&Add"), addKPopupMenu);

	componentLibrary_ = new ComponentsLib(this, addKPopupMenu, windowWorkspace_);
	connect(componentLibrary_, SIGNAL(componentCreated(ExternalComponentBaseW*)), this, SLOT(componentCreatedFromLibrary(ExternalComponentBaseW*)));

	contextKPopupMenu->insertSeparator();

	//Window
	contextKPopupMenu->insertItem( i18n("&Windows"), windowsMenu_); //windowsMenu_ kantaluokasta
}

void ExternalW::componentCreatedFromLibrary(ExternalComponentBaseW *pointerToCreatedComponent)
{
	pToExternalComponentW_=pointerToCreatedComponent;//componentLibrary_->readPointterToLastCreatedComponent();
	pToExternalComponentW_->writePointers(&externalCyc_, &someDataPointerValueChanged_, &dptrValue_, &r0Value_, &r1Value_, &lastStepTime_, &lastMachineCycles_, &programCounterValue_);
	emit message_Update(this->name());
	//emit messageToProcessor8051W(this->name(), I51BaseW::UPDATE, "Only update", 0, 0.0);
}

//Lisätään ulkoisesti komponentit kopiotoiminnassa tai työn latauksella
void ExternalW::addComponent(unsigned long const  type, const QString& name)
{
	componentLibrary_->addComponent(type, name);
}

//Tuodaan lista komponenteista kopioitavasta processorista
void ExternalW::addComponents(QWidgetList listOfComponents)
{
	if(listOfComponents.count())
		this->showNormal();
	ExternalComponentBaseW *copyComponent;
	for ( int i = 0; i < int(listOfComponents.count()); ++i )
	{
		copyComponent = (ExternalComponentBaseW*)listOfComponents.at(i);
		addComponent(copyComponent->readType(), copyComponent->name());

		//Jos external data osoiteavaruutta
		if(copyComponent->isAtExternalDataSpace())
			pToExternalComponentW_->writeExternalDataSpaceStartAddress(copyComponent->readExternalDataSpaceStartAddress());
		//Jos external code osoiteavaruutta
		if(copyComponent->isAtExternalCodeSpace())
			pToExternalComponentW_->writeExternalCodeSpaceStartAddress(copyComponent->readExternalCodeSpaceStartAddress());
		//Jos IO- avaruutta
		if(copyComponent->isAtIOSpace())
			pToExternalComponentW_->copyIOPins(copyComponent->readIOPinsList());

		//Geometria samanlaisiksi kuin kopioitavassa?
		QPoint cp=copyComponent->parentWidget()->pos();
		QRect cr=copyComponent->frameGeometry();
		unsigned int cWs=copyComponent->windowState();
		pToExternalComponentW_->setWindowState(cWs);
		pToExternalComponentW_->setGeometry(cr);
		pToExternalComponentW_->parentWidget()->move(cp);
	}
}


void ExternalW::hideShowToolsButtons(void)
{
	if(readWindowsCounterValue()>0)
	{
		toolsTools_->setItemEnabled(toolsTools_ConnectUnconnectPins_ID_, true);
		toolsTools_->setItemEnabled(toolsTools_ShowComponentChangesWhenRunning_ID_, true);
	// toolsTools_->setItemEnabled(toolsTools_ShowIOChangesWhenRunning_ID_, true);
	}
	else
	{
		toolsTools_->setItemEnabled(toolsTools_ConnectUnconnectPins_ID_, false);
		toolsTools_->setItemEnabled(toolsTools_ShowComponentChangesWhenRunning_ID_, false);
	// toolsTools_->setItemEnabled(toolsTools_ShowIOChangesWhenRunning_ID_, false);
	}
}


void ExternalW::mousePressEvent( QMouseEvent *e )
{
	//QPoint mousePosition=e->pos();
	QPoint mousePosition=e->globalPos();
	if((e->button() == QMouseEvent::RightButton))
	{
		contextKPopupMenu->popup(mousePosition);
	}
	if((e->button() == QMouseEvent::LeftButton))
	{
	}
	WorkspaceMainW::mousePressEvent(e);
}

void ExternalW::connectUnconnectPinsButtonPressed(void)
{
	if(toolsTools_->isButtonOn(toolsTools_ConnectUnconnectPins_ID_))
	{
		connectUnconnectPinsIsOn_=true;
		ioPinsKListBox_->clearSelection();
		portsKListBox_->clearSelection();
	}
	else
	{
		connectUnconnectPinsIsOn_=false;
	}
}

void ExternalW::showComponentChangesWhenRunningButtonPressed(void)
{
	bool value=false;
	if(toolsTools_->isButtonOn(toolsTools_ShowComponentChangesWhenRunning_ID_))
		value=true;
	showComponentChangesWhenRunning_=value;
	if(value)
		executeAndUpdateExternalAreaComponents(lastStepTime_, false, true, true); //Ei executeta
}

void ExternalW::showIOChangesWhenRunningButtonPressed(void)
{
	bool value=false;
	if(toolsTools_->isButtonOn(toolsTools_ShowIOChangesWhenRunning_ID_))
		value=true;
	showIOChangesWhenRunning_=value;
	if(value)
		executeAndUpdateExternalAreaComponents(lastStepTime_, false, true, true); //Ei executeta
}


void ExternalW::connetNextPinToComponent(void)
{
	ExternalComponentBaseW* aw;
	aw=(ExternalComponentBaseW*)windowWorkspace_->activeWindow();
	aw->connectPin(nextPinToConnect_);
}

void ExternalW::unconnetComponentActivePin(void)
{
	ExternalComponentBaseW* aw;
	aw=(ExternalComponentBaseW*)windowWorkspace_->activeWindow();
	aw->unconnectPin();
}

void ExternalW::selectConnection(const QString &connectionText)
{
	QString cTxt=connectionText;
	//First we select component I/O pins.
	QWidgetList extCWin = windowWorkspace_->windowList();
	if(!extCWin.count() )
		return;
	ExternalComponentBaseW *extC;
	int i;
	for(i=0; i<int(extCWin.count()); i++)
	{
		extC=(ExternalComponentBaseW*)extCWin.at(i);
		extC->showConnectedPin(cTxt);
	}
	//prosessor single I/O's
	QString pinTxt;
	bool hits=false;
	for(i=0; i<ioPinsKListBox_->count();i++)
	{
		pinTxt=ioPinsKListBox_->text(i);
		int txtLength=pinTxt.length();
		ioPinsKListBox_->setCurrentItem(i);
		if(pinTxt.right(txtLength-2)==cTxt)
		{
			ioPinsKListBox_->ensureCurrentVisible();
			hits=true;
			break;
		}
	}
	if(!hits)
		ioPinsKListBox_->clearSelection();
	//processor port I/O's
	hits=false;
	for(i=0; i<portsKListBox_->count();i++)
	{
		pinTxt=portsKListBox_->text(i);
		portsKListBox_->setCurrentItem(i);
		if(pinTxt==cTxt)
		{
			portsKListBox_->ensureCurrentVisible();
			hits=true;
			break;
		}
	}
	if(!hits)
		portsKListBox_->clearSelection();
	//Connection nets
	hits=false;
	for(i=0; i<netKListBox_->count();i++)
	{
		pinTxt=netKListBox_->text(i);
		netKListBox_->setCurrentItem(i);
		if(cTxt==pinTxt.left(4))
		{
			netKListBox_->ensureCurrentVisible();
			hits=true;
			break;
		}
	}
	if(!hits)
		netKListBox_->clearSelection();
	
}

//Viestit komponenteilta------
void ExternalW::messageFromComponent_Created(const QString& name)
{
	countUpWindowsCounter();
	hideShowToolsButtons();
}

void ExternalW::messageFromComponent_Destroyed(const QString& name)
{
	countDownWindowsCounter();
	hideShowToolsButtons();
}

void ExternalW::messageFromComponent_PinConnect(const QString& name, const int pinID, const QString& pinName)
{
	QString sCTxt=pinName;
	if(connectUnconnectPinsIsOn_)
	{
		if(nextPinToConnect_)
		{
			//Ei kytketä net listin 0 ja 1 pinniä komponentin OUTPUTTIIN
			IOPin::PinID pid=nextPinToConnect_->readID();
			if(((pinID>=ExternalComponentBaseW::OUTPUTPIN_ID_OFFSET) && (pinID<ExternalComponentBaseW::BIDIRPIN_ID_OFFSET_LEFT)) && ((pid.id_==NETPIN_START_ID) || (pid.id_==NETPIN_START_ID+1)))
			{
				statusBar()->message( i18n("Sorry, couldn't connect to component output."));
			}
			else
			{
				unconnetComponentActivePin();
				connetNextPinToComponent();
			}
			sCTxt=selectedConnectionTxt_;
		}
	}
	else
		selectedConnectionTxt_=sCTxt;
	selectConnection(sCTxt);
}

void ExternalW::messageFromComponent_PinUnconnect(const QString& name)
{
	if(connectUnconnectPinsIsOn_)
		unconnetComponentActivePin();
}

void ExternalW::messageFromComponent_PinConnected(const QString& name, const int pinID, const QString& pinName)
{
	if(pinID>=NETPIN_START_ID) //Jos nettipinni
		changeNetTextConnectingPin(pinID-NETPIN_START_ID, pinName);
}

void ExternalW::messageFromComponent_PinUnconnected(const QString& name, const int pinID, const QString& pinName)
{
	if(pinID>=NETPIN_START_ID) //Jos nettipinni
		changeNetTextUnconnectingPin(pinID-NETPIN_START_ID, pinName);
}

void ExternalW::messageFromComponent_PinCopy(const QString& name, const int pinID)
{
	ExternalComponentBaseW* aw;
	aw=(ExternalComponentBaseW*)windowWorkspace_->activeWindow();
	IOPin* apuPin{nullptr};
	if(pinID>=NETPIN_START_ID)
		apuPin=netPinList_.pToFirstPin();
	else
		if(pinID>=PORTPIN_START_ID)
			apuPin=portPinList_;
		else
			if(pinID>=PIN_START_ID)
				apuPin=ioPinListProcessor_->pToFirstPin();
	while(apuPin!=0)
	{
		IOPin::PinID pid=apuPin->readID();
		if(pid.id_==pinID)
		{
			aw->connectPin(apuPin);
			break;
		}
		apuPin=apuPin->next();
	}
}

void ExternalW::messageFromComponent_MemoryValueChanged(const QString& name, const unsigned long address, const int value)
{
	emit message_MemoryValueChanged(name, address, value);
}

void ExternalW::messageFromComponent_FocusProgramMemoryAddress(const QString& name, const unsigned long address)
{
	emit message_FocusProgramMemoryAddress(name, address);
	//emit messageToProcessor8051W(name, I51BaseW::FOCUS_PROGRAMMEMORY_ADDRESS, name, address, 0.0);
}


void ExternalW::componentActivated()
{
	emit message_WindowLevel(this->name(), KSimu51_NameS::WINDOWLEVEL_EXTERNAL);
	//emit messageToProcessor8051W("externalWorkspace", I51BaseW::WINDOW_LEVEL, "*", KSimu51_NameS::WINDOWLEVEL_EXTERNAL, 0.1);
}

void ExternalW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::DOUBLE<<(double)lastStepTime_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)lastMachineCycles_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)programCounterValue_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)lastClockTicks_;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)dptrValue_;
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)r0Value_;
	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)r1Value_;
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)wAddress_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

	externalDataMemoryAreaInT_->saveWorkDataToFile(saveFileStream);
	externalDataMemoryAreaOutT_->saveWorkDataToFile(saveFileStream);
	externalCodeMemoryAreaOutT_->saveWorkDataToFile(saveFileStream);

	//NetPinnien määrä
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)netPinList_.numberOfPins();
	IOPin* apuP=netPinList_.pToFirstPin();
	//Ei talleteta 0 ja 1 pinniä
	apuP=apuP->next();
	apuP=apuP->next();
	while(apuP!=0)
	{
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)apuP->readValue(false); //Value
		apuP=apuP->next();
	}
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

	//Ulkoisen alueen komponentit
	QWidgetList eac=listOfComponents();
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT16<<(Q_UINT16)eac.count();   //Komponenttien määrä
	for(unsigned int i=0; i<((unsigned int)eac.count()); i++)
	{
		ExternalComponentBaseW *component = (ExternalComponentBaseW*)eac.at(i);
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)component->readType();
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::CHARSTRING<<component->name();
		component->saveWorkDataToFile(saveFileStream);

		//Tallennetaan komponenttien geometria
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QPOINT<<component->parentWidget()->pos();
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QRECT<<component->frameGeometry();
		saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT16<<(Q_UINT16)component->windowState();
		saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
	}
}

void ExternalW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT16 value16;
	Q_UINT8 value8;
	double valueD;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>valueD;
				lastStepTime_=valueD;
				break;
			case 1:
				readFileStream>>typeReadData>>value32;
				lastMachineCycles_=value32;
				break;
			case 2:
				readFileStream>>typeReadData>>value32;
				programCounterValue_=value32;
				break;
			case 3:
				readFileStream>>typeReadData>>value32;
				lastClockTicks_=value32;
				break;
			case 4:
				readFileStream>>typeReadData>>value32;
				dptrValue_=(unsigned int)value32;
				break;
			case 5:
				readFileStream>>typeReadData>>value8;
				r0Value_=(unsigned char)value8;
				break;
			case 6:
				readFileStream>>typeReadData>>value8;
				r1Value_=(unsigned char)value8;
				break;
			case 7:
				readFileStream>>typeReadData>>value32;
				wAddress_=(unsigned int)value32;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);

	externalDataMemoryAreaInT_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	externalDataMemoryAreaOutT_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	externalCodeMemoryAreaOutT_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	//Net pins
	readFileStream>>indexReadData>>typeReadData>>value32;
	for(int n=0; n<value32-2; n++) //Vähennetään kokonaismäärästä 0 ja 1 netti pois
	{
		readFileStream>>indexReadData>>typeReadData>>value8;
		addNetPin(IOPin::PINTYPE_BIDIR_LEFT, (bool)value8);
	}
	readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END

	char* cName;
	readFileStream>>indexReadData>>typeReadData>>value16;
	for(unsigned int i=0; i<(unsigned int)value16; i++)
	{
		readFileStream>>indexReadData>>typeReadData>>value32;
		readFileStream>>indexReadData>>typeReadData>>cName;
		addComponent(value32,cName);
		pToExternalComponentW_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

		QPoint pos;
		QRect rect;
		Q_UINT16 wState;
		bool done=false;
		do
		{
			readFileStream>>indexReadData;
			switch(indexReadData)
			{
				case 0:
					readFileStream>>typeReadData>>pos;
					pToExternalComponentW_->parentWidget()->move(pos);
					break;
				case 1:
					readFileStream>>typeReadData>>rect;
					pToExternalComponentW_->setGeometry(rect);
					break;
				case 2:
					readFileStream>>typeReadData>>wState;
					pToExternalComponentW_->setWindowState(wState);
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
}

