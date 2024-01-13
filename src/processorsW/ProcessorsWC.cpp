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
#include "ProcessorsWC.h"

#include <qwidget.h>
#include <kpopupmenu.h>


ProcessorsWC::ProcessorsWC()
{
	iconKPixmap_.load("pics/KS51hi32Memory.png", 0, KPixmap::Auto );
}

ProcessorsWC::~ProcessorsWC()
{
}

I51BaseW* ProcessorsWC::addProcessorW(QWidget* workspaceToAdd, int const  index, int const  type)
{
	QString indexQStr;
	unsigned int processorIndexCounter=1;
	unsigned int processorMaxIndexCount=0;
	//Etsitään ensimmäinen vapaa indeksi ja max indeksinumero
	QWorkspace* tad=(QWorkspace*)workspaceToAdd;
	QWidgetList processorList = tad->windowList();
	bool reservedIndexTable[256]={false};
	if(processorList.count())
	{
		//Annetaan ensimmäinen vapaa indeksi.
		int i;
		for (i = 0; i < int(processorList.count()); ++i )
		{
			I51BaseW *m = (I51BaseW*)processorList.at( i );
			int apuIndex=m->readProcessorIndex();
			reservedIndexTable[apuIndex]=true;
			if(processorMaxIndexCount<apuIndex)
				processorMaxIndexCount=apuIndex;
		}
		for(i=255;i>=1;i--)
			if(!reservedIndexTable[i])
				processorIndexCounter=i;
	}
	if(index)   //Jos opendoidaan Work
	{
		if(!reservedIndexTable[index]) //Jos tuotu indeksi vapaa
			processorIndexCounter=index; //prosessorin indeksiksi tuotu indeksi.
		else
			processorIndexCounter=processorMaxIndexCount+1;
	}

	if(processorIndexCounter>=UCHAR_MAX)
	{
	//   statusBarLabel->setText(i18n("Processor index out of range"));
		return 0;
	}

	I51BaseW* pointerToProcessorW=addProcessorW_VirtualPart(workspaceToAdd, processorIndexCounter, type);

	if(pointerToProcessorW)
	{
		string captionIndexText=pointerToProcessorW->readProcessorName();
		captionIndexText+="<0x";
		captionIndexText+=hexToString(processorIndexCounter,2);
		captionIndexText+=">";
		pointerToProcessorW->setCaptionIndexText(captionIndexText);

		pointerToProcessorW->show();
		pointerToProcessorW->createExternalAreaWindow();

		return pointerToProcessorW;
	}
	return 0;
}

