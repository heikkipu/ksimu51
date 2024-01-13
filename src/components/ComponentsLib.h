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
#ifndef COMPONENTSLIB_H
#define COMPONENTSLIB_H

//Komponenttien kirjastoluokka.
//Käyttää Components menu ja luontiluokkaa.
//Konstruktorissa annetaan pointteri kpopupmenuun ja workspaceen mihin komponentti luodaan.
//Kun menu valitaan komponentti luodaan ja siitä lähtee signaali, jonka parametrina on pointteri luotuun komponenttii.

#include "ExternalComponentBaseW.h"
//-ModifyToCreateComponentLib-
#include "./memory/MemoryC.h"
#include "./displays/DisplaysC.h"
#include "./keysButtons/KeysButtonsC.h"
#include "./misc/MiscC.h"
#include "./logic74/Logic74C.h"
#include "./logicGALPAL/LogicGALPAL_C.h"
#include "./instruments/InstrumentsC.h"
//-ModifyToCreateComponentLib- END

class ComponentsLib : public QWidget
{
	Q_OBJECT
	public:
		ComponentsLib(QWidget* popupMenuParent, KPopupMenu* popupMenu, QWidget* workspace);
		~ComponentsLib();

		void addComponent(unsigned long const  type, const QString& name);

	signals:
		void componentCreated(ExternalComponentBaseW*);

	private slots:
		//Komponenttien popupit aktivoitu. Joka komponettityypiiryhmälle omansa?!?!?
		//-ModifyToCreateComponentLib-
		void addComponentFromGroup_Memory(int type);
		void addComponentFromGroup_Displays(int type);
		void addComponentFromGroup_KeysButtons(int type);
		void addComponentFromGroup_Logic74(int type);
		void addComponentFromGroup_LogicGALPAL(int type);
		void addComponentFromGroup_Misc(int type);
		void addComponentFromGroup_Instruments(int type);
		//-ModifyToCreateComponentLib- END

	private:
		QWidget* workspaceToAddComponent_;

		ExternalComponentBaseW* edmcw_;
		unsigned int componentIndexCounter_; //Komponenttien indeksilaskuri luotaessa komponentti

		//-ModifyToCreateComponentLib-
		DisplaysComponents* displaysC_;
		KeysButtonsComponents* keysButtonsC_;
		Logic74Components* logic74C_;
		LogicGALPALComponents* logicGALPALC_;
		MemoryComponents* memoryC_;
		MiscComponents* miscC_;
		InstrumentsComponents* instrumentsC_;
		//-ModifyToCreateComponentLib-END

		void sendCreationSignal(void);

};

#endif
