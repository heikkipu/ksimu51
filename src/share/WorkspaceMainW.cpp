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
#include "WorkspaceMainW.h"
#include <qscrollview.h>
#include <qscrollbar.h>
#include <klocale.h>
#include "../ksimu51.h"



WorkspaceMainW::WorkspaceMainW( QWidget* parent, char const * name, int wflags) : ShareMainW( parent, name, wflags)
{
	setWFlags(KMainWindow::WStyle_MinMax);
	//Työskentelyalue
	QVBox* vb = new QVBox(this);
	vb->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	windowWorkspace_ = new MyWorkspace(vb);
	windowWorkspace_->setScrollBarsEnabled(true);
	//windowWorkspace_->setPaletteBackgroundColor(Qt::lightGray);
	setCentralWidget(vb);
	vb->setMinimumSize(QSize(70,70));
	horizontalTileDivider_=1;
	windowsCounter_=0;
	nextWindowToShowCount_=0;
	movingNearTo_=false;
	moveSide_=0;
	connect(windowWorkspace_, SIGNAL(windowActivated(QWidget*)), this, SLOT(childWindowActivated(QWidget*)));

	windowsMenu_ = new KPopupMenu(this);
	windowsMenu_->setCheckable( TRUE );
	connect(windowsMenu_, SIGNAL(aboutToShow()), this, SLOT(windowsMenuAboutToShow()));
	scrollBackList_=new QValueList<QPoint>();
	
}

WorkspaceMainW::~WorkspaceMainW()
{
}

void WorkspaceMainW::countUpWindowsCounter(void)
{
	windowsCounter_++;
}
	
void WorkspaceMainW::countDownWindowsCounter(void)
{
	windowsCounter_--;
	if(windowsCounter_<0)
		windowsCounter_=0;
}

int WorkspaceMainW::readWindowsCounterValue(void)
{
	return windowsCounter_;
}

void WorkspaceMainW::centerActiveWindow(void)
{
	windowWorkspace_->setScrollBarsEnabled(false);
	KMainWindow* activeW=(KMainWindow*)windowWorkspace_->activeWindow();
	QPoint p=activeW->parentWidget()->pos();
	QRect fg=activeW->parentWidget()->frameGeometry();
	QRect g=activeW->geometry();
	QSize vs=windowWorkspace_->frameSize();
	int scroll_dx=(vs.width()/2-4)-(p.x()+fg.width()/2)-(fg.width()-g.width());
	int scroll_dy=(vs.height()/2+4)-(p.y()+g.height()/2)-(fg.height()-g.height());
	windowWorkspace_->scroll(scroll_dx, scroll_dy);
	addScrollBackList(QPoint(scroll_dx,scroll_dy));
	windowWorkspace_->setScrollBarsEnabled(true);
	emit workspaceScrolled();
}

void WorkspaceMainW::cornerActiveWindow(void)
{
	windowWorkspace_->setScrollBarsEnabled(false);
	KMainWindow* activeW=(KMainWindow*)windowWorkspace_->activeWindow();
	QPoint p=activeW->parentWidget()->pos();
	int scroll_dx=-p.x();
	int scroll_dy=-p.y();
	windowWorkspace_->scroll(scroll_dx, scroll_dy);
	addScrollBackList(QPoint(scroll_dx,scroll_dy));
	windowWorkspace_->setScrollBarsEnabled(true);
	emit workspaceScrolled();
}

void WorkspaceMainW::nextWindowToShow(void)
{
//	windowWorkspace_->activateNextWindow();
	QWidgetList w = windowWorkspace_->windowList();
	if(w.count())
	{
		nextWindowToShowCount_++;
		if(nextWindowToShowCount_>=w.count())
			nextWindowToShowCount_=0;
		ShareMainW *shWs = (ShareMainW*)w.at(nextWindowToShowCount_);
		if(shWs)
		{
			shWs->setFocus();
			centerActiveWindow();
		}
	}
}

void WorkspaceMainW::backPreviousWindowState(void)
{
	if(!scrollBackList_->empty())
	{
		windowWorkspace_->setScrollBarsEnabled(false);
		QPoint scrollP=scrollBackList_->last();
		windowWorkspace_->scroll(-scrollP.x(), -scrollP.y());
		scrollBackList_->pop_back();
		windowWorkspace_->setScrollBarsEnabled(true);
	}
	emit workspaceScrolled();
}

void WorkspaceMainW::addScrollBackList(QPoint pos)
{
	scrollBackList_->push_back(pos);
	if(scrollBackList_->count()>=MAX_SCROLLS_BACK)
		scrollBackList_->pop_front();
}

void WorkspaceMainW::activeWindowNearToWindow(void)
{
	if((windowWorkspace_->windowList().count())&&!movingNearTo_)
	{
		KMainWindow* activeW=(KMainWindow*)windowWorkspace_->activeWindow();
		movingWindowStartPosition_=activeW->parentWidget()->pos();
		movingNearTo_=true;
		QWidgetList wList=windowWorkspace_->windowList();
		for (int i=0;i<int(wList.count());++i)
		{
			KMainWindow* countW = (KMainWindow*)wList.at(i);
			if(countW==activeW)
			{
				movingWindowIndex_=i;
				break;
			}
		}
		setMouseTracking(true);
		grabMouse();
	}
}

void WorkspaceMainW::activeWindowResizeHiddenOff(void)
{
	KMainWindow* activeW=(KMainWindow*)windowWorkspace_->activeWindow();
	if(activeW)
	{
		QRect activeWR=activeW->parentWidget()->geometry();
		QScrollView* workSV=(QScrollView*)windowWorkspace_->parentWidget();
		QSize workSS=workSV->viewport()->size();
		int x=activeWR.x();
		int y=activeWR.y();
		int w=activeWR.width();
		int h=activeWR.height();
		if(activeWR.x()<0)
		{
			x=0;
			w+=activeWR.x();
		}
		if(activeWR.y()<0)
		{
			y=0;
			h+=activeWR.y();
		}
		//18 scroll baarin koko, pointterin sieppaus ei onnannu piti käyttää vakiota
		if(x+activeWR.width()>workSS.width()-18)
		{
			w-=x+activeWR.width()-workSS.width()+18;
		}
		if(y+activeWR.height()>workSS.height()-18)
		{
			h-=y+activeWR.height()-workSS.height()+18;
		}
		activeW->parentWidget()->setGeometry(x, y, w, h);
	}
}

void WorkspaceMainW::markActiveWindow(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if(windows.count()<2)
		return;
	unmarkWindows();
	ShareMainW* activeW=(ShareMainW*)windowWorkspace_->activeWindow();
	if(activeW)
		activeW->setActiveStyle();
}

void WorkspaceMainW::unmarkWindows(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if(windows.count())
	{
		for(int i=0;i<int(windows.count());++i)
		{
			ShareMainW *umWs = (ShareMainW*)windows.at(i);
			if(umWs)
				umWs->setNormalStyle();
		}
	}
}

void WorkspaceMainW::childWindowActivated(QWidget * w)
{
	QWidgetList wList=windowWorkspace_->windowList();
	if(wList.count()&&w)
	{
		QRect fg=w->parentWidget()->frameGeometry();
		if(movingNearTo_)
		{
			movingNearTo_=false;
			KMainWindow* mW = (KMainWindow*)wList.at(movingWindowIndex_);
			QRect cWFG=mW->parentWidget()->frameGeometry();
			switch(moveSide_)
			{
				case SIDE_UP:
					mW->parentWidget()->move(fg.x(),fg.y()-cWFG.height());
					break;
				case SIDE_DOWN:
					mW->parentWidget()->move(fg.x(),fg.y()+fg.height());
					break;
				case SIDE_LEFT:
					mW->parentWidget()->move(fg.x()-cWFG.width(),fg.y());
					break;
				case SIDE_RIGHT:
					mW->parentWidget()->move(fg.x()+fg.width(),fg.y());
					break;
			}
			//Aktivoidaan siirretty ikkuna
			mW->setFocus();
		}
	}
	releaseMouse();
	setMouseTracking(false);

	emit workspaceWindowActivated();
}

void WorkspaceMainW::mousePressEvent(QMouseEvent *e)
{
	releaseMouse();
	QPoint mousePosition=e->pos();
	QWidgetList windows = windowWorkspace_->windowList(QWorkspace::StackingOrder);
	bool hits=false;
	if(windows.count())
	{
		for (int i=int(windows.count())-1;i>=0;i--)
		{
			KMainWindow *m = (KMainWindow*)windows.at(i);
			QPoint parentPos=m->parentWidget()->parentWidget()->parentWidget()->pos();
			QRect fg=m->parentWidget()->frameGeometry();
			int clicked_x=mousePosition.x()-parentPos.x();
			int clicked_y=mousePosition.y()-parentPos.y();
			if((clicked_x>fg.x()) && (clicked_x<fg.x()+fg.width()) && (clicked_y>fg.y()) && (clicked_y<fg.y()+fg.height()) && !m->isMinimized())
			{
				moveSide_=SIDE_RIGHT;
				int nearestPoint=fg.x()+fg.width()-clicked_x;
				int cP=fg.y()+fg.height()-clicked_y;
				if(nearestPoint>cP)
				{
					moveSide_=SIDE_DOWN;
					nearestPoint=cP;
				}
				cP=clicked_x-fg.x();
				if(nearestPoint>cP)
				{
					moveSide_=SIDE_LEFT;
					nearestPoint=cP;
				}
				cP=clicked_y-fg.y();
				if(nearestPoint>cP)
					moveSide_=SIDE_UP;
				m->setFocus();
				hits=true;
				break;
			}
		}
	}
	if(!hits&&movingNearTo_)
		movingNearTo_=false;
}

void WorkspaceMainW::mouseMoveEvent(QMouseEvent * e)
{
	if( movingNearTo_)
	{
		KMainWindow* w=(KMainWindow*)windowWorkspace_->activeWindow();
		if(w)
		{
			QPoint pPos=e->pos();
			QPoint wwwPos=w->parentWidget()->parentWidget()->parentWidget()->pos();
			w->parentWidget()->move(pPos-wwwPos);
		}
	}
}

//Katsotaan onko klobaali piste jonkun aktiivin workspaceikkunan tileBaarin alueella.
//Laitetaan signaali ikkunan aktivoimisesta activeWindowLevelin päivityksestä.
bool WorkspaceMainW::activateWorkspaceWindowHittingTileBarFromActiveWindow(QPoint* p)
{
	//Katsotaan onko klikkaus workspacen sisällä
	QRegion r=windowWorkspace_->clipRegion();
	QRect rc=r.boundingRect();
	QPoint pfg=mapFromGlobal(*p);
	QPoint atParentPos=windowWorkspace_->parentWidget()->pos();
	int vx=atParentPos.x()+rc.x();
	int vy=atParentPos.y()+rc.y();
	int vw=atParentPos.x()+rc.x()+rc.width()-18; //18=scroll barin koko
	int vh=atParentPos.y()+rc.y()+rc.height()-18;
	if( (pfg.x()>vx) && (pfg.x()<vw) && (pfg.y()>vy) && (pfg.y()<vh) )
	{
		QWidgetList windows = windowWorkspace_->windowList(QWorkspace::StackingOrder);
		if(windows.count())
		{
			KMainWindow* m=(KMainWindow*)windowWorkspace_->activeWindow();
			if(!m->isMinimized())
			{
				QRect fg=m->parentWidget()->frameGeometry();
				QRect g=m->geometry();
				vx=atParentPos.x()+fg.x();
				vy=atParentPos.y()+fg.y();
				vw=atParentPos.x()+fg.x()+fg.width();
				vh=atParentPos.y()+fg.y()+g.y();   //Otetaan vain tilebaarin korkeus
				if( (pfg.x()>vx) && (pfg.x()<vw) && (pfg.y()>vy) && (pfg.y()<vh))
				{
					emit workspaceWindowActivated();
					return true;
				}
			}
		}
	}
	return false;
}

void WorkspaceMainW::keyPressEvent(QKeyEvent *e)
{
	if(e->key()==Qt::Key_Escape)
	{
		if(movingNearTo_)
		{
			releaseMouse();
			movingNearTo_=false;
			KMainWindow* w=(KMainWindow*)windowWorkspace_->activeWindow();
			if(w)
				w->parentWidget()->move(movingWindowStartPosition_);
		}
	}
	KMainWindow::keyPressEvent(e);
}

void WorkspaceMainW::windowsMenuAboutToShow()
{
	QWidgetList windows = windowWorkspace_->windowList();
	windowsMenu_->clear();
	if(windows.count())
	{
		windowsMenu_->insertItem(i18n("&Cascade"), this, SLOT(cascadeWindows()));
		windowsMenu_->insertItem(i18n("&Tile"), this, SLOT(tileWindows() ) );
		windowsMenu_->insertItem(i18n("Tile &Horizontally"), this, SLOT(tileWindowsHorizontal()));
		windowsMenu_->insertSeparator();
		if(!windowWorkspace_->activeWindow()->isMaximized())
		{
			windowsMenu_->insertItem(i18n("C&enter"), this, SLOT(centerActiveWindow()));
			windowsMenu_->insertItem(i18n("C&orner"), this, SLOT(cornerActiveWindow()));
			windowsMenu_->insertItem(i18n("Ne&xt"), this, SLOT(nextWindowToShow()));
			if(!scrollBackList_->empty())
				windowsMenu_->insertItem(i18n("&Back"), this, SLOT(backPreviousWindowState()));
			windowsMenu_->insertSeparator();

			if(windows.count()>1)
			{
				windowsMenu_->insertItem(i18n("&Near To.."), this, SLOT(activeWindowNearToWindow()));
			}
			windowsMenu_->insertItem(i18n("Hidden &Away"), this, SLOT(activeWindowResizeHiddenOff()));
			windowsMenu_->insertSeparator();
		}
		for ( int i = 0; i < int(windows.count()); ++i )
		{
			QString cap=windows.at(i)->caption();
			int br=cap.find('-', 0, TRUE);
			if(br>0)
				cap.truncate(br);
			int id = windowsMenu_->insertItem(cap, this, SLOT(windowActivatedFromMenu(int)));
			windowsMenu_->setItemParameter(id, i);
			windowsMenu_->setItemChecked(id, windowWorkspace_->activeWindow() == windows.at(i));
		}
	}
}

void WorkspaceMainW::windowActivatedFromMenu( int id )
{
	QWidget* w = windowWorkspace_->windowList().at( id );
	if(w)
	{
		w->showNormal();
		w->setFocus();
		centerActiveWindow();
		emit workspaceWindowActivated();
	}
}

void WorkspaceMainW::tileWindows()
{
	windowWorkspace_->tile();
	emit workspaceWindowActivated();
}

void WorkspaceMainW::tileWindowsHorizontal(void)
{
	QWidgetList windows = windowWorkspace_->windowList();
	if ( !windows.count() )
		return;

	if(horizontalTileDivider_<1)
		horizontalTileDivider_=1;
	if(horizontalTileDivider_>5)
		horizontalTileDivider_=5;
	int heightForEach=windowWorkspace_->height()/horizontalTileDivider_;
	int y=0;
	for ( int i = 0; i < int(windows.count()); ++i )
	{
		QWidget *window=windows.at(i);
		if(window->testWState(WState_Maximized))
		{
			// prevent flicker
			//window->hide(); aiheuttaa crashin
			window->showNormal();
		}
		int maxH=window->parentWidget()->maximumHeight();
		int actHeight = QMIN(heightForEach, maxH)-15;
		int mWidth=window->parentWidget()->maximumWidth();
		int wsWidth=windowWorkspace_->width()-15;

		window->parentWidget()->setGeometry( 0, y, QMIN(mWidth,wsWidth), actHeight);
		y+=actHeight;
//      tileWindow();
	}
	emit workspaceWindowActivated();
}

void WorkspaceMainW::cascadeWindows(void)
{
	windowWorkspace_->cascade();
}

void WorkspaceMainW::showEvent(QShowEvent * e)
{
	KMainWindow::showEvent(e);    //Ohitetaan ShareMainW
}


void WorkspaceMainW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void WorkspaceMainW::readWorkDataFromFile(QDataStream& readFileStream,
									   std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			//case X:
			//	readFileStream>>typeReadData>>Data_xxx_;
			//	break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}


void MyWorkspace::enterEvent(QEvent *e)
{
	//releaseMouse();
	//grabMouse();
	QWorkspace::enterEvent(e);
}

void MyWorkspace::leaveEvent(QEvent *e)
{
	//releaseMouse();
	QWorkspace::leaveEvent(e);
}

void MyWorkspace::mousePressEvent(QMouseEvent *e)
{
	//releaseMouse();
	QWorkspace::mousePressEvent(e);

}


