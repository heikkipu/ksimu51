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
#include "ksimu51.h"
#include "processorsW/I51BaseW.h"

#include <kmainwindow.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <klineedit.h>
#include <kled.h>
#include <kprogress.h>

#include <kstdaction.h>

#include <qdir.h>
#include <qtimer.h>
#include <qstring.h>

#include <fstream>

KSimu51::KSimu51() : WorkspaceMainW(0, "KSimu51", 0)
{
	//Ladataan ikonit:
	loadIcons();
	//Luodaan MENU
	createMenu();
	//ToolBar
	createToolBars();

	//statusBar()->setSizeGripEnabled(true);
	statusBarLabel_ = new QLabel( statusBar() );
	statusBar()->addWidget(statusBarLabel_, 9, TRUE );
	//Statusbaarin kokonais progresbaari
	overallStatusBarProgressBar_= new KProgress(statusBar());
	overallStatusBarProgressBar_->setCenterIndicator(true);
	statusBar()->addWidget(overallStatusBarProgressBar_, 1, true);
	overallStatusBarProgressBar_->setEnabled(false);

	statusBarLabel_->setText( i18n("Open work or add new processor"));

	runTimer_ = new QTimer(this);
	connect(runTimer_, SIGNAL(timeout()), this, SLOT(runTimerTimeout()));
	runTimerInterval_=0;       //Oletusintervalli, 1=1ms. 0=tehdään niin nopeasti kuin systeemi pystyy.

	fastestProcessorMachineTime_=1.0;   //Nopeimman processorin koneaika alkuarvo

	serialNetCounter_=0;
	workSavedFirstTime_=false;
	openWorkFirstTimeAfterClear_=true;
	workFilename_=QString::null;
	workPath_=QString::null;

	activeWindowLevel_=-1;
	connect(this, SIGNAL(workspaceWindowActivated()), this, SLOT(prosessorActivated()));

	horizontalTileDivider_=2;

	connect(qApp, SIGNAL(x11LeftMouseButtonPressed(QPoint*)), this, SLOT(x11Signal_LeftMouseButtonPressed(QPoint*)));

	findDPTRWasDone_=false;

}

KSimu51::~KSimu51()
{
}


void KSimu51::loadIcons(void)
{

	intelIcon_.load("pics/KS51hi32Intel8051.png", 0, KPixmap::Auto );
	newProcessorIcon_.load("pics/KS51hi32NewProcessor.png", 0, KPixmap::Auto );
	loadProgramIcon_.load("pics/KS51hi32LoadProgram.png", 0, KPixmap::Auto );
	loadProgramToAllIcon_.load("pics/KS51hi32LoadProgramToAll.png", 0, KPixmap::Auto );
	openWorkIcon_.load("pics/KS51hi32OpenWork.png", 0, KPixmap::Auto );
	saveWorkIcon_.load("pics/KS51hi32SaveWork.png", 0, KPixmap::Auto );
	saveWorkAsIcon_.load("pics/KS51hi32SaveWorkAs.png", 0, KPixmap::Auto );
	closeIcon_.load("pics/KS51hi32Close.png", 0, KPixmap::Auto );
	quitIcon_.load("pics/KS51hi32Quit.png", 0, KPixmap::Auto );

	copyActiveProcessorIcon_.load("pics/KS51hi32CopyProcessor.png", 0, KPixmap::Auto );
	activeProcessorPropertiesIcon_.load("pics/KS51hi32ProcessorProperties.png", 0, KPixmap::Auto );

	powerOnResetIcon_.load("pics/KS51hi32ResetProcessor.png", 0, KPixmap::Auto );
	powerOnResetAllIcon_.load("pics/KS51hi32ResetAllProcessors.png", 0, KPixmap::Auto );
	runIcon_.load("pics/KS51hi32RunProcessor.png", 0, KPixmap::Auto );
	runAllIcon_.load("pics/KS51hi32RunAllProcessors.png", 0, KPixmap::Auto );
	stopIcon_.load("pics/KS51hi32StopProcessor.png", 0, KPixmap::Auto );
	stopAllIcon_.load("pics/KS51hi32StopAllProcessors.png", 0, KPixmap::Auto );
	stepIcon_.load("pics/KS51hi32StepProcessor.png", 0, KPixmap::Auto );
	stepAllIcon_.load("pics/KS51hi32StepAllProcessors.png", 0, KPixmap::Auto );
	stopAllOneBreakpointIcon_.load("pics/KS51hi32StopAllOneBreakpoint.png", 0, KPixmap::Auto );
	
	characterToSerialIcon_.load("pics/KS51hi32CharToSerial.png", 0, KPixmap::Auto);
	characterToSerialEditIcon_.load("pics/KS51hi32CharToSerialEdit.png", 0, KPixmap::Auto);
	clearSerialNetListBoxIcon_.load("pics/KS51hi32ClearSerialList.png", 0, KPixmap::Auto);

	PCMark_.load("pics/KS51hi16WhiteLedOn.png", 0, KPixmap::Auto );
	SPMark_.load("pics/KS51hi16GreenLedOn.png", 0, KPixmap::Auto );
	R0Mark_.load("pics/KS51hi16BlueLedOn.png", 0, KPixmap::Auto );
	R1Mark_.load("pics/KS51hi16YellowLedOn.png", 0, KPixmap::Auto );
	dptrMark_.load("pics/KS51hi16RedLedOn.png", 0, KPixmap::Auto );
	showBreakpointIcon_.load("pics/KS51hi32ShowBreakpoint.png", 0, KPixmap::Auto );
	refreshIcon_.load("pics/KS51hi32Refresh.png", 0, KPixmap::Auto );
	refreshAllIcon_.load("pics/KS51hi32RefreshAll.png", 0, KPixmap::Auto );
	centerWindowIcon_.load("pics/KS51hi32CenterWindow.png", 0, KPixmap::Auto );
	cornerWindowIcon_.load("pics/KS51hi32CornerWindow.png", 0, KPixmap::Auto );
	nextWindowIcon_.load("pics/KS51hi32NextWindow.png", 0, KPixmap::Auto );
	backWindowStateIcon_.load("pics/KS51hi32BackWindow.png", 0, KPixmap::Auto );
	nearWindowToIcon_.load("pics/KS51hi32NearWindow.png", 0, KPixmap::Auto );
	resizeWindowHiddenOffIcon_.load("pics/KS51hi32ResizeWindow.png", 0, KPixmap::Auto );

}

void KSimu51::createMenu(void)
{
	int id;
	//File-----------------------------------------------------------------
	menuFile_ = new KPopupMenu(this);
	menuBar()->insertItem( i18n("&File"), menuFile_);
	//File->Open/Save Work Close
	menuFile_->insertItem(openWorkIcon_, i18n("&Open / Append Work"),this, SLOT(openWork()),CTRL+Key_W);
	menuFile_SaveWork_ID_=menuFile_->insertItem(saveWorkIcon_, i18n("&Save Work"),this, SLOT(saveWork()),CTRL+Key_S);
	menuFile_->setItemEnabled(menuFile_SaveWork_ID_, false);
	menuFile_SaveWorkAs_ID_=menuFile_->insertItem(saveWorkAsIcon_, i18n("Save Work As..."),this, SLOT(saveWorkAs()),CTRL+Key_A);
	menuFile_->setItemEnabled(menuFile_SaveWorkAs_ID_, false);
	menuFile_Close_ID_=menuFile_->insertItem(closeIcon_, i18n("&Close Work"), this, SLOT(closeAllWindows()), CTRL+Key_Z );
	menuFile_->setItemEnabled(menuFile_Close_ID_, false);
	menuFile_->insertSeparator();
	//File-> Quit
	menuFile_->insertItem(quitIcon_, i18n("&Quit"), KApplication::kApplication(), SLOT( closeAllWindows()),CTRL+Key_Q);
	//-----------

	//Edit
	menuEdit_=new KPopupMenu( this );
	menuEdit_ID_=menuBar()->insertItem( i18n("&Edit"), menuEdit_);
	menuBar()->setItemEnabled(menuEdit_ID_, false);
	menuEdit_->insertItem(copyActiveProcessorIcon_, i18n("&Copy Active Processor"),this, SLOT(copyActiveProcessor()),CTRL+Key_C);
	menuEdit_->insertSeparator();
	menuEdit_->insertItem(activeProcessorPropertiesIcon_, i18n("&Properties"),this, SLOT(activeProcessorProperties()),CTRL+Key_P);

	//View
	menuView_=new KPopupMenu( this );
	menuView_ID_=menuBar()->insertItem( i18n("&View"), menuView_);
	menuBar()->setItemEnabled(menuView_ID_, false);
	menuView_->insertItem(refreshIcon_, i18n("Show update "), this, SLOT(showProsessorUpdate()), ALT+Key_F6);
	menuView_UpdateAll_ID_=menuView_->insertItem(refreshAllIcon_, i18n("Show update all"), this, SLOT(showAllProsessorsUpdate()), ALT+Key_F7);
	menuView_->insertSeparator();
	menuView_->insertItem(PCMark_, i18n("Find PC"), this, SLOT(showPC()), ALT+Key_F8);
	menuView_->insertItem(SPMark_, i18n("Find SP"), this, SLOT(showSP()), ALT+Key_F9);
	menuView_->insertItem(R0Mark_, i18n("Find R0 pointer"), this, SLOT(showR0()), ALT+Key_F10);
	menuView_->insertItem(R1Mark_, i18n("Find R1 pointer"), this, SLOT(showR1()), ALT+Key_F11);
	menuView_->insertItem(dptrMark_, i18n("Find DPTR"), this, SLOT(showDPTR()), ALT+Key_F12);
	menuView_->insertSeparator();
	menuView_->insertItem(centerWindowIcon_, i18n("Scroll center"), this, SLOT(centerActiveWindowInActiveWorkspaceLevel()), CTRL+Key_K);
	menuView_->insertItem(cornerWindowIcon_, i18n("Scroll corner"), this, SLOT(cornerActiveWindowInActiveWorkspaceLevel()), CTRL+Key_L);
	menuView_->insertItem(nextWindowIcon_, i18n("Scroll next"), this, SLOT(nextActiveWindowInActiveWorkspaceLevel()), CTRL+Key_N);
	menuView_->insertItem(backWindowStateIcon_, i18n("Back"), this, SLOT(backPreviousWindowStateInActiveWorkspaceLevel()), CTRL+Key_B);
	menuView_->insertSeparator();
	menuView_->insertItem(nearWindowToIcon_, i18n("Move near to W.."), this, SLOT(moveNearActiveWindowToWindowInActiveWorkspaceLevel()),CTRL+Key_M);
	menuView_->insertItem(resizeWindowHiddenOffIcon_, i18n("Cut hidden "), this, SLOT(resizeWindowHiddenOffInActiveWorkspaceLevel()), CTRL+Key_J);

	//Processor
	menuProcessor_=new KPopupMenu( this );
	menuBar()->insertItem( i18n("&Processor"), menuProcessor_);
	//Processor->New
	KPopupMenu* menuProcessor_New = new KPopupMenu( this );
	menuProcessor_->insertItem(newProcessorIcon_, i18n("&New "), menuProcessor_New);

	//Luodaan proserssoriWindow käyttö libbi ja menut prosessoriW libistä
	processorWindowLib_ = new ProcessorsWLib(this, menuProcessor_New, windowWorkspace_);
	connect(processorWindowLib_, SIGNAL(processorCreated(I51BaseW*)), this, SLOT(processorCreatedFromLibrary(I51BaseW*)));

	menuProcessor_->insertSeparator();

	//Processor->Load HEX
	menuProcessor_LoadHexFile_ID_=menuProcessor_->insertItem(loadProgramIcon_, i18n("&Load HEX File..."),this, SLOT(loadEverythinToActiveProcessor()),CTRL+Key_L);
	menuProcessor_->setItemEnabled(menuProcessor_LoadHexFile_ID_, false);
	menuProcessor_LoadHexFileToAll_ID_=menuProcessor_->insertItem(loadProgramToAllIcon_, i18n("Load HEX File To &All..."),this, SLOT(loadEverythinToAllProcessors()),CTRL+Key_O);
	menuProcessor_->setItemEnabled(menuProcessor_LoadHexFileToAll_ID_, false);

	//Debug
	menuDebug_=new KPopupMenu( this );
	menuDebug_ID_=menuBar()->insertItem( i18n("&Debug"), menuDebug_);
	menuBar()->setItemEnabled(menuDebug_ID_, false);
	menuDebug_->insertItem(powerOnResetIcon_, i18n("Power ON Reset"), this, SLOT(powerOnReset()),Key_F7);
	menuDebug_PowerOnResetAll_ID_=menuDebug_->insertItem(powerOnResetAllIcon_, i18n("Power ON Reset All"), this, SLOT(powerOnResetAll()),SHIFT+Key_F7);
	menuDebug_->setItemEnabled(menuDebug_PowerOnResetAll_ID_, false);
	menuDebug_->insertSeparator();
	menuDebug_->insertItem(stopIcon_, i18n("Stop"), this, SLOT(stop()),Key_F8);
	menuDebug_StopAll_ID_=menuDebug_->insertItem(stopAllIcon_, i18n("Stop All"), this, SLOT(stopAll()),SHIFT+Key_F8);
	menuDebug_->setItemEnabled(menuDebug_StopAll_ID_, false);
	menuDebug_->insertItem(runIcon_, i18n("Run"), this, SLOT(run()),Key_F9);
	menuDebug_RunAll_ID_=menuDebug_->insertItem(runAllIcon_, i18n("Run All"), this, SLOT(runAll()),SHIFT+Key_F9);
	menuDebug_->setItemEnabled(menuDebug_RunAll_ID_, false);
	menuDebug_->insertSeparator();
	menuDebug_->insertItem(stepIcon_, i18n("Step"), this, SLOT(step()),Key_F10);
	menuDebug_StepAll_ID_=menuDebug_->insertItem(stepAllIcon_, i18n("Step All"), this, SLOT(stepAll()),SHIFT+Key_F10);
	menuDebug_->setItemEnabled(menuDebug_StepAll_ID_, false);
	//-----------
	//Windows
	menuWindows_ID_=menuBar()->insertItem( i18n("&Windows"), windowsMenu_);
	menuBar()->setItemEnabled(menuWindows_ID_, false);
	//-----------
	//Help
	KPopupMenu *help = helpMenu(QString::null,false);
	menuBar()->insertItem( i18n("&Help"), help);
	help->changeItem(2,intelIcon_,help->text(2));
	//menuBar()->insertItem( i18n("&Help"), customHelpMenu() );

	//Workspacen menu
	workspaceMenu_=new KPopupMenu(this);
	workspaceMenu_->insertItem(i18n("&Windows"), windowsMenu_); //windowsMenu_ kantaluokasta

}

void KSimu51::createToolBars(void)
{
	int idCount=0;
	//File tools
	toolsFile_ = new KToolBar( this, "file operations" );
	addToolBar(toolsFile_, i18n( "File Operations" ), DockTop, TRUE );
	//Save
	toolsFile_Save_ID_=toolsFile_->insertButton(saveWorkIcon_, idCount++, SIGNAL(clicked()), this, SLOT(saveWork()), false, i18n("Save Work"),-1);
	//Loadload
	toolsFile_->insertLineSeparator(); idCount++;
	toolsFile_LoadProgram_ID_ = toolsFile_->insertButton(loadProgramIcon_, idCount++, SIGNAL(clicked()), this, SLOT(loadEverythinToActiveProcessor()), false, i18n("Load program to active processor"),-1);
	toolsFile_LoadProgramToAll_ID_ = toolsFile_->insertButton(loadProgramToAllIcon_, idCount++,SIGNAL(clicked()), this, SLOT(loadEverythinToAllProcessors()), false, i18n("Load program to all processors"),-1);
	toolsFile_->insertSeparator();

	//Debug tools
	idCount=0;
	toolsDebug_ = new KToolBar( this, "debugger operations" );
	addToolBar(toolsDebug_, i18n( "Debug Operations" ), DockTop, FALSE);
	//Reset
	toolsDebug_->insertButton(powerOnResetIcon_, idCount++, SIGNAL(clicked()), this, SLOT(powerOnReset()), true, i18n("Power ON Reset active processor"), -1);
	toolsDebug_PowerOnResetAll_ID_ = toolsDebug_->insertButton(powerOnResetAllIcon_, idCount++, SIGNAL(clicked()), this, SLOT(powerOnResetAll()), false, i18n("Power ON Reset all processors"), -1);
	toolsDebug_->insertLineSeparator();idCount++;
	//Stop
	toolsDebug_->insertButton(stopIcon_, idCount++, SIGNAL(clicked()), this, SLOT(stop()), true, i18n("Stop active processor"), -1);
	toolsDebug_StopAll_ID_ = toolsDebug_->insertButton(stopAllIcon_, idCount++, SIGNAL(clicked()), this, SLOT(stopAll()), false, i18n("Stop all processors"), -1);
	//Run
	toolsDebug_->insertButton(runIcon_, idCount++, SIGNAL(clicked()), this, SLOT(run()), true, i18n("Run program in active processor"), -1);
	toolsDebug_RunAll_ID_ = toolsDebug_->insertButton(runAllIcon_, idCount++, SIGNAL(clicked()), this, SLOT(runAll()), false, i18n("Run program in all processors"), -1);
	toolsDebug_->insertLineSeparator();idCount++;
	//Step
	toolsDebug_->insertButton(stepIcon_, idCount++, SIGNAL(clicked()), this, SLOT(step()), true, i18n("Step active processor"), -1);
	toolsDebug_StepAll_ID_=toolsDebug_->insertButton(stepAllIcon_, idCount++, SIGNAL(clicked()), this, SLOT(stepAll()), false, i18n("Step all processors synchronously"), -1);
	toolsDebug_->insertLineSeparator();idCount++;
	toolsDebug_StopAllAtBreakpoint_ID_=toolsDebug_->insertButton(stopAllOneBreakpointIcon_, idCount++, false, i18n("Stop all at one processor breakpoint"), -1);
	toolsDebug_->setToggle(toolsDebug_StopAllAtBreakpoint_ID_, true);
	toolsDebug_->setEnabled(false);

	//View tools
	idCount=0;
	toolsView_ = new KToolBar( this, "view operations" );
	addToolBar(toolsView_, i18n( "View operations" ), DockTop, FALSE);

	toolsView_Update_ID_=toolsView_->insertButton(refreshIcon_, idCount++, SIGNAL(clicked()), this, SLOT(showProsessorUpdate()), true,
												  i18n("Show update"),-1);
	toolsView_->setToggle(toolsView_Update_ID_, true);
	toolsView_UpdateAll_ID_=toolsView_->insertButton(refreshAllIcon_, idCount++, SIGNAL(clicked()), this, SLOT(showAllProsessorsUpdate()), true, i18n("Show update all"),-1);
	toolsView_->setToggle(toolsView_UpdateAll_ID_, true);
	toolsView_->insertLineSeparator(); idCount++;
	toolsView_ShowBreakpoint_ID_=toolsView_->insertButton(showBreakpointIcon_, idCount++, true, i18n("Go breakpoint at run."),-1);
	toolsView_->setToggle(toolsView_ShowBreakpoint_ID_, true);
	toolsView_->insertLineSeparator(); idCount++;

	toolsView_->insertButton(PCMark_, idCount++, SIGNAL(clicked()), this, SLOT(showPC()), true, i18n("Find PC(disassemble>source>rom<<)"),-1);
	toolsView_->insertButton(SPMark_, idCount++, SIGNAL(clicked()), this, SLOT(showSP()), true, i18n("Find SP"),-1);
	toolsView_->insertButton(R0Mark_, idCount++, SIGNAL(clicked()), this, SLOT(showR0()), true, i18n("Find R0 (internal<>external)"),-1);
	toolsView_->insertButton(R1Mark_, idCount++, SIGNAL(clicked()), this, SLOT(showR1()), true, i18n("Find R1 (internal<>external)"),-1);
	toolsView_->insertButton(dptrMark_, idCount++, SIGNAL(clicked()), this, SLOT(showDPTR()), true, i18n("Find DPTR"),-1);

	toolsView_->insertLineSeparator(); idCount++;

	toolsView_->insertButton(centerWindowIcon_, idCount++, SIGNAL(clicked()), this, SLOT(centerActiveWindowInActiveWorkspaceLevel()), true, i18n("Scroll active window to center"),-1);
	//toolsView_->setDelayedPopup(idCount-1, , false);
	toolsView_->insertButton(cornerWindowIcon_, idCount++, SIGNAL(clicked()), this, SLOT(cornerActiveWindowInActiveWorkspaceLevel()), true, i18n("Scroll active window to corner"),-1);
	toolsView_->insertButton(nextWindowIcon_, idCount++, SIGNAL(clicked()), this, SLOT(nextActiveWindowInActiveWorkspaceLevel()), true, i18n("Scroll next window to center"),-1);
	toolsView_->insertButton(backWindowStateIcon_, idCount++, SIGNAL(clicked()), this, SLOT(backPreviousWindowStateInActiveWorkspaceLevel()), true, i18n("Back to previous window state"),-1);

	toolsView_->insertLineSeparator(); idCount++;

	toolsView_->insertButton(nearWindowToIcon_, idCount++, SIGNAL(clicked()), this, SLOT(moveNearActiveWindowToWindowInActiveWorkspaceLevel()),true, i18n("Move active near to window "),-1);
	toolsView_->insertButton(resizeWindowHiddenOffIcon_, idCount++, SIGNAL(clicked()), this, SLOT(resizeWindowHiddenOffInActiveWorkspaceLevel()),true, i18n("Cut active window "),-1);

	toolsView_->setEnabled(false);

	//Serial Port ToolBAARI
	addSerialPortNet();
}

void KSimu51::processorCreatedFromLibrary(I51BaseW* pToCreatedProcessor)
{
	connect(pToCreatedProcessor, SIGNAL(message_SetStatusBar(const int, const QString&, const unsigned int )), this, SLOT(messageFromProcessorW_SetStatusBar(const int, const QString&, const unsigned int )));
	connect(pToCreatedProcessor, SIGNAL(message_Close(const int )), this, SLOT(messageFromProcessorW_Close(const int )));
	connect(pToCreatedProcessor, SIGNAL(message_SetWindowLevel(const int, const int )), this, SLOT(messageFromProcessorW_SetWindowLevel(const int, const int )));
	
	pToCreatedProcessor->setIcon(intelIcon_);

	//Haetaan processorin koneaika
	double machineT=pToCreatedProcessor->readProcessorMachineCycleTime();
	if(machineT<fastestProcessorMachineTime_)
		fastestProcessorMachineTime_=machineT;

	countUpWindowsCounter();
	//Enabloidaan menuja ja toolbaareja
	enableDisableMenusToolbaars(false);

	statusBarLabel_->setText( i18n("Load [.hex] file to processor or add more processors"));
	//return w;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define SERIALTOOLS_WIDTH 105
void KSimu51::addSerialPortNet()
{
	int id, index;
	serialTools_ = new KToolBar( this, "serial operations" );
	addToolBar( serialTools_, i18n( "Serial Operations" ), DockRight, FALSE );
	serialTools_->setMovingEnabled(false);
	serialTools_->setMinimumWidth(50);

	serialTools_->insertButton(characterToSerialEditIcon_, 0, SIGNAL(clicked()), this, SLOT(editSerialNet()), true, i18n("Edit serial net"), -1);

	serialNetEditLine_ = new KLineEdit(serialTools_,"serialNetEditLine_");
	QFont serialNetEditLine_font(serialNetEditLine_->font());
	serialNetEditLine_font.setFamily(FONTFAMILY);
	serialNetEditLine_font.setPointSize(10);
	serialNetEditLine_->setFont(serialNetEditLine_font);
	serialNetEditLine_->setFrameShape( QTable::GroupBoxPanel );
	serialNetEditLine_->setMaximumWidth(SERIALTOOLS_WIDTH);
	serialNetEditLine_->setMaxLength(3);
	index=serialTools_->insertWidget(-1, 65,serialNetEditLine_,-1);
	connect(serialNetEditLine_, SIGNAL(returnPressed()), this, SLOT(serialNetEditLineReturnPressed()));
	connect(serialNetEditLine_, SIGNAL(textChanged(const QString &)), this, SLOT(serialNetEditLineTextChanged(const QString & )));

	serialNetEditListBox_ = new KListBox(serialTools_, "serialNetEditListBox_", 0);
	QFont serialNetEditListBox_font(serialNetEditListBox_->font());
	serialNetEditListBox_font.setFamily(FONTFAMILY);
	serialNetEditListBox_font.setPointSize(10);
	serialNetEditListBox_->setFont(serialNetEditListBox_font);
	serialNetEditListBox_->setFrameShape( QTable::GroupBoxPanel );
	serialNetEditListBox_->setMaximumWidth(SERIALTOOLS_WIDTH);
	serialNetEditListBox_->setMinimumHeight(height()/3);
	//serialNetEditListBox_->setGeometry(0,0,SERIALTOOLS_WIDTH,140);
	serialNetEditListBox_->setSelectionMode(QListBox::Extended);
	index=serialTools_->insertWidget(-1, SERIALTOOLS_WIDTH, serialNetEditListBox_,-1);
	id=serialTools_->idAt(index);
	serialTools_->setItemAutoSized(id, true);

	serialTools_->insertButton(characterToSerialIcon_, 0, SIGNAL(clicked()), this, SLOT(charToSerialNet()), true, i18n("Character to serial net"), -1);

	serialNetHistoryListBox_ = new KListBox(serialTools_, "serialNetHistoryListBox_", 0);
	QFont serialNetHistoryListBox_font(serialNetHistoryListBox_->font());
	serialNetHistoryListBox_font.setFamily(FONTFAMILY);
	serialNetHistoryListBox_font.setPointSize(10);
	serialNetHistoryListBox_->setFont(serialNetHistoryListBox_font);
	serialNetHistoryListBox_->setFrameShape( QTable::GroupBoxPanel );
	serialNetHistoryListBox_->setSelectionMode(QListBox::NoSelection);
	serialNetHistoryListBox_->setMaximumWidth(SERIALTOOLS_WIDTH);
	serialNetHistoryListBox_->setMaximumHeight(serialTools_->height()/2);
	index=serialTools_->insertWidget(-1, SERIALTOOLS_WIDTH, serialNetHistoryListBox_,-1);
	id=serialTools_->idAt(index);
	serialTools_->setItemAutoSized(id, true);

	serialTools_->insertButton(clearSerialNetListBoxIcon_, 0, SIGNAL(clicked()), this, SLOT(clearSerialNetListBox()), true, i18n("Clear serial net history"), -1);

	serialIndicator_=new LedIndicatorGrid(5, 1, serialTools_, "serialIndicator_", 0);
	index=serialTools_->insertWidget(-1, SERIALTOOLS_WIDTH, serialIndicator_,-1);
	id=serialTools_->idAt(index);
	serialTransmitOnLed_INDEX_=serialIndicator_->addLed(0,0,"Tx ON", Qt::green);
	serialTransmitErrorLed_INDEX_=serialIndicator_->addLed(1,0,"ERROR", Qt::red);
	serialIndicator_->ledOFF(serialTransmitOnLed_INDEX_);
	serialIndicator_->ledOFF(serialTransmitErrorLed_INDEX_);

	serialNetEditListBoxAddPopupMenu();
}

void KSimu51::serialNetEditLineTextChanged(const QString& t)
{
	int cp=serialNetEditLine_->cursorPosition();
	char cs;
	if(cp)
	{
		cs=t[cp-1];
		if(!(((cs>='0')&&(cs<='9'))||((cs>='A')&&(cs<='F'))||((cs>='a')&&(cs<='f'))))
			serialNetEditLine_->backspace();
	}
}

void KSimu51::editSerialNet(void)
{
	serialNetEditListBox_->setCurrentItem(serialNetEditListBox_->topItem());
}

void KSimu51::charToSerialNet(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	if((serialNetEditListBox_->currentItem())!=-1)
	{
		int data;
		QString sStr=serialNetEditListBox_->currentText();
		serialNetEditListBox_->setCurrentItem(serialNetEditListBox_->currentItem()+1);
		serialNetEditListBox_->clearSelection();
		serialNetEditListBox_->setSelected(serialNetEditListBox_->currentItem(),true);
		string strApu=sStr.mid(3,3);
		data=hexStringToHex(strApu);
		strApu=decToString(++serialNetCounter_,2,'0');
		strApu+=":00>";
		strApu+=hexToString(data,2);
		serialNetHistoryListBox_->insertItem(strApu,0);
		//Lähetetään kaikille.
		for ( int i = 0; i < int(windows.count()); ++i )
		{
			I51BaseW *processorW = (I51BaseW*)windows.at(i);
			processorW->writeDataToSerialPort(data);
		}
		//Lähetetään vain ekalle.
		//I51BaseW *processorW = (I51BaseW*)windows.at(0);
		//processorW->writeDataToSerialPort(data);
	}
}


void KSimu51::updateSerialNet(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	int numProcessorsTransmit=0,data,pindex,eiLahetetaIndex;
	string strApu;
	bool transmittingProcessorTxDValue;
	bool lahetaSarjaporttiin=false;
	serialIndicator_->ledOFF(serialTransmitOnLed_INDEX_);
	serialIndicator_->ledOFF(serialTransmitErrorLed_INDEX_);
	//Skannataan mikä processori lähettää
	for(int i=0; i < int(windows.count()); ++i)
	{
		I51BaseW *tw=(I51BaseW*)windows.at(i);
		bool transmitting=tw->isSerialPortTransmitOn();
		bool wasTransmitting=tw->readPrevoiusStepSerialTransmitFlag();
		if(!transmitting&&wasTransmitting)
		{
			//Jos edellisellä kerralla lähetys päällä.
			data=tw->lastTransmittedData();
			pindex=tw->readProcessorIndex();
			strApu=decToString(++serialNetCounter_,2,'0');
			strApu+=":";
			strApu+=hexToString(pindex,2);
			strApu+=">";
			strApu+=hexToString(data,2);
			if(serialNetHistoryListBox_->count()>255)
				serialNetHistoryListBox_->removeItem(255);
			serialNetHistoryListBox_->insertItem(strApu,0);
			lahetaSarjaporttiin=true;
		}
		if(transmitting||wasTransmitting)
		{
			numProcessorsTransmit++;
			serialIndicator_->ledON(serialTransmitOnLed_INDEX_);
			transmittingProcessorTxDValue=tw->readTxDPin();
			eiLahetetaIndex=i;
		}
	}
	//Jos enemmän kuin yksi lähettää ERROR LED päälle
	if(numProcessorsTransmit>1)
	{
		serialIndicator_->ledON(serialTransmitErrorLed_INDEX_);
	}
	//Luetaan sarjaportista(HW) jos päällä
	//-KOODIA-
	//Lähetetään muille.
	if(numProcessorsTransmit)
	{
		for(int i=0; i < int(windows.count()); ++i)
		{
			I51BaseW *tw = (I51BaseW*)windows.at(i);
			//Ei lähetetä itelle
			if(i!=eiLahetetaIndex)
				tw->writeRxDPin(transmittingProcessorTxDValue);
		}
	}
	if(lahetaSarjaporttiin)
	{
		//Lähetetään myös sarjaporttiin(HW).
		//-KOODIA-
	}
}

void KSimu51::clearSerialNetListBox(void)
{
	serialNetHistoryListBox_->clear();
	serialIndicator_->ledOFF(serialTransmitOnLed_INDEX_);
	serialIndicator_->ledOFF(serialTransmitErrorLed_INDEX_);
	serialNetCounter_=0;
}

void KSimu51::serialNetEditListBoxAddPopupMenu(void)
{
	serialEditListBoxMenu_=new KPopupMenu(this);

//   KPopupMenu* insertLine = new KPopupMenu( this );
	serialEditListBoxMenu_->insertItem(i18n("&Insert"), this,SLOT(serialEditListBoxInsertLineActivated()));

	serialEditListBoxMenu_->insertSeparator();

//   KPopupMenu* removeLine = new KPopupMenu( this );
	serialEditListBoxMenu_->insertItem(i18n("&Remove"), this,SLOT(serialEditListBoxRemoveLineActivated()));

	KPopupMenu* removeAllLines = new KPopupMenu( this );
	serialEditListBoxMenu_->insertItem(i18n("R&emove All"), this,SLOT(serialEditListBoxRemoveAllLinesActivated()));

	serialEditListBoxMenu_->insertSeparator();

	KPopupMenu* clearSelection = new KPopupMenu( this );
	serialEditListBoxMenu_->insertItem(i18n("&Clear Selection"), this,SLOT(serialEditListBoxClearSelectionActivated()));

	connect(serialNetEditListBox_, SIGNAL(rightButtonClicked( QListBoxItem*, const QPoint&)), this, SLOT(serialNetEditListBoxRightButtonClicked(QListBoxItem*, const QPoint &  )));
}

void KSimu51::serialNetEditLineReturnPressed(void)
{
	QString txt;
	bool hits=false;
	for(int n=0;n<serialNetEditListBox_->numRows();n++)
	{
		if(serialNetEditListBox_->isSelected(n))
		{
			txt=decToString(n+1,2,'0');
			txt+=">";
			txt+=serialNetEditLine_->text();
			serialNetEditListBox_->removeItem(n);
			serialNetEditListBox_->insertItem(txt,n);
			hits=true;
		}
	}
	if(!hits)
	{
		txt=decToString(serialNetEditListBox_->numRows()+1,2,'0');
		txt+=">";
		txt+=serialNetEditLine_->text();
		serialNetEditListBox_->insertItem(txt,-1);
	}
	serialNetEditListBoxOrderItems();
	serialNetEditLine_->clear();
}

void KSimu51::serialNetEditListBoxRightButtonClicked(QListBoxItem* item, const QPoint& p)
{
	//currentItemToToggleBreakpoint=disassembleListBox->currentItem();
	serialEditListBoxMenu_->popup(p);
}

void KSimu51::serialNetEditListBoxOrderItems(void)
{
	if(serialNetEditListBox_->numRows())
	{
		for(int n=0;n<serialNetEditListBox_->numRows();n++)
		{
			QString txt=serialNetEditListBox_->text(n);
			txt.replace(0, 2, decToString(n+1,2,'0'));
			serialNetEditListBox_->removeItem(n);
			serialNetEditListBox_->insertItem(txt,n);
		}
	}
}

void KSimu51::serialEditListBoxInsertLineActivated(void)
{
	QString txt=decToString(serialNetEditListBox_->currentItem()+1,2,'0');
	txt+=">";
	txt+=serialNetEditLine_->text();
	serialNetEditListBox_->insertItem(txt,serialNetEditListBox_->currentItem());

	serialNetEditListBoxOrderItems();
}

void KSimu51::serialEditListBoxRemoveLineActivated(void)
{
	if(serialNetEditListBox_->numRows())
	{
		for(int n=0;n<serialNetEditListBox_->numRows();n++)
		{
			if(serialNetEditListBox_->isSelected(n))
				serialNetEditListBox_->removeItem(n--);
		}
	}
	serialNetEditListBoxOrderItems();
}

void KSimu51::serialEditListBoxRemoveAllLinesActivated(void)
{
	serialNetEditListBox_->clear();
}

void KSimu51::serialEditListBoxClearSelectionActivated(void)
{
	serialNetEditListBox_->clearSelection();
}


//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

void KSimu51::powerOnReset()
{
	bool wasRunning=false;
	I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(processorW->isReadyToRun())
	{
		QString qStrApu;
		if(processorW->isRunning())
		{
			processorW->stop();
			wasRunning=true;
		}
		processorW->powerOnReset();
		qStrApu=i18n("Reseted...");
		if(wasRunning)
		{
			processorW->run();
			qStrApu=i18n("Running...");
		}
		QString qStrApu2(processorW->caption());
		qStrApu2+=" : ";
		qStrApu2+=qStrApu;
		statusBarLabel_->setText(qStrApu2);
		updateSerialNet();
	}
}

void KSimu51::powerOnResetAll()
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	bool wasRunning=false;
	for ( int i = 0; i < int(windows.count()); ++i )
	{
		wasRunning=false;
		I51BaseW *processorW = (I51BaseW*)windows.at( i );
		if(processorW->isReadyToRun())
		{
			if(processorW->isRunning())
			{
				processorW->stop();
				wasRunning=true;
			}
			processorW->powerOnReset();
			if(wasRunning)
			{
				processorW->run();
			}
		}
	}
	updateSerialNet();
	//Messaget
	QString qStrApu2(i18n("All processors : Reseted..."));
	statusBarLabel_->setText(qStrApu2);
}

//Runiin ja Stoppeihin enemmän älliä synkronointiin
void KSimu51::run()
{
	I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(processorW && !(processorW->isRunning()))
	{
		QString qStrMessage;
		if(processorW->isReadyToRun())
		{
			processorW->run();
			if(!(runTimer_->isActive()))
				runTimer_->start(runTimerInterval_, false);
			//Messaget
			QString qStrApu=i18n("Running...");
			qStrMessage=processorW->caption();
			qStrMessage+=" : ";
			qStrMessage+=qStrApu;
		}
		else
		{
			//Message ohjelmaa ei ladattu
			QString qStrMessage(processorW->caption());
			qStrMessage+=" : ";
			qStrMessage+=i18n("Program not loaded(Internal/External)!");
		}
		//Message status baariin
		statusBarLabel_->setText(qStrMessage);
	}
}

void KSimu51::runAll(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	QString qStrApu(i18n("Running..."));
	bool allStartRunning=true;
	for ( int i = 0; i < int(windows.count()); ++i)
	{
		I51BaseW *processorW = (I51BaseW*)windows.at(i);
		if(!processorW->isRunning())
		{
			if(processorW->isReadyToRun())
			{
				processorW->run();
			}
			else
				allStartRunning=false;
		}
	}
	runTimer_->start(runTimerInterval_,false);
	//Messaget
	QString qStrMessage;
	if(allStartRunning)
		qStrMessage=i18n("All processors : ");
	else
		qStrMessage=i18n("Not all processors : ");
	qStrMessage+=qStrApu;
	statusBarLabel_->setText(qStrMessage);
}

void KSimu51::stop(void)
{
	I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(processorW && processorW->isRunning())
	{
		processorW->stop();
		processorW->updateProcessorWindows();
		//Messaget
		QString qStrApu(i18n("Stopped..."));
		QString qStrApu2(processorW->caption());
		qStrApu2+=" : ";
		qStrApu2+=qStrApu;
		statusBarLabel_->setText(qStrApu2);
		//Stopataan timeri jos yksikään ikkuna ei juokse
		bool isRunning=false;
		QWidgetList windows = windowWorkspace_->windowList();
		for ( int i = 0; i < int(windows.count()); ++i )
		{
			processorW = (I51BaseW*)windows.at( i );
			if((processorW->isRunning()))
				isRunning=true;
		}
		if(!isRunning)
			runTimer_->stop();
	}
}

void KSimu51::stopAll(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	bool wasRunning=false;
	QString qStrApu;
	for ( int i = 0; i < int(windows.count()); ++i )
	{
		I51BaseW *processorW = (I51BaseW*)windows.at( i );
		if(processorW->isRunning())
		{
			processorW->stop();
			processorW->updateProcessorWindows();
			qStrApu=i18n("Stopped...");
			wasRunning=true;
		}
	}
	runTimer_->stop();
	if(wasRunning)
	{
		QString qStrApu2(i18n("All processors : "));
		qStrApu2+=qStrApu;
		statusBarLabel_->setText(qStrApu2);
	}
}

void KSimu51::step(void)
{
	I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(processorW)
	{
		if(!processorW->isRunning())
		{
			if(processorW->isReadyToRun())
			{
				processorW->step(true, 0.0); //Tehdään 1 steppi asettamalla aikalaskenta nollaksi??
				processorW->updateProcessorWindows();
				processorW->isAtBreakpoint(); //Resetoidaaan bitChange breakpointit
			}
			else
			{
				if(processorW->readEAPin())
					statusBarLabel_->setText(i18n("Program not loaded internal program memory!"));
				else
					statusBarLabel_->setText(i18n("Program not loaded external program memory!"));
			}
		}
		updateSerialNet();
	}
}

void KSimu51::stepAll(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	bool steps=false;
	do
	{
		int runningCount=0;
		for ( int i = 0; i < int(windows.count()); ++i )
		{
			I51BaseW *processorW = (I51BaseW*)windows.at( i );
			if(!processorW->isRunning())
			{
				if(processorW->isReadyToRun())
				{
					if(processorW->step(true, fastestProcessorMachineTime_))
					{
						steps=true;
						//processorW->updateExternalDataIOAreaAndProcessor(false);
						//processorW->stepAfterUpdate();
						processorW->updateProcessorWindows();
						processorW->isAtBreakpoint(); //Resetoidaaan bitChange breakpointit
					}
				}
				else
				{
					QString qStrMessage(processorW->caption());
					if(processorW->readEAPin())
						qStrMessage+=i18n(": Program not loaded internal program memory!");
					else
						qStrMessage+=i18n(": Program not loaded external program memory!");
					statusBarLabel_->setText(qStrMessage);
				}
			}
			else
				runningCount++;
		}
		if(runningCount==int(windows.count()))
			steps=true;
	}
	while(!steps);//Tehdään niin kauan,että joku steppaa. Tai kaikki juoksee
	updateSerialNet();
}


void KSimu51::runTimerTimeout(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;
	bool noOneIsRunning=true;
	bool updeittaaSerialNet=false;
	bool stopAllAtBreakpoint=false;
	I51BaseW* activeProcessorW = (I51BaseW*)windowWorkspace_->activeWindow();
	for(int i = 0; i < int(windows.count()); ++i)
	{
		I51BaseW *processorW=(I51BaseW*)windows.at(i);
		if(processorW->isRunning())
		{
			if(processorW->step(false, fastestProcessorMachineTime_))
			{
				if(toolsView_->isButtonOn(toolsView_Update_ID_))
					activeProcessorW->updateProcessorWindows();
				else
					if(toolsView_->isButtonOn(toolsView_UpdateAll_ID_))
						processorW->updateProcessorWindows();

				updeittaaSerialNet=true;
				if(processorW->isAtBreakpoint())
				{
					if(toolsDebug_->isButtonOn(toolsDebug_StopAllAtBreakpoint_ID_))
						stopAllAtBreakpoint=true;
					processorW->stop();
					processorW->setIcon(intelIcon_);
					processorW->updateProcessorWindows();
					if(toolsView_->isButtonOn(toolsView_ShowBreakpoint_ID_))
					{
						processorW->showBreakpoint();
						activeWindowLevel_=KSimu51_NameS::WINDOWLEVEL_PROCESSOR;
					}
					//Messaget
					QString qStrApu(i18n("Stopped at breakpoint..."));
					//processorW->updateDisassembleActiveLable(qStrApu);
					QString qStrApu2(processorW->caption());
					qStrApu2+=" : ";
					qStrApu2+=qStrApu;
					statusBarLabel_->setText(qStrApu2);
				}
			}
			noOneIsRunning=false;
		}
	}
	if(stopAllAtBreakpoint)
	{
		for(int i = 0; i < int(windows.count()); ++i)
		{
			I51BaseW *processorW = (I51BaseW*)windows.at(i);
			processorW->stop();
			processorW->setIcon(intelIcon_);
			processorW->updateProcessorWindows();
			//if(toolsView_->isButtonOn(toolsView_ShowBreakpoint_ID_))
				//processorW->showBreakpoint();
		}
		//Messaget
		QString qStrApu(i18n("All Stopped at breakpoint..."));
		statusBarLabel_->setText(qStrApu);
		noOneIsRunning=true;
	}
	if(updeittaaSerialNet)
		updateSerialNet();
	if(noOneIsRunning)
		runTimer_->stop();
}

void KSimu51::showPC(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		pW->showPointerPC();
		activeWindowLevel_=KSimu51_NameS::WINDOWLEVEL_PROCESSOR;
	}
}

void KSimu51::showSP(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		pW->showPointerSP();
		activeWindowLevel_=KSimu51_NameS::WINDOWLEVEL_PROCESSOR;
	}
}

void KSimu51::showR0(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		pW->showPointerR0();
		activeWindowLevel_=KSimu51_NameS::WINDOWLEVEL_PROCESSOR;
	}
}

void KSimu51::showR1(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		pW->showPointerR1();
		activeWindowLevel_=KSimu51_NameS::WINDOWLEVEL_PROCESSOR;
	}
}

void KSimu51::showDPTR(void)
{
	findDPTRWasDone_=true;
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		pW->showPointerDPTR();
		activeWindowLevel_=KSimu51_NameS::WINDOWLEVEL_PROCESSOR;
	}
}

void KSimu51::showProsessorUpdate(void)
{
	if(toolsView_->isButtonOn(toolsView_Update_ID_))
		toolsView_->setButton(toolsView_UpdateAll_ID_, false);
}

void KSimu51::showAllProsessorsUpdate(void)
{
	if(toolsView_->isButtonOn(toolsView_UpdateAll_ID_))
		toolsView_->setButton(toolsView_Update_ID_, false);
}

void KSimu51::centerActiveWindowInActiveWorkspaceLevel(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				centerActiveWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->centerActiveWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->centerActiveWindowInExternalArea();
				break;
			default:   ;
		}
	}
}

void KSimu51::cornerActiveWindowInActiveWorkspaceLevel(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				cornerActiveWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->cornerActiveWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->cornerActiveWindowInExternalArea();
				break;
			default:   ;
		}
	}
}

void KSimu51::nextActiveWindowInActiveWorkspaceLevel(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				nextWindowToShow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->nextWindowToShow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->nextWindowToShowInExternalArea();
				break;
			default:   ;
		}
	}
}

void KSimu51::backPreviousWindowStateInActiveWorkspaceLevel(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				backPreviousWindowState();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->backPreviousWindowState();
				if(!findDPTRWasDone_)
					break;
				findDPTRWasDone_=false;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->backPreviousWindowStateInExternalArea();
				break;
			default:   ;
		}
	}
}

void KSimu51::moveNearActiveWindowToWindowInActiveWorkspaceLevel(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				activeWindowNearToWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->activeWindowNearToWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->activeWindowNearToWindowInExternalArea();
				break;
			default:   ;
		}
	}
}


void KSimu51::resizeWindowHiddenOffInActiveWorkspaceLevel(void)
{
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW)
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				activeWindowResizeHiddenOff();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->activeWindowResizeHiddenOff();
				break;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->activeWindowResizeHiddenOffInExternalArea();
				break;
			default:   ;
		}
	}
}

void KSimu51::setActiveWindowColorInActiveWorkspaceLevel(void)
{
	//Unmarkataan kaikki
	QWidgetList windows = windowWorkspace_->windowList();
	if(!windows.count())
		return;
	unmarkWindows();
	for(int i=0;i<int(windows.count());++i)
	{
		I51BaseW *processorW = (I51BaseW*)windows.at( i );
		processorW->unmarkWindowsInExternalArea();
		processorW->unmarkWindows();
	}
	//Markataan aktiivi
	I51BaseW* pW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(pW&&pW->isReadyToRun())
	{
		switch(activeWindowLevel_)
		{
			case KSimu51_NameS::WINDOWLEVEL_MAIN:
				markActiveWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_PROCESSOR:
				pW->markActiveWindow();
				break;
			case KSimu51_NameS::WINDOWLEVEL_EXTERNAL:
				pW->markActiveWindowInExternalArea();
				break;
			default:   ;
		}
	}
}

void KSimu51::loadEverything(bool all, QString hexFilename)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if (!windows.count())
		return;
	if(all)
	{
		for(int i=0;i<int(windows.count());++i)
		{
			I51BaseW *processorW = (I51BaseW*)windows.at( i );
			if(processorW->isRunning())
			{
				processorW->stop();
				processorW->setIcon(intelIcon_);
			}
			processorW->loadProgramAndCreateWindows(hexFilename);
		}
		statusBarLabel_->setText(i18n("All processors loaded : ")+hexFilename);
	}
	else
	{
		I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
		if(processorW)
		{
			if(processorW->isRunning())
			{
				processorW->stop();
				processorW->setIcon(intelIcon_);
			}
			processorW->loadProgramAndCreateWindows(hexFilename);
		}
	}
	//Enabloidaan menuja ja toolbaareja
	enableDisableMenusToolbaars(false);
}

QString KSimu51::getHexFileNameToOpen(void)
{
	QString pathTo=workPath_;
	QString hexFileToOpen=KFileDialog::getOpenFileName(pathTo, "*.hex *.ihx", this, i18n("Load [.hex] file "));
	return hexFileToOpen;
}

void KSimu51::loadEverythinToActiveProcessor()
{
	QWidgetList windows = windowWorkspace_->windowList();
	if(windows.count())
	{

		QString filename=getHexFileNameToOpen();
		if(filename!=QString::null)
		{
			//Overall progresbaarin asetus
			setProgressBar(100);

			loadEverything(false, filename);

			//Overall progresbaarin nollaus
			resetProgressBar();
		}
	}
}

void KSimu51::loadEverythinToAllProcessors()
{
	QWidgetList windows = windowWorkspace_->windowList();
	if(windows.count())
	{

		QString filename=getHexFileNameToOpen();
		if(filename!=QString::null)
		{
			//Overall progresbaarin asetus
			setProgressBar(int(windows.count())*100);

			loadEverything(true, filename);

			//Overall progresbaarin nollaus
			resetProgressBar();
		}
	}
}

void KSimu51::copyActiveProcessor(void)
{
	I51BaseW* sourceProcessorW=(I51BaseW*)windowWorkspace_->activeWindow();
	if(sourceProcessorW)
	{

		processorWindowLib_->addProcessor(0, sourceProcessorW->readProcessorType(),sourceProcessorW->readProcessorWindowName());
		QWidgetList windows = windowWorkspace_->windowList();
		int numWins=windows.count();
		I51BaseW *destinationProcessorW = (I51BaseW*)windows.at(numWins-1);

		//Overall progresbaarin asetus jos lähdeprocessorissa on ohjelma ladattuna.
		if(sourceProcessorW->isReadyToRun())
		{
			setProgressBar(100);
			countProgressBar(1);
		}

		destinationProcessorW->copy(sourceProcessorW);

		//Messaget
		QString qStrApu=sourceProcessorW->caption();
		qStrApu+=" : Copied...";
		statusBarLabel_->setText(qStrApu);

		enableDisableMenusToolbaars(false);
		//Overall progresbaarin nollaus
		resetProgressBar();
	}
}

void KSimu51::activeProcessorProperties(void)
{
	I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(processorW)
	{
	}
}


void KSimu51::readCFG(void)
{
	QFile configFile(CONFIGFILENAME);
	if(configFile.open(IO_ReadOnly))
	{
		QDataStream configFileStream( &configFile );

		configFileStream>>workFilename_;
		configFileStream>>workPath_;

		configFile.close();

		openWorkFilename_=workFilename_;
	}
	else
		workPath_=QDir::homeDirPath();
}


void KSimu51::saveCFG(void)
{
	QFile configFile(CONFIGFILENAME);
	if(configFile.open(IO_WriteOnly))
	{
		QDataStream configFileStream( &configFile );
		configFileStream<<(QString)workFilename_;
		configFileStream<<(QString)workPath_;
		configFile.close();
	}
}


void KSimu51::saveWorkDo(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if((windows.count())&&(workFilename_!=QString::null))
	{
		stopAll();
		//Talletetaan ensin cfg
		workPath_=workFilename_.left(workFilename_.findRev('/', -1, TRUE)+1);
		saveCFG();
		QFile saveFile(workFilename_);
		if(saveFile.open(IO_WriteOnly))
		{
			setProgressBar(100*int(windows.count()));

			QDataStream saveFileStream( &saveFile );

			//Talletetaan prosessorien määrä
			saveFileStream<<(Q_UINT16)0<<(Q_UINT8)QUINT16<<(Q_UINT16)int(windows.count());

			for(int i=0;i<int(windows.count());++i)
			{
				I51BaseW *processorW = (I51BaseW*)windows.at( i );
				countProgressBar(20);
				saveFileStream<<(Q_UINT16)0<<(Q_UINT8)QUINT16<<(Q_UINT16)processorW->readProcessorIndex();//Prosessorin indeksi
				saveFileStream<<(Q_UINT16)1<<(Q_UINT8)QUINT16<<(Q_UINT16)processorW->readProcessorType();   //Prosessorin tyyyppi
				countProgressBar(20);
				saveFileStream<<(Q_UINT16)2<<(Q_UINT8)QSTRING<<processorW->readProcessorWindowName(); //Nimi prosesssorin ryhmä
				saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
				countProgressBar(20);

				processorW->saveWorkDataToFile(saveFileStream);      //Talletetaan data ja ulkoiset komponentit
				countProgressBar(20);
			}
			countProgressBar(20);

			//Tähän tämän luokan talletettavat
			saveFileStream<<(Q_UINT16)0<<(Q_UINT8)QUINT32<<(Q_UINT32)runTimerInterval_;
			saveFileStream<<(Q_UINT16)1<<(Q_UINT8)QUINT32<<(Q_UINT32)serialNetCounter_;
			saveFileStream<<(Q_UINT16)2<<(Q_UINT8)QUINT16<<(Q_UINT16)windowState(); //Tämän ikkunan tila

			//Toggle Buttons
			saveFileStream<<(Q_UINT16)3<<(Q_UINT8)QUINT8<<(Q_UINT8)toolsDebug_->isButtonOn(toolsDebug_StopAllAtBreakpoint_ID_);
			saveFileStream<<(Q_UINT16)4<<(Q_UINT8)QUINT8<<(Q_UINT8)toolsView_->isButtonOn(toolsView_ShowBreakpoint_ID_);
			saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

			saveFile.close();

			workSavedFirstTime_=true;
			resetProgressBar();
			statusBarLabel_->setText(i18n("Work saved"));
			setCaption(workFilename_);
		}
	}
}

void KSimu51::saveWork(void)
{
	if(workSavedFirstTime_)
		saveWorkDo();
	else
		saveWorkAs();
}

void KSimu51::saveWorkAs(void)
{
	QString pathTo=workPath_;
	QString saveWorkFilename=KFileDialog::getSaveFileName(pathTo, "*.work51", this, i18n("Save Work as"));
	if(saveWorkFilename!=QString::null)
	{
		workFilename_=saveWorkFilename;
		saveWorkDo();
	}
}


void KSimu51::openWorkDo(void)
{
	if(openWorkFilename_!=QString::null)
	{
		QFile readFile(openWorkFilename_);
		if(readFile.open(IO_ReadOnly))
		{
			if(openWorkFirstTimeAfterClear_)
			{
				workFilename_=openWorkFilename_;
				workPath_=workFilename_.left(workFilename_.findRev('/', -1, TRUE)+1);
			}

			//All readWorkDataFromFile() handler.
			auto funcHandleUnknownIndexRead = [](QDataStream& readStream) ->bool
			{
				Q_UINT8 savedWorkDataType;
				readStream>>savedWorkDataType;
				switch(savedWorkDataType)
				{
					case QUINT8:
					{
						Q_UINT8 dataRead;
						readStream>>dataRead;
						return true;
					}
					case QUINT16:
					{
						Q_UINT16 dataRead;
						readStream>>dataRead;
						return true;
					}
					case QUINT32:
					{
						Q_UINT32 dataRead;
						readStream>>dataRead;
						return true;
					}
					case QUINT64:
					{
						Q_UINT64 dataRead;
						readStream>>dataRead;
						return true;
					}
					case DOUBLE:
					{
						double dataRead;
						readStream>>dataRead;
						return true;
					}
					case CHARSTRING:
					{
						char* dataRead;
						readStream>>dataRead;
						return true;
					}
					case QSTRING:
					{
						QString dataRead;
						readStream>>dataRead;
						return true;
					}
					case QPOINT:
					{
						QPoint dataRead;
						readStream>>dataRead;
						return true;
					}
					case QRECT:
					{
						QRect dataRead;
						readStream>>dataRead;
						return true;
					}
					default:;
				}
				return false;
			};

			QDataStream readFileStream( &readFile );

			Q_UINT16 indexReadData;
			Q_UINT8 typeReadData;

			//Prosessorien määrä
			Q_UINT16 numProcessors;
			readFileStream>>indexReadData>>typeReadData>>numProcessors;

			setProgressBar(100);

			for(int i=0;i<int(numProcessors);i++)
			{
				Q_UINT16 indexProcessor;
				Q_UINT16 typeProcessor;
				QString nameProcessor;

				bool done=false;
				do
				{
					readFileStream>>indexReadData;
					switch(indexReadData)
					{
						case 0:
							readFileStream>>typeReadData>>indexProcessor;
							break;
						case 1:
							readFileStream>>typeReadData>>typeProcessor;
							break;
						case 2:
							readFileStream>>typeReadData>>nameProcessor;
							break;
						case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
							done=true;
							break;
						default:
							done = !funcHandleUnknownIndexRead(readFileStream);
					}
				}
				while(!done);

				processorWindowLib_->addProcessor(indexProcessor, typeProcessor, nameProcessor);

				I51BaseW* processorW=(I51BaseW*)windowWorkspace_->activeWindow();
				processorW->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead); //Luetaan Data ja ulkoiset komponentit
			}

			//Tähän tämän luokan talletettujen tietojen luku.
			Q_UINT32 value32;
			Q_UINT16 value16;
			Q_UINT8 value8;
			bool done=false;
			do
			{
				readFileStream>>indexReadData;
				switch(indexReadData)
				{
					case 0:
						readFileStream>>typeReadData>>value32;
						if(openWorkFirstTimeAfterClear_) //Muutetaan arvoja vain jos luetaan ekakertaa. Appendissa ei
							runTimerInterval_=(int)value32;
						break;
					case 1:
						readFileStream>>typeReadData>>value32;
						if(openWorkFirstTimeAfterClear_) //Muutetaan arvoja vain jos luetaan ekakertaa. Appendissa ei
							serialNetCounter_=(unsigned int)value32;
						break;
					case 2: //Ikkunan tila
						readFileStream>>typeReadData>>value16;
						setWindowState(value16);
						break;
					case 3:
						readFileStream>>typeReadData>>value8;
						toolsDebug_->setButton(toolsDebug_StopAllAtBreakpoint_ID_,(bool)value8);
						break;
					case 4:
						readFileStream>>typeReadData>>value8;
						toolsView_->setButton(toolsView_ShowBreakpoint_ID_,(bool)value8);
						break;
					case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
						done=true;
						break;
					default:
						done = !funcHandleUnknownIndexRead(readFileStream);
				}
			}
			while(!done);

			readFile.close();

			workSavedFirstTime_=true;

			//Overall progresbaarin nollaus
			resetProgressBar();
			statusBarLabel_->setText(i18n("Work opened"));

			openWorkFirstTimeAfterClear_=false;
			setCaption(workFilename_);
			enableDisableMenusToolbaars(false);
		}
	}
}

void KSimu51::loadPrevWork(void)
{
	readCFG();
	openWorkDo();
}

void KSimu51::openWork(void)
{
	QString pathTo=workPath_;
	openWorkFilename_=KFileDialog::getOpenFileName(pathTo, "*.work51", this, i18n("Open work51 [.work51]  "));
	if(openWorkFilename_!=QString::null)
		openWorkDo();
}


void KSimu51::closeWindow(void)
{
	I51BaseW* processorW = (I51BaseW*)windowWorkspace_->activeWindow();
	if(processorW)
		processorW->close();
}

void KSimu51::closeAllWindows(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	stopAll();
	if(windows.count())
	{
		for (int i = 0; i < int(windows.count()); ++i)
		{
			QWidget *window = windows.at(i);
			if (!window->close())
				return;
		}
		statusBarLabel_->setText(i18n("Work closed"));
	}
	openWorkFirstTimeAfterClear_=true;
	workSavedFirstTime_=false;
	setCaption(workFilename_);
}

//Katellaan miten ikkunat on auki ja laitetaan toolbaareja ja menuja sen mukaan.
//close on True jos suljetaan ikkunoita.
void KSimu51::enableDisableMenusToolbaars(bool const  close)
{
	QWidgetList processorList = windowWorkspace_->windowList();
	//Katsotaan onko ohjelmia ladattu
	int loadedCounter=0;
	int processorListCounter=readWindowsCounterValue();
	if(processorListCounter>0)
	{
		menuBar()->setItemEnabled(menuWindows_ID_, true);
		//Tutkitaan ladatut ohjelmat
		for(int i=0;i<processorList.count();++i)
		{
			I51BaseW *targetProcessorW = (I51BaseW*)processorList.at( i );
			if(targetProcessorW->isReadyToRun())
				loadedCounter++;
		}
		I51BaseW* activeProcessor = (I51BaseW*)windowWorkspace_->activeWindow();
		if(close&&activeProcessor->isReadyToRun())
			loadedCounter--;
		if(loadedCounter<1)
		{
			menuBar()->setItemEnabled(menuDebug_ID_, false);
			menuBar()->setItemEnabled(menuView_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_PowerOnResetAll_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_RunAll_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_StopAll_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_StepAll_ID_, false);
			menuView_->setItemEnabled(menuView_UpdateAll_ID_, false);

			toolsDebug_->setEnabled(false);
			toolsDebug_->setItemEnabled(toolsDebug_PowerOnResetAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_RunAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_StopAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_StepAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_StopAllAtBreakpoint_ID_, false);
			toolsView_->setEnabled(false);
			toolsView_->setItemEnabled(toolsView_UpdateAll_ID_, false);
		}
		if(loadedCounter==1)
		{
			menuBar()->setItemEnabled(menuDebug_ID_, true);
			menuBar()->setItemEnabled(menuView_ID_, true);
			menuDebug_->setItemEnabled(menuDebug_PowerOnResetAll_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_RunAll_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_StopAll_ID_, false);
			menuDebug_->setItemEnabled(menuDebug_StepAll_ID_, false);
			menuView_->setItemEnabled(menuView_UpdateAll_ID_, false);

			toolsDebug_->setEnabled(true);
			toolsDebug_->setItemEnabled(toolsDebug_PowerOnResetAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_RunAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_StopAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_StepAll_ID_, false);
			toolsDebug_->setItemEnabled(toolsDebug_StopAllAtBreakpoint_ID_, false);
			toolsView_->setEnabled(true);
			toolsView_->setItemEnabled(toolsView_UpdateAll_ID_, false);
		}
		if(loadedCounter>1)
		{
			menuBar()->setItemEnabled(menuDebug_ID_, true);
			menuBar()->setItemEnabled(menuView_ID_, true);
			menuDebug_->setItemEnabled(menuDebug_PowerOnResetAll_ID_, true);
			menuDebug_->setItemEnabled(menuDebug_RunAll_ID_, true);
			menuDebug_->setItemEnabled(menuDebug_StopAll_ID_, true);
			menuDebug_->setItemEnabled(menuDebug_StepAll_ID_, true);
			menuView_->setItemEnabled(menuView_UpdateAll_ID_, true);

			toolsDebug_->setEnabled(true);
			toolsDebug_->setItemEnabled(toolsDebug_PowerOnResetAll_ID_, true);
			toolsDebug_->setItemEnabled(toolsDebug_RunAll_ID_, true);
			toolsDebug_->setItemEnabled(toolsDebug_StopAll_ID_, true);
			toolsDebug_->setItemEnabled(toolsDebug_StepAll_ID_, true);
			toolsDebug_->setItemEnabled(toolsDebug_StopAllAtBreakpoint_ID_, true);
			toolsView_->setEnabled(true);
			toolsView_->setItemEnabled(toolsView_UpdateAll_ID_, true);
		}

		if(processorListCounter==1)
		{
			menuFile_->setItemEnabled(menuFile_SaveWork_ID_, true);
			menuFile_->setItemEnabled(menuFile_SaveWorkAs_ID_, true);
			menuFile_->setItemEnabled(menuFile_Close_ID_, true);
			menuBar()->setItemEnabled(menuEdit_ID_, true);
			menuProcessor_->setItemEnabled(menuProcessor_LoadHexFile_ID_, true);
			menuProcessor_->setItemEnabled(menuProcessor_LoadHexFileToAll_ID_, false);

			toolsFile_->setItemEnabled(toolsFile_Save_ID_, true);
			toolsFile_->setItemEnabled(toolsFile_LoadProgram_ID_, true);
			toolsFile_->setItemEnabled(toolsFile_LoadProgramToAll_ID_, false);
		}
		if(processorListCounter>1)
		{
			menuProcessor_->setItemEnabled(menuProcessor_LoadHexFileToAll_ID_, true);

			toolsFile_->setItemEnabled(toolsFile_LoadProgramToAll_ID_, true);
		}
	}
	else
	{
		menuBar()->setItemEnabled(menuEdit_ID_, false);
		menuBar()->setItemEnabled(menuView_ID_, false);
		menuBar()->setItemEnabled(menuDebug_ID_, false);
		menuBar()->setItemEnabled(menuWindows_ID_, false);
		menuFile_->setItemEnabled(menuFile_SaveWork_ID_, false);
		menuFile_->setItemEnabled(menuFile_SaveWorkAs_ID_, false);
		menuFile_->setItemEnabled(menuFile_Close_ID_, false);
		menuView_->setItemEnabled(menuView_UpdateAll_ID_, false);
		menuDebug_->setItemEnabled(menuDebug_PowerOnResetAll_ID_, false);
		menuDebug_->setItemEnabled(menuDebug_RunAll_ID_, false);
		menuDebug_->setItemEnabled(menuDebug_StopAll_ID_, false);
		menuDebug_->setItemEnabled(menuDebug_StepAll_ID_, false);
		menuProcessor_->setItemEnabled(menuProcessor_LoadHexFile_ID_, false);
		menuProcessor_->setItemEnabled(menuProcessor_LoadHexFileToAll_ID_, false);

		toolsFile_->setItemEnabled(toolsFile_Save_ID_, false);
		toolsFile_->setItemEnabled(toolsFile_LoadProgram_ID_, false);
		toolsFile_->setItemEnabled(toolsFile_LoadProgramToAll_ID_, false);
		toolsDebug_->setEnabled(false);
		toolsDebug_->setItemEnabled(toolsDebug_PowerOnResetAll_ID_, false);
		toolsDebug_->setItemEnabled(toolsDebug_RunAll_ID_, false);
		toolsDebug_->setItemEnabled(toolsDebug_StopAll_ID_, false);
		toolsDebug_->setItemEnabled(toolsDebug_StepAll_ID_, false);
		toolsDebug_->setItemEnabled(toolsDebug_StopAllAtBreakpoint_ID_, false);
		toolsView_->setEnabled(false);
		toolsView_->setItemEnabled(toolsView_UpdateAll_ID_, false);

		openWorkFirstTimeAfterClear_=true;
		workSavedFirstTime_=false;
		setCaption(workFilename_);
	}
}

void KSimu51::countProgressBar(const int value)
{
	overallProgressBarCounter_+=value;
	if(overallProgressBarCounter_>=overallStatusBarProgressBar_->totalSteps())
		overallStatusBarProgressBar_->setProgress(overallStatusBarProgressBar_->totalSteps());
	else
		overallStatusBarProgressBar_->setProgress(overallProgressBarCounter_);
}

void KSimu51::setProgressBar(const int value)
{
	overallStatusBarProgressBar_->setEnabled(true);
	overallProgressBarCounter_=0;
	overallStatusBarProgressBar_->setTotalSteps(value);
}

void KSimu51::resetProgressBar(void)
{
	overallStatusBarProgressBar_->setProgress(0);
	overallProgressBarCounter_=0;
	overallStatusBarProgressBar_->setEnabled(false);
}

void KSimu51::messageFromProcessorW_SetStatusBar(const int processorIndex, const QString& text, const unsigned int time)
{
	statusBarLabel_->setText(text);
}

void KSimu51::messageFromProcessorW_Close(int const  processorIndex)
{
	countDownWindowsCounter();
	enableDisableMenusToolbaars(true);
}

void KSimu51::messageFromProcessorW_SetWindowLevel(const int processorIndex, const int level)
{
	activeWindowLevel_=level;
	setActiveWindowColorInActiveWorkspaceLevel();
}

void KSimu51::prosessorActivated(void)
{
	messageFromProcessorW_SetWindowLevel(0, KSimu51_NameS::WINDOWLEVEL_MAIN);
}

//Tuodaaan mousebuttoni x11 eventti signaalina ja tutktaan osuiko painallus workspacen ikkunaan.
//Tarvitaan aktiivisen ikkunanlevelin määritykseen tapauksessa jossa ikkuna on jo aktiivinen ja klikataan ikkunaa hiirellä tilebaarista.
//QT ei lähetä mitään eventtiä eikä signaalia tästä tapauksesta.
void KSimu51::x11Signal_LeftMouseButtonPressed(QPoint* p)
{
	QWidgetList processorList = windowWorkspace_->windowList();
	if(processorList.count())
	{
		//Ekaksi katototaan Ksimun workspace
		activateWorkspaceWindowHittingTileBarFromActiveWindow(p);   //Workspace laittaa signaalin ikunan aktivoitumisesta

		//Sitten käydään katsomassa prosessorin ikkunat ja prosessorin externaali
		for(int i=0; i<int(processorList.count()); ++i)
		{
			I51BaseW *processorW = (I51BaseW*)processorList.at(i);
			if(processorW->activateWindowFromGlobalPointValue(p))
				break;
		}
	}
}

void KSimu51::resizeEvent( QResizeEvent *e )
{
	QSize oldSize=e->oldSize();
	QSize newSize=e->size();
	if((oldSize.height()) != (newSize.height()))
	{
		serialNetHistoryListBox_->setMaximumHeight(serialTools_->height()/2);
		serialNetEditListBox_->setMinimumHeight(height()/4);
	}
}

void KSimu51::closeEvent( QCloseEvent *e )
{
	closeAllWindows();
	KMainWindow::closeEvent(e);
}

bool KSimu51::event(QEvent *e)
{
	return KMainWindow::event(e);   //Potkastaan ohi shareMainWindowin
}


void KSimu51::mousePressEvent(QMouseEvent *e)
{
	QPoint mousePosition=e->globalPos();
	if((e->button() == QMouseEvent::RightButton))
	{
		workspaceMenu_->popup(mousePosition);
	}
	WorkspaceMainW::mousePressEvent(e);
}



#include "ksimu51.moc"
