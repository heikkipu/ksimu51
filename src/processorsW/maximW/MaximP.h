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
#ifndef MAXIMP_H
#define MAXIMP_H

//Käyttöluokka:
//Tähän luokkaan kastaan kaikki MAXIMProcessor luokan jäsenet.

#include "../ProcessorsWC.h"
class ProcessorsWC;

#include "MaximMAX7651W.h"
#include "MaximMAX7652W.h"

class MaximMAX7651W;
class MaximMAX7652W;

class MaximProcessors : public ProcessorsWC
{
	public:
		MaximProcessors();
		~MaximProcessors();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define PROCESSORWGROUPNAME_MAXIM "MaximProcessorsW"
		enum maximProcessors
		{
			MAXIM_MAX7651,
			MAXIM_MAX7652,
		};

	protected:


		I51BaseW* addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type) override;

};

#endif
