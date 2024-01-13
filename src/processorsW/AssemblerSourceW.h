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
#ifndef ASSEMBLERSOURCEW_H
#define ASSEMBLERSOURCEW_H

#include "../processors/I8031.h"
#include "IOPortPin.h"   //Käytetään ioPortPinniluokkaa nimettyjen bittien luonnissa
#include "BitChangeBreakpoint.h"

#include "../share/ShareMainW.h"
#include <ktoolbar.h>
#include <klistbox.h>
#include <kpixmap.h>
#include <kactivelabel.h>
#include <kfinddialog.h>
#include <kmessagebox.h>
#include <string>


#include <qdatastream.h>
#include <qlayout.h>
#include <qvaluelist.h>
#include <qtabwidget.h>
#include <qptrvector.h>

class QGridLayout;
class KListBox;
class KPopupMenu;
class KPixmap;
class KToolBar;
class KMessageBox;
class QTabWidget;

//Widget class to assembler Tab class.
class AsmSourceQWidget : public QWidget
{
	Q_OBJECT
	public:
		static unsigned long const NO_ADDRESS=I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE+1;
		enum lineIcon
		{
			BREAKPOINT_ICON,
			EXECUTIONPOINT_ICON,
			PROGRAMCOUNTERPOINTER_ICON
		};

		AsmSourceQWidget(QWidget * parent, const char * name, WFlags f);
		~AsmSourceQWidget();

		unsigned long insertItem(string const& sitem);
		void focusLine(unsigned long const address);
		unsigned long addressInCurrentActiveLine(void);
		int currentLineNumber(void);

		void setIcon(lineIcon const iconToSet, bool const onOff, unsigned long const address);

		KListBox* pointerToAsmSourceKListBox(void) const {return asmSourceKListBox_;};

	private:
		KListBox* asmSourceKListBox_;
		QValueList<unsigned long>addressInSourceKListBoxLineQValueList_;
		unsigned int asmSourceKListBoxLineInAddress_[I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE];

		KPixmap breakpointIcon_;
		KPixmap executionpointIcon_;
		KPixmap programCounterIcon_;
		int programCounterLineNum_;
};

//Assembler Tab class.
class SourceTabQTabWidget : public QTabWidget
{
	Q_OBJECT
	public:
		SourceTabQTabWidget(QWidget * parent, const char * name, WFlags f);
		~SourceTabQTabWidget();

		void clear(void);
		int addNewSourceTab(const QString& label);
		void insertSourceLine(int const tabNum, string const& sitem);
		void focusLine(unsigned long const address);

		long addressInCurrentPageSourceListActiveLine(void);
		int currentPageSourceListActiveLineNumber(void);
		void toggleBreakpoint(unsigned long const address);
		void removeBreakpoints(void);
		bool isBreakpoint(unsigned long const address);
		void clearExecutionPoint(void);
		void setExecutionPoint(unsigned long const address);
		void setProgramCounterPointer(unsigned long const address);

		KListBox* pointerToCurrentAsmSourceQWidgetKListBox(void) const;
		KListBox* pointerToAsmSourceQWidgetKListBox(int const tabNum);

		void saveWorkDataToFile(QDataStream& saveFileStream);
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);

	private:
		AsmSourceQWidget* asmSourceQWidget_;
		unsigned long asmSourceKTabInAddress_[I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE];//What Tab address is.
		QPtrVector<AsmSourceQWidget>vectorToAsmSourceQWidget_;

		bool asmSourceBreakpointTable_[I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE]; //prosessorin osoiteavaruus
		unsigned long asmSourceExecutionPointAddress_;
		unsigned long programCounterPointerPrevAddress_;
};

//Named BitListWidget
class NamedBitListWidget : public QWidget
{
	Q_OBJECT
	public:
		NamedBitListWidget(QWidget* parent, char const * name, int wflags);
		~NamedBitListWidget();

		void clear(void);
		void addNamedBit(unsigned int const bitAddress, string const bitName, bool const  bitValue);
		void updateNamedBits(unsigned int const byteAddress, unsigned char const byteValue, BitChangeBreakpoint* const breakpointBitList);
		bool showBitChangeBreakpoint(unsigned int const bitAddress);

	private slots:
		void namedBitsKListBoxDoubleClicked (QListBoxItem* item, const QPoint& pos);
		void namedBitsKListBoxRightButtonClicked(QListBoxItem* item, const QPoint& pos);

	signals:
		void bitDoubleClicked(const QString&, const int);
		void bitRightMouseButtonClicked(const unsigned int);

	private:
		bool firstNamedBitToAdd_;
		IOPortPin* namedBitList_;
		IOPortPin* namedBitListLastMember_;
		KListBox* namedBitsKListBox_;

		void changeLedAtListBox(KPixmap const ledIcon, int const index);
		bool readNamedBitListValue(int const index);
		unsigned int readNamedBitListAddress(int const index);

		KPixmap ledOnIconKPixmap_;
		KPixmap ledOffIconKPixmap_;
		KPixmap breakpointLedOnKPixmap_;
		KPixmap breakpointLedOffKPixmap_;
};

//Main class
class AssemblerSourceW: public ShareMainW
{
	Q_OBJECT
	public:
		AssemblerSourceW(QWidget* parent, char const * name, int wflags);
		~AssemblerSourceW();

		void clear(void);
		bool addSourceFile(QString& sourceFilename, QString& activeHexFilename, I8031* processor);

		void updateAsmSource(unsigned long const address, unsigned long const lastStepAddress);
		void updateNamedBits(unsigned int const bitByteAddress, unsigned char const  bitByteValue);
		void updateNamedPortBits(unsigned int const  portAddress, unsigned char const  portValue);

		void focusLine(unsigned long const address);
		
		bool isAtBreakpoint(unsigned long const address);
		void clearExecutionPoint(void);

		bool toggleBitChangeBreakpoint(unsigned int const bitAddress, bool const currentValue);
		unsigned int isAtBitChangeBreakpoint(I8031* const processor);
		bool showBitChangeBreakpoint(void);


		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

		QSize getMaximumSize(void) override;

	private slots:

		void asmSourceTabQTabWidgetRightButtonClicked(QListBoxItem* item, const QPoint& p);
		void toggleBreakpointActivated(void);
		void removeBreakpointsActivated(void);
		void executionPointActivated(void);
		void findActivated(void);
		void findNext(void);

		void currentAsmSourceTabChanged(QWidget* page);
		void bitsListBoxBitDoubleClicked(const QString& mstr, const int value);
		void bitListRightMouseButtonClicked(const unsigned int bitAddress);

	signals:
		void message_SetExecutionPoint(const QString& , const unsigned long );
		void message_ToggleBit(const QString&, const QString&, const int );
		void message_ToggleBreakpointBit(const QString&, const int );

	private:
		QGridLayout* asmSourceFormQGridLayout_;

		bool sourceFileParcer(QString& sourceFilename, QString& activeHexFilename, I8031* processor, unsigned int *lineNumber);//REKUR
		SourceTabQTabWidget* asmSourceTabQTabWidget_;

		void addPopupMenu(void);
		KPopupMenu* asmSourceKListBoxMenu_;

		//Source koodissa olevien prosessorin bittialueen nimettyjen bittien työkalu
		NamedBitListWidget* namedBitsListBoxQWidget_;
		//Source koodissa olevien prosessorin porttien käyttäjän nimeämät nimet
		NamedBitListWidget* namedPortPinListBoxQWidget_;
		//Edellisten bittibreakpointti
		BitChangeBreakpoint* bitBreakpoint_;

		//Right mouse click menu things at asm Tab.
		long pointedLineAddressInActiveSourceTabWidget_;
		long lastPointedLineAddressInSourceTabWidget_;
		unsigned int lastFocusLineAddress_;
		int pointedLineInActiveSourceTabWidget_;

		KPixmap breakPointIcon32KPixmap_;
		KPixmap executionPointIcon32KPixmap_;

		//Tekstin etsintä asmSourcesta;
		QString textToFind_;
		long findOptions_;
		unsigned int lineToFindText_;
		unsigned int firstLineToFindText_;
		bool startBeginning_;
		QStringList findHistoryQStringList_;
		int menuFindNext_ID_;
		KPixmap findIcon32KPixmap_;

		QListBoxItem* asmSourceTabQTabWidgetRightButtonClickedItem_{nullptr};
		QPoint asmSourceTabQTabWidgetRightButtonClickedPos_{0,0};
};


#endif
