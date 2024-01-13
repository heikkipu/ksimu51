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
#ifndef I51BASEW_H
#define I51BASEW_H

#include "../share/WorkspaceMainW.h"
#include "../KSimu51_NameS.h"
#include <kcursor.h>
#include <qstringlist.h>

#include <string>
#include <functional>

#include "../processors/I8031.h"
#include "DisassembleW.h"
#include "AssemblerSourceW.h"
#include "SfrW.h"
#include "SFRBitW.h"
#include "RegsW.h"
#include "MiscW.h"
#include "InternalRAMW.h"
#include "BitAddressableAreaW.h"
#include "RegisterBankAreaW.h"
#include "ExternalW.h"
#include "RomW.h"
#include "InternalXRAMW.h"


class DisassembleW;
class AssemblerSourceW;
class SfrW;
class SFRBitW;
class RegsW;
class MiscW;
class InternalRAMW;
class InternalXRAMW;
class BitAddressableAreaW;
class RegisterBankAreaW;
class ExternalW;
class RomW;

class I8031;

class I51BaseW :  public WorkspaceMainW
{
	Q_OBJECT
	public:
		I51BaseW(QWidget* parent, char const * name, int wflags, int const index, int const type);
		~I51BaseW();

		void copy(I51BaseW* sourceProcessorW);

		virtual void createExternalAreaWindow(void);

		int readProcessorIndex(void) const {return processorIndex_;};
		int readProcessorType(void) const {return processorType_;};
		double readProcessorMachineCycleTime(void);   //Palauttaa yhden konejakson kuluttaman ajan.

		virtual void loadProgramAndCreateWindows(const QString& hexFilename);//Lataa hex filen ,sourceAsmin(jos on) ja muodostaa kaikki ikkunat

		virtual QString readEAPinPointingProgramMemoryFilename(void) const; //Paluttaa EA pinnin mukaan aktiivisen joko internal tai external

		QString readProcessorName(void);
		QString readProcessorWindowName(void);
		bool isReadyToRun(void){return readyToRun_;};

		void updateProcessorWindows(void);

		void centerActiveWindowInExternalArea(void);
		void cornerActiveWindowInExternalArea(void);
		void nextWindowToShowInExternalArea(void);
		void backPreviousWindowStateInExternalArea(void);
		void activeWindowNearToWindowInExternalArea(void);
		void activeWindowResizeHiddenOffInExternalArea(void);
		void markActiveWindowInExternalArea(void);
		void unmarkWindowsInExternalArea(void);

		void updateDPTR(void);
		virtual void showPointerPC(void);
		virtual void showPointerSP(void);
		virtual void showPointerR0(void);
		virtual void showPointerR1(void);
		virtual void showPointerDPTR(void);

		//Kopioidaan external area components
		QWidgetList readExternalAreaComponents(void); //Luku
		void addExternalAreaComponents(QWidgetList externalAreaComponentList); //Lisäys listasta
		IOPin* readExternalAreaNets(void);
		void addExternalAreaNets(IOPin* nets);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

		//virtual void reset(void);
		virtual void powerOnReset(void);
		void run(void);
		void stop(void);
		virtual bool step(bool const  disassemble, double const  stepTime);

		//Kokonaisaikalaskuirin luku.
		double readWholeStepTimeCounter(void);

		bool readEAPin(void);
		void setEAPin(bool value); //EA pinnillä valitaan mihin ohjelma ladataan (internal/external) (copy)
		bool isRunning(void){return running_;};
		bool isAtBreakpoint(void);
		void showBreakpoint(void);

		bool isSerialPortTransmitOn(void);
		bool readPrevoiusStepSerialTransmitFlag(void);
		int lastTransmittedData(void);
		void writeDataToSerialPort(int data);
		virtual bool readTxDPin(void);
		virtual void writeRxDPin(bool const value);


		bool activateWindowFromGlobalPointValue(QPoint* p);

		void print(QPrinter*);

		void setCaptionIndexText(string& text){captionIndexText_=text;setPlainCaption(captionIndexText_);};

	protected:
		#define FONTFAMILY "MonoSpace"

		virtual void setCaptionAndIconToProgramMemoryWs(KPixmap const &icon);
		string captionIndexText_;
		KPixmap whiteLedIcon_;

		I8031* processor_; //Pointteri prosessoriluokkaan

		ExternalW* externalw_;
		MiscW* miscw_;

		bool firstProgramLoad_;//Ensimmäinen ohjelman lataus
		void loadProgramAndCreateAll(const QString& hexFilename, std::vector<std::tuple<unsigned int, unsigned char>>* programLoaded);

		virtual void copy_VirtualPart(I51BaseW* sourceProcessorW){;};

		virtual void updateProcessorWindows_VirtualPart(void){};
		virtual void updateProgramMemoryWindows(void);

		virtual bool isAtBreakpoint_VirtualPart(void);
		virtual void showBreakpoint_VirtualPart(void);

		void insertItems(void);
		void createDynamicWindows(void);
		void createAsmSourceWindow(AssemblerSourceW*& aliasPToW, QString const programMemoryName, const char* windowName);
		void createDisassembleWindow(DisassembleW*& aliasPToW, QString const programMemoryName, const char* windowName);
		virtual void createDynamicWindows_VirtualPart(void){};
		void createStaticWindows(void);
		virtual void createStaticWindows_VirtualPart(void){};

		virtual void clearExternalAreas(void);
		virtual void clearInternalRamAreas(void);

		bool doStep(bool const  disassemble, double const  stepTime);
		long lastStepProgramCounterValue;

		//SP R0 ja R1 arvo kaikkien perivien käyttöön
		inline int spValue(void) {return processor_->readInternalDataMemory(I8031_NameS::SP);};
		inline unsigned int r0Value(void){return processor_->readInternalDataMemory(I8031_NameS::MIN_REGISTER_BANK_ADDRESS+8*processor_->registerBankInUse());};
		inline unsigned int r1Value(void){return processor_->readInternalDataMemory((I8031_NameS::MIN_REGISTER_BANK_ADDRESS+1)+8*processor_->registerBankInUse());};

		int pointerPC_WCount_; //ShowPointterPC vaihto disassemble>source>rom
		bool internalR0_; //ShowPointterR0 vaihto sisäinenAlue<>ulkoinenAlue
		bool internalR1_; //ShowPointterR1 vaihto sisäinenAlue<>ulkoinenAlue

		QString activeHexFilename_;
		IOPinList::PinIndex pinEA_Index_;
		
		//Messages functions virtual part
		virtual void messageFromWorkspaceW_SetExecutionPoint_VirtualPart(const QString& name, const unsigned long address){};
		virtual void messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value){};
		virtual void messageFromWorkspaceW_ValueChanged_VirtualPart(const QString& name, const unsigned long index, const long value){};
		virtual void messageFromWorkspaceW_ToggleBit_VirtualPart(const QString& name, const QString& messageString, const int messageValue){};
		virtual void messageFromWorkspaceW_ToggleBreakpointBit_VirtualPart(const QString& name, int const  bitAddress, bool const bitValue){};
		virtual void messageFromWorkspaceW_FocusProgramMemoryAddress_VirtualPart(const QString& name, const unsigned long address){};

		//Perivien luokkien talletukset uudelleenmääritettävissä
		virtual void saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)=0;
		virtual void readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
													  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)=0;
		virtual void rewriteProgramMemory(void); //Käyttää externalLoadedProgram_ vector containeria.

		//QT Events
		void closeEvent(QCloseEvent *e) override;
		bool event(QEvent *e) override;
		void resizeEvent(QResizeEvent *e) override;
		void mousePressEvent(QMouseEvent *e) override;

	signals:
		//void messageToMain(const int , const int , const QString&, const int);
		void message_SetStatusBar(const int, const QString&, const unsigned int );
		void message_Close(const int );
		void message_SetWindowLevel(const int, const int );
		
	private slots:
		void childWindowActivated(void);

		//Messages functions
		void messageFromWorkspaceW_SetExecutionPoint(const QString& name, const unsigned long address);
		void messageFromWorkspaceW_MemoryValueChanged(const QString& name, const unsigned long address, const int value);
		void messageFromWorkspaceW_ValueChanged(const QString& name, const unsigned long index, const long value);
		void messageFromWorkspaceW_ToggleBit(const QString& name, const QString& messageString, const int messageValue);
		void messageFromWorkspaceW_ToggleBreakpointBit(const QString& name, const int messageValue);
		void messageFromWorkspaceW_Update(const QString& name);
		void messageFromWorkspaceW_FocusProgramMemoryAddress(const QString& name, const unsigned long address);
		void messageFromWorkspaceW_WindowLevel(const QString& name, const int level);

	private:

		int processorIndex_;
		int processorType_;

		void addProcessor(int const  pType, unsigned char const  pIndex);
		bool readyToRun_;

		QString externalProgramMemoryHexFilename_;
		std::vector<std::tuple<unsigned int, unsigned char>> externalLoadedProgram_;

		QGridLayout* processorWindowLayout_;

		KToolBar *I51BaseWTools_;

		void addPopupMenu(void);
		KPopupMenu* workspaceMenu_;

		DisassembleW* external_disassemblew_;
		AssemblerSourceW* external_assemblersourcew_;
		RomW* external_romw_;
		SfrW* sfrw_;
		RegsW* regsw_;
		SFRBitW* sfrbitw_;
		BitAddressableAreaW* bitaddressableareaw_;
		RegisterBankAreaW* registerbankareaw_;
		InternalRAMW* scratchpadareaw_;

		int miscWClockSpeedIndex;

		void updateProcessorExDataToExternalArea(void);
		void updateExternalAreaToProcessorExData(void);
		void updateProcessorIOToExternalArea(bool show);
		void updateExternalAreaToProcessorIO(void);
		void updateExternalDataIOAreaAndProcessor(bool const  updateToShow);

		void updateSFRWindow(void);
		void updateMiscWindow(void);
		void updateRegisterBankWindow(void);
		void updateBitAddressableAreaWindow(void);
		void updateScratchPadAreaWindow(void);
		
		//Ladataan work fileä
		bool loadingWorkFile_;
		//Ohjelmaa ajetaan
		bool running_;

		//Kokonaisaikalaskuri
		double wholeStepTimeCounter_;
		double wholeStepTimeCounterInSync_; //Kokonaisaikalaskuri muiden processorien kanssa synkronointiin

		//Resettipinnin kanssa visualisointia
		bool resetActiveLastValue_;
		KPixmap resetIcon_;
		KPixmap runIcon_;
		KPixmap intelIcon_;

		bool previousStepSerialTransmitFlag_;

	public:
		//Window yleissälää
		static unsigned int const  WINDOW_HEIGHT_MAX=5000;
		static unsigned int const  WINDOW_WIDTH_MAX=3000;
		static unsigned int const  WIDTH_TO_ADD_WINDOW_GEOMETRY=24;
		static unsigned int const  HEIGHT_TO_ADD_WINDOW_GEOMETRY=25;

		//Ikkunoitten nimet
		#define WINDOWNAME_REGS "regsW"
		#define WINDOWNAME_MISC "miscW"
		#define WINDOWNAME_SFR_AREA "sfrW"
		#define WINDOWNAME_SFR_BIT "sfrBW"
		#define WINDOWNAME_BITADDRESSABLE_AREA "bitAddressableAreaW"
		#define WINDOWNAME_REGISTERBANK_AREA "registerBankAreaW"
		#define WINDOWNAME_SCRATCHPAD_AREA "scratchPadAreaW"
		#define WINDOWNAME_EXTERNAL_AREA "externalW"
		#define WINDOWNAME_EXTERNAL_DISASSEMBLE "externalDisassembleW"
		#define WINDOWNAME_EXTERNAL_ASM_SOURCE "externalASMSourceW"
		#define WINDOWNAME_EXTERNAL_ROM "externalROMW"


};

#endif
