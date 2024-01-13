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
#ifndef WORKSPACEMAINW_H
#define WORKSPACEMAINW_H

#include "ShareMainW.h"

#include <kcursor.h>
#include <kpopupmenu.h>
#include <qworkspace.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

class KMainWindow;
class QWorkspace;
class KPopupMenu;

class MyWorkspace : public QWorkspace
{
	public:
		MyWorkspace(QWidget * parent) : QWorkspace (parent){;};
		~MyWorkspace(){;};

	protected:

		void enterEvent(QEvent *e);
		void leaveEvent(QEvent *e);
		void mousePressEvent(QMouseEvent *e);
};

class WorkspaceMainW : public ShareMainW
{
	Q_OBJECT
	public:
		WorkspaceMainW(QWidget* parent, char const * name, int wflags);
		virtual ~WorkspaceMainW ();

		int readWindowsCounterValue(void);//luetaan oma windows counteri
		void markActiveWindow(void);
		void unmarkWindows(void);
		
		bool activateWorkspaceWindowHittingTileBarFromActiveWindow(QPoint* p);

		QWidgetList readWindowList(void){return windowWorkspace_->windowList();};

	protected:
		MyWorkspace* windowWorkspace_;
		void mousePressEvent(QMouseEvent *e) override;
		void mouseMoveEvent(QMouseEvent *e) override;
		void keyPressEvent(QKeyEvent *e) override;
		void showEvent(QShowEvent * e) override;

		//Oman ikkunacounterin counttaus ylös/alas
		void countUpWindowsCounter(void);
		void countDownWindowsCounter(void);

		KPopupMenu* windowsMenu_;
		//Ikkunakikkailua
		//Scrollauksessa tarvittavaa

		int horizontalTileDivider_;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

signals:
		void workspaceScrolled(void);
		void workspaceWindowActivated(void);      //Ihan mikä tahansa workspacen ikkuna aktivoitunut.

	public slots:
		void centerActiveWindow(void);
		void cornerActiveWindow(void);
		void backPreviousWindowState(void);
		void activeWindowNearToWindow(void);
		void activeWindowResizeHiddenOff(void);
		void nextWindowToShow(void);
		virtual void tileWindowsHorizontal(void);
		virtual void tileWindows(void);
		virtual void cascadeWindows(void);

	private slots:
		void childWindowActivated(QWidget*);

		void windowsMenuAboutToShow(void);
		void windowActivatedFromMenu(int id);

	private:

		//Movessa tarvittavaa
		bool movingNearTo_;
		int moveSide_;
		enum moveSides
		{
			SIDE_UP=1,
			SIDE_DOWN,
			SIDE_LEFT,
			SIDE_RIGHT
		};
		int movingWindowIndex_;
		QPoint movingWindowStartPosition_;
		int windowsCounter_;//Toimii nopeammin kuin windowList.count().Tarvitaan hideShowMenusToolbarssissa.
		int nextWindowToShowCount_;
		QValueList<QPoint> *scrollBackList_;
		void addScrollBackList(QPoint pos);
		static int const MAX_SCROLLS_BACK=20;
};

#endif
