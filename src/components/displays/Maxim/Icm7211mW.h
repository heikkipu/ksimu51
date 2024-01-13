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
#ifndef ICM7211MW_H
#define ICM7211MW_H

#include "../../ExternalComponentBaseW.h"

#include <qlayout.h>
#include <qlcdnumber.h>
#include <qtable.h>
#include <qcheckbox.h>

class KPopupMenu;
class QLCDNumber;



class Icm7211mW : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		Icm7211mW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~Icm7211mW();
		void writeExternalDataSpaceStartAddress(unsigned int const  address) override;

		void clearExternalDataSpace(void) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void updateDatapointers(void) override;
		void update_VirtualPart(void) override;
		double execute_VirtualPart(double executeTime) override;

	private slots:
		void externalDataSpaceStartAddressUpperActivated(int sA);
		void externalDataSpaceStartAddressLowerActivated(int sA);
		void addressTableVerticalHeaderClicked(int section);
		void codeBQCheckBoxStateChanged(int s);

	private:
		static unsigned char const  NUM_DIGITS=4;
		char digitValue_[NUM_DIGITS];

		bool codeBIsOn_;
		bool inputValueChanged_;

		void changeExternalDataSpaceStartAddress(int const  sA, bool const  upper);

		void addPopupMenu(void);
		KPopupMenu* externalDataSpaceStartAddressMenu_;
		KPopupMenu* externalDataSpaceStartAddressUpperMenu_;
		KPopupMenu* externalDataSpaceStartAddressLowerMenu_;

		QGridLayout* displayQGridLayout_;
		QLCDNumber* displayQLCDNumber_;
		QTable* addressTable_;
		QCheckBox* codeBQCheckBox_;
};

#endif
