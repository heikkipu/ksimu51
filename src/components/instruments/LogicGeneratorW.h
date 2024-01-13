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
#ifndef LOGICGENERATORW_H
#define LOGICGENERATORW_H

#include "InstrumentsC.h"
#include "../ExternalComponentBaseW.h"

#include <qlayout.h>
#include <knuminput.h>
#include <kpushbutton.h>

class LogicGeneratorW : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		LogicGeneratorW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~LogicGeneratorW(void);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:

		void reset_VirtualPart(void) override;
		void update_VirtualPart(void) override;
		double execute_VirtualPart(double executeTime) override;

	private:
		void setWindowSize(void);

		double randomizeTime(double const scale);
		double setLow(double const startTime);
		double setHigh(double const startTime);
		
		QGridLayout *logicGeneratorQGridLayout_;
		KDoubleNumInput *highTimeKDoubleNumInput_;
		KDoubleNumInput *lowTimeKDoubleNumInput_;
		KDoubleNumInput *highRandomTimeKDoubleNumInput_;
		KDoubleNumInput *lowRandomTimeKDoubleNumInput_;

		bool startLevel_;
		KPushButton* startLevelKPushButton_;
		
		bool isOn_;
		KPushButton* onOffKPushButton_;
		
		KPixmap onOffIcon_;

		IOPin* pToGenOutPin_;
		#define PINNAME_GENOUT "OUT"
		
	private slots:
		void startLevelKPushButtonClicked(void);
		void onOffKPushButtonClicked(void);
};

#endif
