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
#ifndef PROCESSORSWC_H
#define PROCESSORSWC_H

//Kantaluokka proserssorien menulle ja luontiluokalle.
#include "I51BaseW.h"

#include <kpixmap.h>
#include <kpopupmenu.h>

class ProcessorsWC
{
	public:
		ProcessorsWC();
		~ProcessorsWC();

		virtual void addMenu(QWidget* parent, KPopupMenu* popupMenu){;};
		I51BaseW* addProcessorW(QWidget* workspaceToAdd, int const  index, int const  type);

	protected:

		virtual I51BaseW* addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type){return nullptr;};
		KPixmap iconKPixmap_;

};

#endif
