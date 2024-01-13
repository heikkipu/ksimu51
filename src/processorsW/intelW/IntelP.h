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
#ifndef INTELP_H
#define INTELP_H

//Käyttöluokka:

#include "../ProcessorsWC.h"

#include "Intel8031W.h"
#include "Intel8032W.h"
#include "Intel8051W.h"
#include "Intel8052W.h"
#include "Intel80C31W.h"
#include "Intel80C32W.h"
#include "Intel80C51W.h"
#include "Intel80C52W.h"

class ProcessorsWC;

class IntelProcessors : public ProcessorsWC
{
	public:
		IntelProcessors();
		~IntelProcessors();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define PROCESSORWGROUPNAME_INTEL "IntelProcessorsW"
		enum intelProcessors
		{
			INTEL_8031,
			INTEL_80C31,
			INTEL_8032,
			INTEL_80C32,
			INTEL_8051,
			INTEL_80C51,
			INTEL_8052,
			INTEL_80C52,
		};

	protected:


		I51BaseW* addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type) override;

};

#endif
