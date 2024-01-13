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
#ifndef EXTERNALRAMW_H
#define EXTERNALRAMW_H

#include "../ExternalComponentBaseW.h"


#include <qtable.h>

class QTable;
class KPopupMenu;



class ExternalRAMW : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		ExternalRAMW( QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType, unsigned int const  sz);
		~ExternalRAMW();

		void writeExternalDataSpaceStartAddress(unsigned int const  address) override;

		void clearExternalDataSpace(void) override;
		void setExternalDataSpaceValue(unsigned int const  address, unsigned char const  value) override;

		void showPointerPosition(unsigned char const  pointer);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void update_VirtualPart(void) override;

	private slots:
		void externalRAMTableVerticalHeaderClicked(int section);
		void externalDataSpaceStartAddressActivated(int sA);
		void externalRAMTableValueChanged(int row, int col);
		void externalRAMTableCurrentChanged(int row, int col);

	private:

		int dptrRow_;
		int dptrColumn_;
		int r0Row_;
		int r0Column_;
		int r1Row_;
		int r1Column_;
		void updateDatapointers(void) override;
		void writeValueToTable(unsigned int address, unsigned char value);
		void changeAddressHeader(unsigned int sA);

		void addPopupMenu(void);
		KPopupMenu* externalDataSpaceStartAddressMenu_;

		QSize externalRAMTableGeometrySize(void);
		QTable* externalRAMTable_;
		QString externalRAMTableCurrentCellText_;
};

#endif
