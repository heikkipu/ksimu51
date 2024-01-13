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
#ifndef KSIMU51_H
#define KSIMU51_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "KSimu51_NameS.h"
#include "./share/WorkspaceMainW.h"
#include "./processorsW/ProcessorsWLib.h"
#include "./share/LedIndicatorGrid.h"

#include <string>

#include <kmenubar.h>
#include <kpixmap.h>
#include <kstatusbar.h>
#include <klistbox.h>
#include <kapplication.h>

#include <qobject.h>

class QTimer;
class KPopupMenu;
class KListBox;
class KLineEdit;
class KProgress;


class I51BaseW;


/**
* @short Application Main Window
* @author Heikki Pulkkinen <heikki@CompaQ7113EA>
* @version 0.1
*/
class KSimu51 : public WorkspaceMainW
{
	Q_OBJECT
	public:
		/**
		* Default Constructor
		*/
		KSimu51();

		/**
		* Default Destructor
		*/
		virtual ~KSimu51();

		void loadPrevWork(void); //Voidaan mainista ladata edellinen työ käynnistyksen yhteydessä

	protected:
		#define FONTFAMILY "MonoSpace"

		void closeEvent( QCloseEvent * ) override;
		bool event(QEvent *e) override;
		void resizeEvent(QResizeEvent *e) override;
		void mousePressEvent(QMouseEvent *e) override;

	signals:
//      void messageTile(void);

	private slots:
		void processorCreatedFromLibrary(I51BaseW*);
		//I51BaseW* newProcessor(int const  processorMenuIndex, int const  index);

		void openWork(void);
		void saveWork(void);
		void saveWorkAs(void);

		void loadEverythinToActiveProcessor(void);
		void loadEverythinToAllProcessors(void);
		void copyActiveProcessor(void);
		void activeProcessorProperties(void);
		void powerOnReset(void);
		void powerOnResetAll(void);
		void run(void);
		void runAll(void);
		void stop(void);
		void stopAll(void);
		void step(void);
		void stepAll(void);

		void showPC(void);
		void showSP(void);
		void showR0(void);
		void showR1(void);
		void showDPTR(void);
		void showProsessorUpdate(void);
		void showAllProsessorsUpdate(void);
		void centerActiveWindowInActiveWorkspaceLevel(void);
		void cornerActiveWindowInActiveWorkspaceLevel(void);
		void nextActiveWindowInActiveWorkspaceLevel(void);
		void backPreviousWindowStateInActiveWorkspaceLevel(void);
		void moveNearActiveWindowToWindowInActiveWorkspaceLevel(void);
		void resizeWindowHiddenOffInActiveWorkspaceLevel(void);
		void setActiveWindowColorInActiveWorkspaceLevel(void);

		void runTimerTimeout(void);

		void editSerialNet(void);
		void serialNetEditLineReturnPressed(void);
		void serialNetEditLineTextChanged(const QString& t);
		void serialNetEditListBoxRightButtonClicked( QListBoxItem* item, const QPoint& p);
		void serialEditListBoxInsertLineActivated(void);
		void serialEditListBoxRemoveLineActivated(void);
		void serialEditListBoxRemoveAllLinesActivated(void);
		void serialEditListBoxClearSelectionActivated(void);
		void charToSerialNet(void);
		void clearSerialNetListBox(void);

		void closeWindow(void);
		void closeAllWindows(void);

		void messageFromProcessorW_SetStatusBar(const int processorIndex, const QString& text, const unsigned int time);
		void messageFromProcessorW_Close(const int processorIndex);
		void messageFromProcessorW_SetWindowLevel(const int processorIndex, const int level);
		
		void prosessorActivated(void);

		void x11Signal_LeftMouseButtonPressed(QPoint* p);

//-----------------------------------------------------------------------------------------------------------------------------------------
// 	Save/Read Work.
	public:
		enum SAVEDWORKDATATYPE{
			QUINT8=1, //8 bits
			QUINT16, //16 bits
			QUINT32, //32 bits
			QUINT64, //64 bits
			DOUBLE,
			CHARSTRING,
			QSTRING,
			QPOINT,
			QRECT
		};
	private:
		QString workFilename_;
		QString workPath_;
		QString openWorkFilename_;
		void saveWorkDo(void);
		void openWorkDo(void);

		void saveCFG(void);
		void readCFG(void);

		bool openWorkFirstTimeAfterClear_;
		bool workSavedFirstTime_;
//-----------------------------------------------------------------------------------------------------------------------------------------

		unsigned int serialNetCounter_;
		bool findDPTRWasDone_;

		int activeWindowLevel_;
		//Ikonit
		KPixmap intelIcon_;
		KPixmap newProcessorIcon_;
		KPixmap loadProgramIcon_;
		KPixmap loadProgramToAllIcon_;
		KPixmap openWorkIcon_;
		KPixmap saveWorkIcon_;
		KPixmap saveWorkAsIcon_;
		KPixmap closeIcon_;
		KPixmap copyActiveProcessorIcon_;
		KPixmap activeProcessorPropertiesIcon_;
		KPixmap quitIcon_;
		
		KPixmap powerOnResetIcon_;
		KPixmap powerOnResetAllIcon_;
		KPixmap runIcon_;
		KPixmap runAllIcon_;
		KPixmap stopIcon_;
		KPixmap stopAllIcon_;
		KPixmap stepIcon_;
		KPixmap stepAllIcon_;
		KPixmap stopAllOneBreakpointIcon_;
		
		KPixmap characterToSerialIcon_;
		KPixmap characterToSerialEditIcon_;
		KPixmap clearSerialNetListBoxIcon_;
		KPixmap PCMark_;
		KPixmap SPMark_;
		KPixmap R0Mark_;
		KPixmap R1Mark_;
		KPixmap dptrMark_;
		KPixmap showBreakpointIcon_;
		KPixmap refreshIcon_;
		KPixmap refreshAllIcon_;
		KPixmap centerWindowIcon_;
		KPixmap cornerWindowIcon_;
		KPixmap nextWindowIcon_;
		KPixmap backWindowStateIcon_;
		KPixmap nearWindowToIcon_;
		KPixmap resizeWindowHiddenOffIcon_;

		void loadIcons(void);

		void createMenu(void);
		void enableDisableMenusToolbaars(bool const  close);
		//Enable/Disable tai show/hide menut/toolbaarit
		KPopupMenu* menuFile_;
		int menuFile_SaveWork_ID_;
		int menuFile_SaveWorkAs_ID_;
		int menuFile_Close_ID_;
		KPopupMenu* menuEdit_;
		int menuEdit_ID_;
		KPopupMenu* menuView_;
		int menuView_ID_;
		int menuView_UpdateAll_ID_;
		KPopupMenu* menuProcessor_;
		int menuProcessor_LoadHexFile_ID_;
		int menuProcessor_LoadHexFileToAll_ID_;
		KPopupMenu* menuDebug_;
		int menuDebug_ID_;
		int menuDebug_PowerOnResetAll_ID_;
		int menuDebug_RunAll_ID_;
		int menuDebug_StopAll_ID_;
		int menuDebug_StepAll_ID_;

		int menuWindows_ID_;

		KPopupMenu* workspaceMenu_;

		void createToolBars(void);
		KToolBar *toolsFile_;
		int toolsFile_LoadProgram_ID_;
		int toolsFile_LoadProgramToAll_ID_;
		int toolsFile_Save_ID_;
		KToolBar *toolsDebug_;
		int toolsDebug_PowerOnResetAll_ID_;
		int toolsDebug_RunAll_ID_;
		int toolsDebug_StopAll_ID_;
		int toolsDebug_StepAll_ID_;
		int toolsDebug_StopAllAtBreakpoint_ID_;
		KToolBar *toolsView_;
		int toolsView_Update_ID_;
		int toolsView_UpdateAll_ID_;
		int toolsView_ShowBreakpoint_ID_;
		KToolBar *serialTools_;

		QLabel* statusBarLabel_;
		KProgress* overallStatusBarProgressBar_;
		int overallProgressBarCounter_;
		int overallProgressBarMultiplier_;
		void countProgressBar(const int value);
		void setProgressBar(const int value);
		void resetProgressBar(void);

		void addSerialPortNet(void);
		void updateSerialNet(void);
		void serialNetEditListBoxAddPopupMenu(void);
		void serialNetEditListBoxOrderItems(void);
		KPopupMenu* serialEditListBoxMenu_;
		KLineEdit *serialNetEditLine_;
		KListBox *serialNetEditListBox_;
		KListBox *serialNetHistoryListBox_;
		LedIndicatorGrid* serialIndicator_;
		int serialTransmitOnLed_INDEX_;
		int serialTransmitErrorLed_INDEX_;

		QString getHexFileNameToOpen(void);
		void loadEverything(bool all, QString hexFilename);

		QTimer *runTimer_;
		int runTimerInterval_;
		double fastestProcessorMachineTime_;   //Nopeimman processorin koneaika

		ProcessorsWLib* processorWindowLib_;

		//Processorin lisäykseen tarvittavat eri valmistajat:
		//-ModifyToCreateProsessor- Add New Menu if new Manufacturer
		KPopupMenu* menuProcessor_New_Maxim_;
		KPopupMenu* menuProcessor_New_Philips_;
		KPopupMenu* menuProcessor_New_Atmel_;
		KPopupMenu* menuProcessor_New_Chipcon_;

		#define CONFIGFILENAME "ksimu51.cfg"

};

#endif // _KSIMU51_H_
