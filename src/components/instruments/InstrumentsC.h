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
#ifndef INSTRUMENTSCOMPONENTS_H
#define INSTRUMENTSCOMPONENTS_H

#include "../Components.h"

#include "LogicAnalyzerW.h"
#include "LogicGeneratorW.h"

class Components;

class LogicAnalyzerW;
class LogicGeneratorW;

class InstrumentsComponents : public Components
{
	public:
		InstrumentsComponents();
		~InstrumentsComponents();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define COMPONENTGROUPNAME_INSTRUMENTS "instrumentsComponent"
		enum instrumentsType
		{
			LOGICANALYZER8=1,
			LOGICANALYZER16,
			LOGICANALYZER24,
			LOGICANALYZER32,
			LOGICANALYZER40,
			LOGICGENERATOR,
		};

	protected:
		ExternalComponentBaseW* addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString) override;
};

#endif

