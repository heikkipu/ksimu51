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
#include "AssemblerSourceW.h"
#include "I51BaseW.h"
#include <kpopupmenu.h>
#include <klocale.h>
#include <kwin.h>

#include <qframe.h>
#include <qlabel.h>
#include <qstringlist.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <qtooltip.h>
#include "../ksimu51.h"



//Tab widgetti
AsmSourceQWidget::AsmSourceQWidget(QWidget * parent, const char * name, WFlags f):QWidget(parent, name, f)
{
	QGridLayout* grid=new QGridLayout(this, 1, 1, 0, 0);

	//Source list BOX
	asmSourceKListBox_ = new KListBox(this, name);
	grid->addWidget(asmSourceKListBox_, 0, 0, 0);
	setFocusProxy(asmSourceKListBox_);
	QFont asmSourceKListBox_font(  asmSourceKListBox_->font() );
	asmSourceKListBox_font.setFamily( FONTFAMILY );
	asmSourceKListBox_font.setPointSize(10);
	asmSourceKListBox_->setFont( asmSourceKListBox_font );
	asmSourceKListBox_->setFrameShape( KActiveLabel::StyledPanel );
	asmSourceKListBox_->setFrameShadow( KActiveLabel::Sunken );
	asmSourceKListBox_->setSelectionMode(QListBox::Single);
	asmSourceKListBox_->clear();

	breakpointIcon_.load ( "pics/KS51hi16StopProcessor.png", 0, KPixmap::Auto );
	executionpointIcon_.load ( "pics/KS51hi16RunProcessor.png", 0, KPixmap::Auto );
	programCounterIcon_.load ( "pics/KS51hi16WhiteLedOn.png", 0, KPixmap::Auto );

	programCounterLineNum_=0;
}

AsmSourceQWidget::~AsmSourceQWidget()
{
}

unsigned long AsmSourceQWidget::insertItem(string const& sitem)
{
	asmSourceKListBox_->insertItem(sitem);
	string addressStr(&sitem[8],&sitem[12]);
	QString aQStr=addressStr;
	bool ok;
	unsigned int ui=aQStr.toUInt(&ok,16);
	if(ok)
	{
		addressInSourceKListBoxLineQValueList_.append(ui);
		asmSourceKListBoxLineInAddress_[ui]=asmSourceKListBox_->count()-1;
		return ui;//Palauttaa rivin osoitteen
	}
	unsigned long na=NO_ADDRESS;
	addressInSourceKListBoxLineQValueList_.append(na);
	return na;//Jos ei osoitetta
}

void AsmSourceQWidget::focusLine(unsigned long const address)
{
	asmSourceKListBox_->setCurrentItem(asmSourceKListBoxLineInAddress_[address]);
	asmSourceKListBox_->ensureCurrentVisible();
}

unsigned long AsmSourceQWidget::addressInCurrentActiveLine(void)
{
	return addressInSourceKListBoxLineQValueList_[asmSourceKListBox_->currentItem()];
}

int AsmSourceQWidget::currentLineNumber(void)
{
	return asmSourceKListBox_->currentItem();
}

void AsmSourceQWidget::setIcon(lineIcon const iconToSet, bool const onOff, unsigned long const address)
{
	KPixmap setIcon;
	switch(iconToSet)
	{
		case BREAKPOINT_ICON:
			setIcon=breakpointIcon_;
			break;
		case EXECUTIONPOINT_ICON:
			setIcon=executionpointIcon_;
			break;
		case PROGRAMCOUNTERPOINTER_ICON:
			setIcon=programCounterIcon_;
			break;
		default:
			return;
	}
	int lineIndex=asmSourceKListBoxLineInAddress_[address];
	QString lineText=asmSourceKListBox_->text(lineIndex);
	if(onOff)
		asmSourceKListBox_->changeItem(setIcon, lineText, lineIndex);
	else
		asmSourceKListBox_->changeItem(lineText, lineIndex);
}


//Asm Tab
SourceTabQTabWidget::SourceTabQTabWidget(QWidget * parent, const char * name, WFlags f) : QTabWidget(parent, name, f)
{
	asmSourceExecutionPointAddress_=0;
	programCounterPointerPrevAddress_=0;
	for(unsigned long n=0;n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE;n++)
		asmSourceBreakpointTable_[n]=false;

}

SourceTabQTabWidget::~SourceTabQTabWidget()
{
	vectorToAsmSourceQWidget_.clear();
}

void SourceTabQTabWidget::clear(void)
{
	while(count())
	{
		AsmSourceQWidget* w = vectorToAsmSourceQWidget_[currentPageIndex()];
		//delete w; w=0;
		//w->clear();
		removePage(page(currentPageIndex()));
	}
	vectorToAsmSourceQWidget_.resize(0);

	for(unsigned long n=0;n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE;n++)
		asmSourceBreakpointTable_[n]=false;
}

int SourceTabQTabWidget::addNewSourceTab(const QString & label )
{
	asmSourceQWidget_ = new AsmSourceQWidget(this, label, 0);
	insertTab(asmSourceQWidget_, label, -1);
	showPage(asmSourceQWidget_);

	vectorToAsmSourceQWidget_.resize(currentPageIndex()+1);
	vectorToAsmSourceQWidget_.insert(currentPageIndex(), asmSourceQWidget_);

	return currentPageIndex();
}

void SourceTabQTabWidget::insertSourceLine(int const tabNum, string const& sitem)
{
	if(tabNum<count())
	{
		AsmSourceQWidget* w = vectorToAsmSourceQWidget_[tabNum];
		unsigned long rv=w->insertItem(sitem);
		if(rv!=AsmSourceQWidget::NO_ADDRESS)
			asmSourceKTabInAddress_[rv]=tabNum;
	}
}

void SourceTabQTabWidget::focusLine(unsigned long const address)
{
	if((address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE)&&(count()))
	{
		setCurrentPage(asmSourceKTabInAddress_[address]);
		AsmSourceQWidget* w=vectorToAsmSourceQWidget_[currentPageIndex()];
		w->focusLine(address);
	}
}

long SourceTabQTabWidget::addressInCurrentPageSourceListActiveLine(void)
{
	if(count())
	{
		AsmSourceQWidget* w = vectorToAsmSourceQWidget_[currentPageIndex()];
		return w->addressInCurrentActiveLine();
	}
	return -1;
}

int SourceTabQTabWidget::currentPageSourceListActiveLineNumber(void)
{
	if(count())
	{
		AsmSourceQWidget* w = vectorToAsmSourceQWidget_[currentPageIndex()];
		return w->currentLineNumber();
	}
	return -1;
}

void SourceTabQTabWidget::toggleBreakpoint(unsigned long const address)
{
	if((address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE)&&count())
	{
		setCurrentPage(asmSourceKTabInAddress_[address]);
		AsmSourceQWidget* w=vectorToAsmSourceQWidget_[currentPageIndex()];
		if(asmSourceBreakpointTable_[address])
		{
			asmSourceBreakpointTable_[address]=false;
			w->setIcon(AsmSourceQWidget::BREAKPOINT_ICON, false, address);
		}
		else
		{
			asmSourceBreakpointTable_[address]=true;
			w->setIcon(AsmSourceQWidget::BREAKPOINT_ICON, true, address);
		}
	}
}

void SourceTabQTabWidget::removeBreakpoints(void)
{
	for(unsigned int address=0; address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE;address++)
	{
		if(asmSourceBreakpointTable_[address])
		{
			setCurrentPage(asmSourceKTabInAddress_[address]);
			AsmSourceQWidget* w=vectorToAsmSourceQWidget_[currentPageIndex()];
			asmSourceBreakpointTable_[address]=false;
			w->setIcon(AsmSourceQWidget::BREAKPOINT_ICON, false, address);
		}
	}
}

bool SourceTabQTabWidget::isBreakpoint(unsigned long const address)
{
	if(address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE)
	{
		return asmSourceBreakpointTable_[address];
	}
	return false;
}

void SourceTabQTabWidget::clearExecutionPoint(void)
{
	if(count())
	{
		setCurrentPage(asmSourceKTabInAddress_[asmSourceExecutionPointAddress_]);
		AsmSourceQWidget* w=vectorToAsmSourceQWidget_[currentPageIndex()];
		w->setIcon(AsmSourceQWidget::EXECUTIONPOINT_ICON, false, asmSourceExecutionPointAddress_);
		//Katotaan oliko paikalla Breakpointti
		if(asmSourceBreakpointTable_[asmSourceExecutionPointAddress_])
			w->setIcon(AsmSourceQWidget::BREAKPOINT_ICON, true, asmSourceExecutionPointAddress_);
	}
}

void SourceTabQTabWidget::setExecutionPoint(unsigned long const address)
{
	if((address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE)&&count())
	{
		clearExecutionPoint();
		setCurrentPage(asmSourceKTabInAddress_[address]);
		AsmSourceQWidget* w=vectorToAsmSourceQWidget_[currentPageIndex()];
		asmSourceExecutionPointAddress_=address;
		w->setIcon(AsmSourceQWidget::EXECUTIONPOINT_ICON, true, asmSourceExecutionPointAddress_);
	}
}

void SourceTabQTabWidget::setProgramCounterPointer(unsigned long const address)
{
	if((address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE)&&count())
	{
		AsmSourceQWidget *w;
		setCurrentPage(asmSourceKTabInAddress_[programCounterPointerPrevAddress_]);
		w=vectorToAsmSourceQWidget_[currentPageIndex()];
		w->setIcon(AsmSourceQWidget::PROGRAMCOUNTERPOINTER_ICON, false, programCounterPointerPrevAddress_);
		if(asmSourceBreakpointTable_[programCounterPointerPrevAddress_])
			w->setIcon(AsmSourceQWidget::BREAKPOINT_ICON, true, programCounterPointerPrevAddress_);
		if(asmSourceExecutionPointAddress_==programCounterPointerPrevAddress_)
			w->setIcon(AsmSourceQWidget::EXECUTIONPOINT_ICON, true, programCounterPointerPrevAddress_);
		
		setCurrentPage(asmSourceKTabInAddress_[address]);
		w=vectorToAsmSourceQWidget_[currentPageIndex()];
		w->setIcon(AsmSourceQWidget::PROGRAMCOUNTERPOINTER_ICON, true, address);
		programCounterPointerPrevAddress_=address;
	}
}

KListBox* SourceTabQTabWidget::pointerToCurrentAsmSourceQWidgetKListBox(void) const
{
	if(count())
	{
		AsmSourceQWidget* w = vectorToAsmSourceQWidget_[currentPageIndex()];
		return w->pointerToAsmSourceKListBox();
	}
	return 0;
}

KListBox* SourceTabQTabWidget::pointerToAsmSourceQWidgetKListBox(int const tabNum)
{
	if((tabNum<count())&&(tabNum>=0))
	{
		AsmSourceQWidget* w = vectorToAsmSourceQWidget_[tabNum];
		return w->pointerToAsmSourceKListBox();
	}
	return 0;
}

void SourceTabQTabWidget::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//Breakpoint table
	for(unsigned long n=0; n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE; n++)
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)asmSourceBreakpointTable_[n];
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void SourceTabQTabWidget::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	//Breakpoint taulukko
	Q_UINT8 value8;
	for(unsigned long n=0; n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE; n++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8;
		asmSourceBreakpointTable_[n]=(bool)value8;
		//Päivitetään breakpointti näkyviin
		if(value8)
		{
			setCurrentPage(asmSourceKTabInAddress_[n]);
			AsmSourceQWidget* w=vectorToAsmSourceQWidget_[currentPageIndex()];
			w->setIcon(AsmSourceQWidget::BREAKPOINT_ICON, true, n);
		}
	}

	readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END
}

//Named BitListWidget
NamedBitListWidget::NamedBitListWidget(QWidget* parent, char const * name, int wflags) : QWidget(parent, name, wflags)
{
	QGridLayout* grid=new QGridLayout(this, 1, 1, 0, 0);

	namedBitsKListBox_ = new KListBox(this, "namedBitsKListBox_");
	QToolTip::add(namedBitsKListBox_,"Dpl click, toggle bit.\nRight click, toggle breakpoint.");
	QFont namedBitsKListBox_font(namedBitsKListBox_->font());
	namedBitsKListBox_font.setFamily(FONTFAMILY);
	namedBitsKListBox_font.setPointSize(10);
	namedBitsKListBox_->setFont(namedBitsKListBox_font);
	namedBitsKListBox_->setFrameShape( QFrame::GroupBoxPanel );
	//namedBitsKListBox_->setMaximumWidth(170);
	//namedBitsKListBox_->setMinimumHeight(50);
	setMaximumWidth(170);
	setMinimumHeight(50);
	namedBitsKListBox_->setSelectionMode(QListBox::Single);

	grid->addWidget(namedBitsKListBox_, 0, 0);

	ledOnIconKPixmap_.load("pics/KS51hi16CyanLedOn.png", 0, KPixmap::Auto );
	ledOffIconKPixmap_.load("pics/KS51hi16CyanLedOff.png", 0, KPixmap::Auto );
	breakpointLedOnKPixmap_.load("pics/KS51hi16RedBreakpointLedOn.png", 0, KPixmap::Auto);
	breakpointLedOffKPixmap_.load("pics/KS51hi16RedBreakpointLedOff.png", 0, KPixmap::Auto);

	firstNamedBitToAdd_=true;

	connect(namedBitsKListBox_, SIGNAL(doubleClicked (QListBoxItem* , const QPoint&)), this, SLOT(namedBitsKListBoxDoubleClicked(QListBoxItem* , const QPoint&)));
	connect(namedBitsKListBox_, SIGNAL(rightButtonClicked(QListBoxItem*, const QPoint&)), this, SLOT(namedBitsKListBoxRightButtonClicked(QListBoxItem* , const QPoint&)));

}

NamedBitListWidget::~NamedBitListWidget()
{
	if(!firstNamedBitToAdd_)//Täytyy olla mitä deletoida
	{
		delete namedBitList_;
	}
}

void NamedBitListWidget::clear(void)
{
	namedBitsKListBox_->clear();
	if(!firstNamedBitToAdd_)
	{
		//Deletoidaan vanhat listat.
		delete namedBitList_;
		//delete namedBitListLastMember_;
		//Uusi lista alulle
		firstNamedBitToAdd_=true;
	}
}

void NamedBitListWidget::addNamedBit(unsigned int const bitAddress, string const bitName, bool const  bitValue)
{
	//Luodaan ensin jäsen listaan
	IOPortPin* namedBitToAdd;
	namedBitToAdd = new IOPortPin(bitAddress, bitName, namedBitToAdd->NO_ID());
	if(firstNamedBitToAdd_) //Jos ensimmäinen kerta kutsuvaan ohjelmaan
	{
		namedBitList_=new IOPortPin();
		namedBitListLastMember_=new IOPortPin();
		namedBitListLastMember_=namedBitToAdd;
		namedBitList_=namedBitListLastMember_;
		firstNamedBitToAdd_=false;
	}
	else
	{
		namedBitListLastMember_->setNextPin(namedBitToAdd);
		namedBitListLastMember_=namedBitToAdd;
	}
	namedBitListLastMember_->writeValue(bitValue, false);
	//Asetetaan Tieto List Boxiin
	KPixmap icon;
	if(bitValue)
		icon=ledOnIconKPixmap_;
	else
		icon=ledOffIconKPixmap_;
	namedBitsKListBox_->insertItem(icon, bitName, -1 );
}

//Vaihdetaan lediconi lista boxiin
void NamedBitListWidget::changeLedAtListBox(KPixmap const ledIcon, int const index)
{
	QString pname(namedBitsKListBox_->text(index));
	namedBitsKListBox_->changeItem(ledIcon, pname, index);
}

bool NamedBitListWidget::readNamedBitListValue(int const index)
{
	int rows=namedBitsKListBox_->numRows();
	if(index<rows)
	{
		IOPortPin* namedBitListToFind;
		namedBitListToFind=namedBitList_;
		for(int n=0; n<index; n++)
			namedBitListToFind=namedBitListToFind->next();
		return namedBitListToFind->readValue(false);
	}
	return false;
}

unsigned int NamedBitListWidget::readNamedBitListAddress(int const index)
{
	int rows=namedBitsKListBox_->numRows();
	if(index<rows)
	{
		IOPortPin* namedBitListToFind;
		namedBitListToFind=namedBitList_;
		for(int n=0; n<index; n++)
			namedBitListToFind=namedBitListToFind->next();
		return namedBitListToFind->readAddress();
	}
	return 0xFFFF;
}

//Päivittää nimetyt bitialueen bitit
void NamedBitListWidget::updateNamedBits(unsigned int const byteAddress, unsigned char const byteValue, BitChangeBreakpoint* const breakpointBitList)
{
	//Käyttää ioPortPin luokkaa joka on bittilista
	if(!firstNamedBitToAdd_) //Täytyy olla lista olemassa jotta voi päivittää
	{
		IOPortPin* namedBitToSet;
		KPixmap icon;
		int currentI=namedBitsKListBox_->currentItem();
		unsigned int index=0;
		namedBitToSet=namedBitList_;
		while(namedBitToSet!=0)
		{
			int namedBitAddress=namedBitToSet->readAddress();
			bool bitValue;
			//Jos listan bitin osoite sattuu päivitettävälle alueelle
			if((namedBitAddress>>8)==byteAddress)
			{
				if((byteValue>>(namedBitAddress&0x0F))&0x01)
				{
					if(breakpointBitList->isBitBreakpointBit(namedBitAddress))
						icon=breakpointLedOnKPixmap_;
					else
						icon=ledOnIconKPixmap_;
					bitValue=true;
				}
				else
				{
					if(breakpointBitList->isBitBreakpointBit(namedBitAddress))
						icon=breakpointLedOffKPixmap_;
					else
						icon=ledOffIconKPixmap_;
					bitValue=false;
				}
				namedBitToSet->writeValue(bitValue, false);
				changeLedAtListBox(icon, index);
			}
			index++; //
			namedBitToSet=namedBitToSet->next();
		}
		if(currentI>-1)
		{
			namedBitsKListBox_->setCurrentItem(currentI);
			namedBitsKListBox_->ensureCurrentVisible();
		}
	}
}

bool NamedBitListWidget::showBitChangeBreakpoint(unsigned int const bitAddress)
{
	if(!firstNamedBitToAdd_) //Täytyy olla lista olemassa jotta voi tehdä
	{
		IOPortPin* namedBitToShow;
		unsigned int index=0;
		unsigned int correctedBitAddress=bitAddress&0xFF0F;
		namedBitToShow=namedBitList_;
		while(namedBitToShow!=0)
		{
			int namedBitAddress=namedBitToShow->readAddress();
			//Jos listan bitin osoite sattuu päivitettävälle alueelle
			if(namedBitAddress==correctedBitAddress)
			{
				namedBitsKListBox_->setCurrentItem(index);
				namedBitsKListBox_->ensureCurrentVisible();
				return true;
			}
			index++;
			namedBitToShow=namedBitToShow->next();
		}
	}
	return false;
}

void NamedBitListWidget::namedBitsKListBoxDoubleClicked (QListBoxItem* item, const QPoint& pos)
{
	//Lähettää viestin MDI ikkunalle pinnin tilanvaihdosta.
	if(item)
	{
		//Muunnos QString > string
		QString listText(item->text());
		string messageString("");
		messageString=QChar(listText[0]);
		for(int n=1;n<listText.length();n++)
			messageString+=QChar(listText[n]);

		int currItem=namedBitsKListBox_->currentItem();
		unsigned int bitAddress=readNamedBitListAddress(currItem);
		emit bitDoubleClicked(messageString, bitAddress);
	}
}

void NamedBitListWidget::namedBitsKListBoxRightButtonClicked(QListBoxItem* item, const QPoint& pos)
{
	if(item)
	{
		if(item->isCurrent())
		{
			int currItem=namedBitsKListBox_->currentItem();
			unsigned int bitAddress=readNamedBitListAddress(currItem);
			emit bitRightMouseButtonClicked(bitAddress);
		}
	}
}

//MAIN
AssemblerSourceW::AssemblerSourceW( QWidget* parent, char const * name, int wflags )
	: ShareMainW( parent, name, wflags )
{
	setCentralWidget(new QWidget( this, "qt_central_widget" ) );
	asmSourceFormQGridLayout_ = new QGridLayout(centralWidget(), 1, 1, 1, 1, "asmSourceFormQGridLayout_");

	connect(this, SIGNAL(message_SetExecutionPoint(const QString& , const unsigned long )), this->parent()->parent()->parent() ,SLOT(messageFromWorkspaceW_SetExecutionPoint(const QString& , const unsigned long )));
	connect(this, SIGNAL(message_ToggleBit(const QString&, const QString&, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_ToggleBit(const QString&, const QString&, const int )));
	connect(this, SIGNAL(message_ToggleBreakpointBit(const QString&, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_ToggleBreakpointBit(const QString&, const int )));
	
	//Source list Tab
	asmSourceTabQTabWidget_ = new SourceTabQTabWidget(centralWidget(), "asmSourceTabQTabWidget_", 0);
	asmSourceFormQGridLayout_->addMultiCellWidget(asmSourceTabQTabWidget_, 0, 2, 0, 0, 0);
	connect(asmSourceTabQTabWidget_, SIGNAL(currentChanged(QWidget *)), this, SLOT(currentAsmSourceTabChanged(QWidget * )));

	//Named Bits List Box
	namedBitsListBoxQWidget_ = new NamedBitListWidget(centralWidget(), "namedBitsKListBox_", 0);
	asmSourceFormQGridLayout_->addWidget(namedBitsListBoxQWidget_, 0, 1);
	connect(namedBitsListBoxQWidget_, SIGNAL(bitDoubleClicked(const QString&, const int)), this, SLOT(bitsListBoxBitDoubleClicked(const QString&, const int)));
	connect(namedBitsListBoxQWidget_, SIGNAL(bitRightMouseButtonClicked(const unsigned int)), this, SLOT(bitListRightMouseButtonClicked(const unsigned int)));
	//Named Port Bits List Box
	namedPortPinListBoxQWidget_ = new NamedBitListWidget(centralWidget(), "namedPortPinKListBox", 0);
	asmSourceFormQGridLayout_->addWidget(namedPortPinListBoxQWidget_, 2, 1);
	connect(namedPortPinListBoxQWidget_, SIGNAL(bitDoubleClicked(const QString&, const int)), this, SLOT(bitsListBoxBitDoubleClicked(const QString&, const int)));
	connect(namedPortPinListBoxQWidget_, SIGNAL(bitRightMouseButtonClicked(const unsigned int )), this, SLOT(bitListRightMouseButtonClicked(const unsigned int)));

	breakPointIcon32KPixmap_.load("pics/KS51hi32StopProcessor.png", 0, KPixmap::Auto );
	executionPointIcon32KPixmap_.load("pics/KS51hi32RunProcessor.png", 0, KPixmap::Auto );
	findIcon32KPixmap_.load("pics/KS51hi32Find.png", 0, KPixmap::Auto );

	addPopupMenu();

	lineToFindText_=0;
	textToFind_="";

	lastFocusLineAddress_=0;
	lastPointedLineAddressInSourceTabWidget_=-1;

	//Bitin muutos breakpointtiluokka
	bitBreakpoint_=new BitChangeBreakpoint();
}

AssemblerSourceW::~AssemblerSourceW()
{
}

void AssemblerSourceW::clear(void)
{
	asmSourceTabQTabWidget_->clear();

	//addressInSourceKListBoxLineQValueList_.clear();
	lastFocusLineAddress_=0;
	lastPointedLineAddressInSourceTabWidget_=-1;

	namedBitsListBoxQWidget_->clear();
	namedPortPinListBoxQWidget_->clear();
}


bool AssemblerSourceW::addSourceFile(QString& sourceFilename, QString& activeHexFilename, I8031* processor)
{
	unsigned int lineN=1;
	processor->setProgramCounterValue(0);
	if(sourceFileParcer(sourceFilename, activeHexFilename, processor, &lineN))
		return true;
	return false;
}

//Rekursiivinen ohjelma. Mahdollistaa sisäkkäiset include sourcet
bool AssemblerSourceW::sourceFileParcer(QString& sourceFilename, QString& activeHexFilename, I8031* processor, unsigned int *lineNumber)
{
	const char* sfn = sourceFilename.ascii();
	ifstream sourceFile(sfn);
	string insertJono;
	char rivi[161];
	if(sourceFile.is_open())
	{
		//insertItem(decToString((*lineNumber)++,6,' ')+"Open: "+sourceFilename);
		QString asmTabName=sourceFilename.right(sourceFilename.length()-sourceFilename.findRev('/')-1);
		int tabPageNumber=asmSourceTabQTabWidget_->addNewSourceTab(asmTabName);

		connect(asmSourceTabQTabWidget_->pointerToAsmSourceQWidgetKListBox(tabPageNumber), SIGNAL(rightButtonClicked( QListBoxItem*, const QPoint&)), this, SLOT(asmSourceTabQTabWidgetRightButtonClicked(QListBoxItem*, const QPoint &  )));

		while(sourceFile.getline(rivi,160))
		{
			insertJono=decToString((*lineNumber)++,6,' ');
			insertJono+="  ";
			char c;
			int n=0;
			rivi[160]='\n';
			string apuStr("");

			//Otetaan kommentit, modit, includet ja DOS jäänteet pois Korvataan \t tyhjillä, jos on
			do
			{
				c=rivi[n];
				if((c!=';')&&(c!='\r'))
				{
					if(c!='\t')
						apuStr+=rivi[n];
					else
						apuStr+="   ";//Täytetään tabulaattorit välilyönneillä
				}
				n++;
			}
			while ((c!=';')&&(c!='$')&&(c!='\r')&&(n<160));

			//Asetetaan program counteri kohalleen
			QString qStrApu(apuStr);
			QString qStrApu2;
			QString qStrApu3;
			n=qStrApu.find(" org ", 0, false); //Ottaa huomioon myös ORG:n Välilyönnit erottaa LABELISTA
			if(n>=0)
			{      //Osoite pitää olla heksana h kirjaimen on löydyttävä
				int o=qStrApu.find('h', 0, false);
				if(o>n) //Ja h kirjaimen on oltava myöhempänä merkkijonossa
				{
					int m=qStrApu.findRev(' ', -(qStrApu.length()-o),false)+1;
					string strApu2(apuStr,m,o-m);
					processor->setProgramCounterValue(hexStringToHex(strApu2));
				}
			}

			//Katsotaan löytyykö bit tai BIT
			n=qStrApu.find(" bit ", 0, false);//Pitää ottaa huomioon sourcessa, että on välilyönnit oikein
			if(n>=0)
			{
				//Jos löytyy heitetään sourcen named listaan bitti mikäli se on bittiositettavasta alueesta
				//Bitin osoite
				qStrApu2=qStrApu;
				n=qStrApu2.length();
				while(((qStrApu2[n-1])==' ')||(qStrApu2[n-1])=='\t')
				{
					n--;
				}
				if(n<(qStrApu2.length()))
					qStrApu2.setLength(n);
				qStrApu3=qStrApu2.section(' ',-1,-1);

				//Jos oli heksaluku muunnetaan desimaaliluvuksi
				n=qStrApu3.length();
				if((qStrApu3[n-1]=='h')||(qStrApu3[n-1]=='H'))
				{
					qStrApu3.setLength(qStrApu3.length()-1);
					bool ok;
					n=qStrApu3.toInt( &ok, 16 );
					qStrApu3=QString::number(n,10);
				}
				string bitAddressStr("");
				for(int c=0;c<qStrApu3.length();c++)
					bitAddressStr+=QChar(qStrApu3[c]);
				n=decStringToDec(bitAddressStr);
				//qStrApu2="["+qStrApu3.rightJustify( 3,' ')+"]";
				qStrApu2=decToString(n,3,' ')+"["+hexToString(n, 2)+"H]";
				//Bitin nimi
				qStrApu2+=qStrApu.section(' ',0,0);
				//Jos oli desimaaliluku tai heksaluku
				if(isStringDec(bitAddressStr))
				{
					//Jos bittiosoitettavalla alueella viedään sourceikkunaan
					if(n<=I8031_NameS::MAX_BIT_AREA_BIT_ADDRESS)
					{
						//Muokataan ensin sana muotoon yläosassa osoite alaosassa bitti
						unsigned int addr=(n/8)+I8031_NameS::MIN_BIT_AREA_ADDRESS;
						unsigned int lsb=n%8;
						addr<<=8;
						addr|=lsb;
						namedBitsListBoxQWidget_->addNamedBit(addr,qStrApu2,false);
					}
				}

				//Jos oli portin bitti: Viedään uuserin määrittämään nimilistaan source ikkunaan
				if((qStrApu3[0]=='P')&&((qStrApu3[1]>='0')&&(qStrApu3[1]<='9')))
				{
					qStrApu2="["+qStrApu3+"]";
					qStrApu2+=qStrApu.section(' ',0,0,QString::SectionSkipEmpty);
					//Märitellään osoite: Yläosa porttinumero alaosa bittinumero
					apuStr="P";
					apuStr+=QChar(qStrApu3[1]);
					n=processor->readSFRAddress(apuStr);//Haetaan portin osoite nimellä
					if(n>=0)
					{   //Jos positiivinen luku portti löytyi
						n<<=8;//osoite yläosaan
						n|=ascToHex(QChar(qStrApu3[3])); //bitti alaosaan
						namedPortPinListBoxQWidget_->addNamedBit(n,qStrApu2,true);
					}
				}
			}

			//Katsotaan löytyykö db, DB, dw, DW, dd tai DD
			if(((qStrApu.find(" db ", 0, false))>=0)||((qStrApu.find(" dw ", 0, false))>=0)||((qStrApu.find(" dd ", 0, false))>=0))
			{
				unsigned int pcV=processor->readProgramCounterValue();
				processor->step(true,false);
				processor->setProgramCounterValue(++pcV);
				apuStr=processor->readCodeBufferStr();
				apuStr.resize(7);
				insertJono+=apuStr;
				//Katsotaan montako lisätavua on haettava
				int numBytes;
				if((qStrApu.find(" db ", 0, false))>=0)
				{
					//db:ssä mahdollisuus merkkijonojan kirjoitteluun
					if(qStrApu.contains('"',false))
					{
						numBytes=(qStrApu.findRev('"',-1,false)-qStrApu.find('"',0,false))-1;
						//-1 koska QString merkkijonossa " = /"
					}
					else
					{
						//jos mutta pilkulla erottelu ei toimi kääntäjässä oikein sourcessa ei saa olla sitä
						numBytes=qStrApu.contains(',',false)+1;
					}
				}
				//else poistaa sen mahdollisuuden että "" merkkien sisällä esiintyy dw tai dd
				else
				{
					//Sana
					if((qStrApu.find(" dw ", 0, false))>=0)
					{
						//Tavujen määrä on pilkulla erotettujen lukujen määrä kerrottuna kahella
						numBytes=qStrApu.contains(',',false)+1;
						numBytes*=2;
					}
					//Tuplasana
					if((qStrApu.find(" dd ", 0, false))>=0)
					{
						numBytes=qStrApu.contains(',',false)+1;
						numBytes*=4;
					}
				}
				numBytes--;   //Vähennetään jo haettu tavu
				for(int z=0;z<numBytes;z++)
				{
					processor->step(true,false);
					apuStr=processor->readCodeBufferStr();
					insertJono+=apuStr[5];
					insertJono+=apuStr[6];
					processor->setProgramCounterValue(++pcV);
				}
			}
			else
			{
				//löytyykö riviltä käskyä
				//Labeli pois, jos on, häiritsemästä käskyn etsintää.
				if(qStrApu.contains(':',false))
					//Välilyönti eteen jos on kirjoteltu putkeen labelin jälkeen
					qStrApu2=" "+qStrApu.section(':',1);
				else
					qStrApu2=qStrApu;
				qStrApu=qStrApu2.stripWhiteSpace();
				qStrApu+=" ";
				qStrApu2=qStrApu.section(' ',0,0); //Otetaan vain eka sana tutkailuun.
				if(processor->isInstruction(qStrApu2.lower()))
				{
					processor->step(true,false);
					apuStr=processor->readCodeBufferStr();
					apuStr.resize(12);
					insertJono+=apuStr;
				}
				else
					insertJono+="            ";
			}
			insertJono+=rivi;

			//Korvataan tabulaattorit kolmella välilyönnillä.
			qStrApu2=insertJono;
			insertJono=QChar(qStrApu2[0]);
			for(n=1;n<qStrApu2.length();n++)
			{
				if(qStrApu2[n]!='\r')
				{
					if(qStrApu2[n]=='\t')
						insertJono+="   ";
					else
						insertJono+=QChar(qStrApu2[n]);
				}
			}

			//Viedään sourceikkunaan
			asmSourceTabQTabWidget_->insertSourceLine(tabPageNumber, insertJono);

			//Katsotaan onko include sourcefileä
			qStrApu=rivi;
			if(qStrApu.contains("$include",false))
			{
				//Korjataan hieman tiedostonimeä
				if(qStrApu.contains(".a51",false))
				{
					qStrApu.setLength((qStrApu.find(".a51",0,false))+4);
				}
				else
				{
					if(qStrApu.contains(".h51",false))
					{
						qStrApu.setLength((qStrApu.find(".h51",0,false))+4);
					}
					else
					{
						if(qStrApu.contains(".h",false))
						{
							qStrApu.setLength((qStrApu.find(".h",0,false))+2);
						}
					}
				}
				QString includeFilename;
				if((qStrApu.contains('/'))&&!(qStrApu.contains("./")))
				{
					includeFilename=qStrApu.mid(qStrApu.find("$include",0,false)+9, qStrApu.length());
				}
				else
				{
					includeFilename=activeHexFilename;
					includeFilename.setLength(activeHexFilename.findRev('/')+1);
					includeFilename+=qStrApu.mid(qStrApu.find("$include",0,false)+9, qStrApu.length());
				}
				if((includeFilename.contains(".a51",false))||(includeFilename.contains(".h51",false))||(includeFilename.contains(".H",false))||(includeFilename.contains(".h",false)))
				{
					if(!sourceFileParcer(includeFilename, activeHexFilename, processor, lineNumber)) //Rekur
					{	//jos yksikin source file puuttuu ei ladata enempää
						sourceFile.close();
						return false;
					}
				}
			}
			//Katsotaan onko modia
			if(qStrApu.contains("$mod",false))
			{
				//Ei tarttee loadata
			}
		}
		sourceFile.close();
		//insertItem(decToString((*lineNumber)++,6,' ')+"Close: "+sourceFilename);
		return true;
	}
	return false;
}


void AssemblerSourceW::updateAsmSource(unsigned long const address, unsigned long const lastStepAddress)
{
	asmSourceTabQTabWidget_->setProgramCounterPointer(address);
	lastFocusLineAddress_=lastStepAddress;
}

void AssemblerSourceW::focusLine(unsigned long const address)
{
	asmSourceTabQTabWidget_->focusLine(address);
}

bool AssemblerSourceW::isAtBreakpoint(unsigned long const address)
{
	return asmSourceTabQTabWidget_->isBreakpoint(address);
}

bool AssemblerSourceW::toggleBitChangeBreakpoint(unsigned int const bitAddress, bool const currentValue)
{
	return bitBreakpoint_->toggleBreakpoint(bitAddress, currentValue);
}

unsigned int AssemblerSourceW::isAtBitChangeBreakpoint(I8031* const processor)
{
	return bitBreakpoint_->isBreakpointsAtBitChange(processor);
}

bool AssemblerSourceW::showBitChangeBreakpoint(void)
{
	unsigned int bitAddress=bitBreakpoint_->lastBreakpoint();
	if(bitAddress!=BitChangeBreakpoint::NO_BITCHANGES)
	{
		bitAddress&=0xFF0F; //Korjataan breakpointin arvo pois;
		namedBitsListBoxQWidget_->showBitChangeBreakpoint(bitAddress);
		namedPortPinListBoxQWidget_->showBitChangeBreakpoint(bitAddress);
		return true;
	}

	return false;
}

void AssemblerSourceW::clearExecutionPoint(void)
{
	lastPointedLineAddressInSourceTabWidget_=-1;
	asmSourceTabQTabWidget_->clearExecutionPoint();
}

void AssemblerSourceW::asmSourceTabQTabWidgetRightButtonClicked(QListBoxItem* item, const QPoint& p)
{
	asmSourceTabQTabWidgetRightButtonClickedItem_=item;
	asmSourceTabQTabWidgetRightButtonClickedPos_=p;

	pointedLineAddressInActiveSourceTabWidget_=asmSourceTabQTabWidget_->addressInCurrentPageSourceListActiveLine();
	pointedLineInActiveSourceTabWidget_=asmSourceTabQTabWidget_->currentPageSourceListActiveLineNumber();
	asmSourceKListBoxMenu_->popup(p);
}

void AssemblerSourceW::addPopupMenu(void)
{
	asmSourceKListBoxMenu_=new KPopupMenu(this);
	asmSourceKListBoxMenu_->insertItem(breakPointIcon32KPixmap_, i18n("&Toggle Breakpoint"), this,SLOT(toggleBreakpointActivated()));
	asmSourceKListBoxMenu_->insertItem( i18n("&Remove Breakpoints"), this,SLOT(removeBreakpointsActivated()));
	asmSourceKListBoxMenu_->insertItem(executionPointIcon32KPixmap_, i18n("&Execution Point"), this,SLOT(executionPointActivated()));
	asmSourceKListBoxMenu_->insertSeparator();
	asmSourceKListBoxMenu_->insertItem(findIcon32KPixmap_, i18n("&Find"), this,SLOT(findActivated()),CTRL+Key_F);
	menuFindNext_ID_=asmSourceKListBoxMenu_->insertItem(findIcon32KPixmap_, i18n("Find &Next"), this,SLOT(findNext()),Key_F3);
	asmSourceKListBoxMenu_->setItemEnabled(menuFindNext_ID_, false);

}

void AssemblerSourceW::toggleBreakpointActivated(void)
{
	if(pointedLineAddressInActiveSourceTabWidget_!=AsmSourceQWidget::NO_ADDRESS)
	{
		asmSourceTabQTabWidget_->toggleBreakpoint(pointedLineAddressInActiveSourceTabWidget_);
	}
}

void AssemblerSourceW::removeBreakpointsActivated(void)
{
	asmSourceTabQTabWidget_->removeBreakpoints();
}

void AssemblerSourceW::executionPointActivated(void)
{
	if(pointedLineAddressInActiveSourceTabWidget_!=AsmSourceQWidget::NO_ADDRESS)
	{
		if(lastPointedLineAddressInSourceTabWidget_!=pointedLineAddressInActiveSourceTabWidget_)
		{
			asmSourceTabQTabWidget_->setExecutionPoint(pointedLineAddressInActiveSourceTabWidget_);
			lastPointedLineAddressInSourceTabWidget_=pointedLineAddressInActiveSourceTabWidget_;
			emit message_SetExecutionPoint(this->name(), pointedLineAddressInActiveSourceTabWidget_);
		}
		else
		{
			lastPointedLineAddressInSourceTabWidget_=-1;
			asmSourceTabQTabWidget_->clearExecutionPoint();
			emit message_SetExecutionPoint(this->name(), lastFocusLineAddress_);
		}
	}
}

void AssemblerSourceW::findActivated(void)
{
	KFindDialog findDlg(asmSourceTabQTabWidget_, "findDialog", KFindDialog::FromCursor, findHistoryQStringList_, false);
	QString textInRow = asmSourceTabQTabWidgetRightButtonClickedItem_->text();

	QPoint cPos=parentWidget()->pos();
	int cWidth=asmSourceTabQTabWidget_->width();
	int mousePx=asmSourceTabQTabWidgetRightButtonClickedPos_.x();
	int mousePosXInRow=(mousePx-cPos.x()-3); //3=reunapikselit.
	int charPosInRow = mousePosXInRow/8-1; //8=fonttikoko pikseleinä, 1=merkki rivillä takaisin päin.
	QString textUnderCursor = QString("");
	if((charPosInRow>=0) && (mousePosXInRow < cWidth) )
	{
		int n=charPosInRow;
		auto isCategory = [](QChar c) -> bool
		{
			return (c.category()==QChar::Category::Letter_Uppercase)||
				(c.category()==QChar::Category::Letter_Lowercase)||
				(c.category()==QChar::Category::Number_DecimalDigit)||
				(c =='_');
		};
		for( ; n>=0; --n )
			if(!isCategory(textInRow[n]))
				break;
		++n;
		int maxMarks=std::min(cWidth/8 - 1,int(textInRow.length()));
		for( ; n<maxMarks; ++n )
			if(isCategory(textInRow[n]))
				textUnderCursor += textInRow[n];
			else
				break;
	}

	findDlg.setPattern(textUnderCursor);

	findDlg.setSupportsBackwardsFind(false);
	findDlg.setSupportsRegularExpressionFind(false);
	if ( findDlg.exec() != QDialog::Accepted )
		return;

	findOptions_=findDlg.options();
	textToFind_=findDlg.pattern();

	if(findOptions_&KFindDialog::FromCursor)
		lineToFindText_=pointedLineInActiveSourceTabWidget_+1;
	else
		lineToFindText_=0;
	if(!(findHistoryQStringList_.contains(textToFind_)))
		findHistoryQStringList_.append(textToFind_);
	firstLineToFindText_=lineToFindText_;
	startBeginning_=false;
	asmSourceKListBoxMenu_->setItemEnabled(menuFindNext_ID_, true);
	findNext();
}

void AssemblerSourceW::findNext()
{
	KListBox* pToASLB=asmSourceTabQTabWidget_->pointerToCurrentAsmSourceQWidgetKListBox();
	if((textToFind_!="")&&pToASLB)
	{
		bool stop=false;
		do
		{
			do
			{
				QString txtLine=pToASLB->text(lineToFindText_++);
				if((lineToFindText_>=firstLineToFindText_)&&startBeginning_)
				{
					startBeginning_=false;
					if((KMessageBox::messageBox(pToASLB,KMessageBox::QuestionYesNo, i18n("End of search: "+textToFind_+"\nContinue?"),i18n("Find"), KStdGuiItem::cont(), KStdGuiItem::cancel(), "",KMessageBox::Notify))==KMessageBox::No)
					{
						return;
					}
				}
				bool find=false;
				if(findOptions_&KFindDialog::WholeWordsOnly)
				{
					if((findOptions_&KFindDialog::CaseSensitive)&&(txtLine.find(" "+textToFind_+" ",0,true)>=0))
						find=true;
					if((!(findOptions_&KFindDialog::CaseSensitive))&&(txtLine.find(" "+textToFind_+" ",0,false)>=0))
						find=true;
				}
				else
				{
					if((findOptions_&KFindDialog::CaseSensitive)&&(txtLine.find(textToFind_,0,true)>=0))
						find=true;
					if((!(findOptions_&KFindDialog::CaseSensitive))&&(txtLine.find(textToFind_,0,false)>=0))
						find=true;
				}
				if(find)
				{
					pToASLB->setCurrentItem(lineToFindText_-1);
					pToASLB->ensureCurrentVisible();
						return;
				}
			}
			while(lineToFindText_<pToASLB->count());
			lineToFindText_=0;
			startBeginning_=true;
			if((KMessageBox::messageBox(pToASLB,KMessageBox::QuestionYesNo, i18n("End of asmSource tab reached. \nContinue beginning of tab?"),i18n("Find"), KStdGuiItem::cont(), KStdGuiItem::cancel(), "",KMessageBox::Notify))==KMessageBox::No)
				stop=true;
		}
		while(!stop);
	}
}

void AssemblerSourceW::currentAsmSourceTabChanged(QWidget* page)
{
	lineToFindText_=0;
	firstLineToFindText_=lineToFindText_;
	startBeginning_=false;
}

void AssemblerSourceW::updateNamedBits(unsigned int const bitByteAddress, unsigned char const  bitByteValue)
{
	namedBitsListBoxQWidget_->updateNamedBits(bitByteAddress, bitByteValue, bitBreakpoint_);
}

void AssemblerSourceW::updateNamedPortBits(unsigned int const portAddress, unsigned char const portValue)
{
	namedPortPinListBoxQWidget_->updateNamedBits(portAddress, portValue, bitBreakpoint_);
}

void AssemblerSourceW::bitsListBoxBitDoubleClicked(const QString& mstr, const int value)
{
	emit message_ToggleBit(this->name(), mstr, value);
}

void AssemblerSourceW::bitListRightMouseButtonClicked(const unsigned int bitAddress)
{
	emit message_ToggleBreakpointBit(this->name(), bitAddress);
}

void AssemblerSourceW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	asmSourceTabQTabWidget_->saveWorkDataToFile(saveFileStream);
}

void AssemblerSourceW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	asmSourceTabQTabWidget_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
}

QSize AssemblerSourceW::getMaximumSize(void)
{
	return QSize(I51BaseW::WINDOW_WIDTH_MAX, I51BaseW::WINDOW_HEIGHT_MAX);
}
