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
#ifndef PROCESSORSWLIB_H
#define PROCESSORSWLIB_H

#include "I51BaseW.h" //Kantaluokka 8031 ikkunakomponenttiin

//-ModifyToCreateLib-
#include "intelW/IntelP.h"
#include "atmelW/AtmelP.h"
#include "maximW/MaximP.h"
#include "philipsW/PhilipsP.h"
#include "texasInstrumentsW/TexasInstrumentsP.h"

class IntelProcessors;
class AtmelProcessors;
class MaximProcessors;
class PhilipsProcessors;
class TexasInstrumentsProcessors;
//-ModifyToCreateLib- END

#include <qwidget.h>

class ProcessorsWLib : public QWidget
{
	Q_OBJECT
	public:
		ProcessorsWLib(QWidget* popupMenuParent, KPopupMenu* popupMenu, QWidget* workspace);
		~ProcessorsWLib();

		void addProcessor(int const  index, int const  type, const QString& name);

	signals:
		void processorCreated(I51BaseW*);

	private slots:
		//Komponenttien popupit aktivoitu. Joka komponettityypiiryhmälle omansa?!?!?
		//-ModifyToCreateComponentLib-
		void addProcessorFromGroup_Intel(int type);
		void addProcessorFromGroup_Atmel(int type);
		void addProcessorFromGroup_Maxim(int type);
		void addProcessorFromGroup_Philips(int type);
		void addProcessorFromGroup_TexasInstruments(int type);
		//-ModifyToCreateComponentLib- END

	private:
		QWidget* workspaceToAddProcessor_;

		int processorToAddIndex_; //
		I51BaseW* processorWindow_;

		//-ModifyToCreateLib-
		ProcessorsWC* intelP_;
		AtmelProcessors* atmelP_;
		MaximProcessors* maximP_;
		PhilipsProcessors* philipsP_;
		TexasInstrumentsProcessors* texasInstrumentsP_;
		//-ModifyToCreateLib-END

		void sendCreationSignal(void);

};



#endif
