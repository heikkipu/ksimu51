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
#ifndef DISASSEMBLEW_H
#define DISASSEMBLEW_H

#include <klistbox.h>
#include <kactivelabel.h>
#include <kpixmap.h>

#include <string>

#include "../processors/I8031.h"
#include "../share/ShareMainW.h"

#include <qdatastream.h>
#include <qlayout.h>

class QGridLayout;
class KListBox;
class KActiveLabel;
class KPopupMenu;


class DisassembleW: public ShareMainW
{
	Q_OBJECT
	public:
		DisassembleW( QWidget* parent, char const * name, int wflags );
		~DisassembleW();

		void clear(void);
		//Lisätään rivi
		void insertItem(const QString& sitem, unsigned long const prosessorAddress);
		unsigned int count();      //Ilmoittaa rivien määrän
		void update(unsigned long const instructionAddress, unsigned long const lastInstructionAddress, unsigned long const lastStepAddress);

		void focusLine(unsigned long const address);
		
		bool isAtBreakpoint(unsigned long const address);
		void clearExecutionPoint(void);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

		QSize getMaximumSize(void) override;

	protected:

	private slots:
		void disassembleKListBoxRightButtonClicked(QListBoxItem* item, const QPoint& p);
		void toggleBreakpointActivated(void);
			void removeBreakpointsActivated(void);
		void executionPointActivated(void);

	signals:
		void message_SetExecutionPoint(const QString& , const unsigned long );


	private:

		QGridLayout* disassembleFormLayout_;

		KListBox* disassembleKListBox_;
		KActiveLabel* lastInstructionKActiveLabel_;
		void updateActiveLable(const QString& labelText);

		void addPopupMenu(void);
		KPopupMenu* disassembleKListBoxMenu_;

		int pointedItemInDisassembleKListBox_;
		int lastPointedItemInDisassembleKListBox_;
		int executionPointLine_;
		bool disassembleBreakpointTable_[I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE]; //Prosessorin osoiteavaruus
		void clearExecutionPointPrivate(void);
		void setProgramCounterPointer(unsigned long const address);

		QValueList<unsigned long>addressInDisassembleKListBoxLineQVList_;
		unsigned int disassembleKListBoxLineInAddress_[I8031_NameS::EXTERNAL_PROGRAM_MEMORY_SIZE]; //Prosessorin osoiteavaruus
		unsigned long disassembleExecutionPoint_;
		unsigned long lastUpdateExecutionAddress_;
		unsigned long prevProgramCounterPointerAddress_;

		KPixmap breakPointIcon32_;
		KPixmap breakPointIcon16_;
		KPixmap executionPointIcon32_;
		KPixmap executionPointIcon16_;
		KPixmap procramCounterPointerIcon16_;
};


#endif
