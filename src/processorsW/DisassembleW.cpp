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
#include "DisassembleW.h"
#include "I51BaseW.h"

#include <kpopupmenu.h>
#include <klocale.h>
#include "../ksimu51.h"



DisassembleW::DisassembleW( QWidget* parent, char const * name, int wflags )
	: ShareMainW( parent, name, wflags )
{
	connect(this, SIGNAL(message_SetExecutionPoint(const QString& , const unsigned long )), this->parent()->parent()->parent() ,SLOT(messageFromWorkspaceW_SetExecutionPoint(const QString& , const unsigned long )));

	setCentralWidget( new QWidget( this, "qt_central_widget" ) );
	disassembleFormLayout_=new QGridLayout( centralWidget(), 1, 1, 1, 1, "dissassembleFormLayout");

	//Disassemble list BOX
	disassembleKListBox_ = new KListBox(centralWidget(), "disassembleKListBox_" );
	setFocusProxy(disassembleKListBox_);
	QFont disassembleKListBox_font(  disassembleKListBox_->font() );
	disassembleKListBox_font.setFamily( FONTFAMILY );
	disassembleKListBox_font.setPointSize(10);
	disassembleKListBox_->setFont( disassembleKListBox_font );
	disassembleKListBox_->setFrameShape( KActiveLabel::StyledPanel );
	disassembleKListBox_->setFrameShadow( KActiveLabel::Sunken );
	disassembleKListBox_->clear();
	disassembleFormLayout_->addWidget( disassembleKListBox_, 0, 0 );

	//Disassemble Last instruction label
	lastInstructionKActiveLabel_ = new KActiveLabel(centralWidget(), "lastInstructionKActiveLabel_" );
	lastInstructionKActiveLabel_->setMaximumSize(QSize(32767,25));
	lastInstructionKActiveLabel_->setGeometry( QRect( 0,parent->height()-50,parent->width() , 25 ) );
	lastInstructionKActiveLabel_->setPaletteBackgroundColor(QColor(255, 255, 255));
	QFont lastInstructionKActiveLabel_font(lastInstructionKActiveLabel_->font());
	lastInstructionKActiveLabel_font.setFamily(FONTFAMILY);
	lastInstructionKActiveLabel_->setFont(lastInstructionKActiveLabel_font);
	lastInstructionKActiveLabel_->setMouseTracking( FALSE );
	lastInstructionKActiveLabel_->setFrameShape( KActiveLabel::StyledPanel );
	lastInstructionKActiveLabel_->setFrameShadow( KActiveLabel::Sunken );
	lastInstructionKActiveLabel_->setVScrollBarMode( KActiveLabel::AlwaysOff );
	lastInstructionKActiveLabel_->setTextFormat( KActiveLabel::AutoText );
	lastInstructionKActiveLabel_->setText( "" );
	disassembleFormLayout_->addWidget(lastInstructionKActiveLabel_, 1, 0 );

	breakPointIcon32_.load("pics/KS51hi32StopProcessor.png", 0, KPixmap::Auto );
	breakPointIcon16_.load("pics/KS51hi16StopProcessor.png", 0, KPixmap::Auto );
	executionPointIcon32_.load("pics/KS51hi32RunProcessor.png", 0, KPixmap::Auto );
	executionPointIcon16_.load("pics/KS51hi16RunProcessor.png", 0, KPixmap::Auto );
	procramCounterPointerIcon16_.load( "pics/KS51hi16WhiteLedOn.png", 0, KPixmap::Auto );
	addPopupMenu();

	disassembleExecutionPoint_=0;
	lastUpdateExecutionAddress_=0;
	prevProgramCounterPointerAddress_=0;
	lastPointedItemInDisassembleKListBox_=-1;
	executionPointLine_=-1;
	connect(disassembleKListBox_, SIGNAL(rightButtonClicked( QListBoxItem*, const QPoint&)), this, SLOT(disassembleKListBoxRightButtonClicked(QListBoxItem*, const QPoint &  )));
	removeBreakpointsActivated();
}

DisassembleW::~DisassembleW()
{
}

void DisassembleW::clear(void)
{
	disassembleKListBox_->clear();
	for(unsigned long n=0;n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE;n++)
	{
		disassembleBreakpointTable_[n]=false;
		disassembleKListBoxLineInAddress_[n]=0;
	}
	addressInDisassembleKListBoxLineQVList_.clear();
	lastUpdateExecutionAddress_=0;
	lastPointedItemInDisassembleKListBox_=-1;
	executionPointLine_=-1;
}

void DisassembleW::insertItem(const QString& sitem, unsigned long processorAddress)
{
	disassembleKListBox_->insertItem(sitem);
	addressInDisassembleKListBoxLineQVList_.append(processorAddress);
	disassembleKListBoxLineInAddress_[processorAddress]=disassembleKListBox_->count()-1;
	disassembleBreakpointTable_[processorAddress]=false;
}

void DisassembleW::update(unsigned long const instructionAddress, unsigned long const lastInstructionAddress, unsigned long const lastStepAddress)
{
	//Edellinen käsky labeliin näkyviin
	disassembleKListBox_->setCurrentItem(disassembleKListBoxLineInAddress_[lastInstructionAddress]);
	QString labelText=disassembleKListBox_->currentText();
	updateActiveLable(labelText);
	
	setProgramCounterPointer(instructionAddress);

	lastUpdateExecutionAddress_=lastStepAddress;
}

void DisassembleW::focusLine(unsigned long const address)
{
	disassembleKListBox_->setCurrentItem(disassembleKListBoxLineInAddress_[address]);
	disassembleKListBox_->ensureCurrentVisible();
}

bool DisassembleW::isAtBreakpoint(unsigned long pAddress)
{
	return disassembleBreakpointTable_[pAddress];
}

void DisassembleW::clearExecutionPoint(void)
{
	lastPointedItemInDisassembleKListBox_=-1;
	clearExecutionPointPrivate();
}

void DisassembleW::clearExecutionPointPrivate(void)
{
	QString lineText=disassembleKListBox_->text(disassembleExecutionPoint_);
	executionPointLine_=-1;
	if(disassembleBreakpointTable_[addressInDisassembleKListBoxLineQVList_[disassembleExecutionPoint_]])
		disassembleKListBox_->changeItem(breakPointIcon16_, lineText, disassembleExecutionPoint_);
	else
		disassembleKListBox_->changeItem(lineText, disassembleExecutionPoint_);
}

void DisassembleW::setProgramCounterPointer(unsigned long const address)
{
	//Poistetaan edellinen programcounterpointteri-ikoni
	int lineIndex=disassembleKListBoxLineInAddress_[prevProgramCounterPointerAddress_];
	QString lineText=disassembleKListBox_->text(lineIndex);
	disassembleKListBox_->changeItem(lineText, lineIndex);
	if(disassembleBreakpointTable_[prevProgramCounterPointerAddress_])
		disassembleKListBox_->changeItem(breakPointIcon16_, lineText, lineIndex);
	if(executionPointLine_==lineIndex)
		disassembleKListBox_->changeItem(executionPointIcon16_, lineText, lineIndex);
		
	//Nyt suoritettava näkyviin
	lineIndex=disassembleKListBoxLineInAddress_[address];
	lineText=disassembleKListBox_->text(lineIndex);
	disassembleKListBox_->changeItem(procramCounterPointerIcon16_, lineText, disassembleKListBoxLineInAddress_[address]);
	prevProgramCounterPointerAddress_=address;
}

void DisassembleW::updateActiveLable(const QString& labelText)
{
	lastInstructionKActiveLabel_->setText(labelText);
}

unsigned int DisassembleW::count(void)
{
	return disassembleKListBox_->count();
}

void DisassembleW::disassembleKListBoxRightButtonClicked(QListBoxItem* item, const QPoint& p)
{
	pointedItemInDisassembleKListBox_=disassembleKListBox_->currentItem();
	disassembleKListBoxMenu_->popup(p);
}

void DisassembleW::addPopupMenu(void)
{
	disassembleKListBoxMenu_=new KPopupMenu(this);
	disassembleKListBoxMenu_->insertItem(breakPointIcon32_, i18n("&Toggle Breakpoint"), this,SLOT(toggleBreakpointActivated()));
	disassembleKListBoxMenu_->insertItem( i18n("&Remove Breakpoints"), this,SLOT(removeBreakpointsActivated()));

	disassembleKListBoxMenu_->insertItem(executionPointIcon32_, i18n("&Execution Point"), this,SLOT(executionPointActivated()));}

void DisassembleW::toggleBreakpointActivated(void)
{
	QString lineText;
	if(disassembleBreakpointTable_[addressInDisassembleKListBoxLineQVList_[pointedItemInDisassembleKListBox_]])
	{
		disassembleBreakpointTable_[addressInDisassembleKListBoxLineQVList_[pointedItemInDisassembleKListBox_]]=false;
		lineText=disassembleKListBox_->text(pointedItemInDisassembleKListBox_);
		disassembleKListBox_->changeItem(lineText, pointedItemInDisassembleKListBox_);
	}
	else
	{
		disassembleBreakpointTable_[addressInDisassembleKListBoxLineQVList_[pointedItemInDisassembleKListBox_]]=true;
		lineText=disassembleKListBox_->text(pointedItemInDisassembleKListBox_);
		disassembleKListBox_->changeItem(breakPointIcon16_, lineText, pointedItemInDisassembleKListBox_);
	}
}

void DisassembleW::removeBreakpointsActivated(void)
{
	for(unsigned int address=0; address<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE;address++)
	{
		if(disassembleBreakpointTable_[address])
		{
			disassembleBreakpointTable_[address]=false;
			for(int n=0; n<disassembleKListBox_->count(); n++)
				if(addressInDisassembleKListBoxLineQVList_[n]==address)
				{
					QString lineText(disassembleKListBox_->text(n));
					disassembleKListBox_->changeItem(lineText,n);
				}
		}
	}
}

void DisassembleW::executionPointActivated(void)
{
	clearExecutionPointPrivate();
	if(lastPointedItemInDisassembleKListBox_!=pointedItemInDisassembleKListBox_)
	{
		QString lineText=disassembleKListBox_->text(pointedItemInDisassembleKListBox_);
		disassembleKListBox_->changeItem(executionPointIcon16_, lineText,pointedItemInDisassembleKListBox_);
		disassembleExecutionPoint_=pointedItemInDisassembleKListBox_;
		lastPointedItemInDisassembleKListBox_=pointedItemInDisassembleKListBox_;
		executionPointLine_=lastPointedItemInDisassembleKListBox_;
		emit message_SetExecutionPoint(this->name(), addressInDisassembleKListBoxLineQVList_[pointedItemInDisassembleKListBox_]);
	}
	else
	{
		lastPointedItemInDisassembleKListBox_=-1;
		emit message_SetExecutionPoint(this->name(), lastUpdateExecutionAddress_);
	}
}


void DisassembleW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//Breakpoint taulukko
	for(unsigned long n=0; n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE; n++)
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)disassembleBreakpointTable_[n];

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

	//Aktiivinen rivi
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)disassembleKListBox_->currentItem();
	//Last instruction Active label
	QString qstr=lastInstructionKActiveLabel_->text();
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QSTRING<<qstr;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void DisassembleW::readWorkDataFromFile(QDataStream& readFileStream,
										std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	//Breakpoint table
	Q_UINT8 value8;
	for(unsigned long n=0; n<I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE; n++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8;
		disassembleBreakpointTable_[n]=(bool)value8;
		//Päivitetään breakpointti näkyviin
		if(value8)
		{
			for(unsigned long i=0; i<disassembleKListBox_->count(); i++)
				if(addressInDisassembleKListBoxLineQVList_[i]==n)
				{
					QString lineText=disassembleKListBox_->text(i);
					disassembleKListBox_->changeItem(breakPointIcon16_, lineText, i);
					break;
				}
		}
	}
	readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END

	Q_UINT32 value32;
	QString qstr;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				//Aktiivinen rivi
				readFileStream>>typeReadData>>value32;
				disassembleKListBox_->setCurrentItem(value32);
				disassembleKListBox_->ensureCurrentVisible();
				break;
			case 1:
				//Last instruction Active label
				readFileStream>>typeReadData>>qstr;
				updateActiveLable(qstr);
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

QSize DisassembleW::getMaximumSize(void)
{
	return QSize(280, I51BaseW::WINDOW_HEIGHT_MAX);
}
