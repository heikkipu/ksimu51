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
#ifndef LOGICANALYZERW_H
#define LOGICANALYZERW_H

#include "InstrumentsC.h"
#include "../ExternalComponentBaseW.h"
#include "LogicAnalyzerQListBoxItems.h"

#include <list>
#include <limits>

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qslider.h>
#include <qtimer.h>

#include <kpushbutton.h>
#include <klistbox.h>
#include <kled.h>

class WaveViewQListBoxItem;
class TimeViewQListBoxItem;
class ProgramViewQListBoxItem;
class QTimer;


class LeftRightClickableKPushButton : public KPushButton
{
	Q_OBJECT
	public:
		LeftRightClickableKPushButton(const KGuiItem &item, QWidget *parent, const char *name);
		~LeftRightClickableKPushButton();

	protected:
		void mouseReleaseEvent(QMouseEvent * me);
		void mousePressEvent(QMouseEvent * me);
		
	signals:
		void rightButtonClicked(void);
		void leftButtonClicked(void);
		
	private:

};

class DataLogger
{
	public:
		DataLogger(unsigned int const maxV);
		~DataLogger();

		void addValue(bool const value);
		_List_iterator<bool>begin(void);

		void clear(void);

	private:
		unsigned int maxValues_;
		unsigned int listCount_;
		list<bool>logicValueList_;
};

class LogicAnalyzerW : public ExternalComponentBaseW
{
	Q_OBJECT
	public:
		//componentType pitää olla välillä 1-5
		LogicAnalyzerW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType);
		~LogicAnalyzerW(void);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:

		void reset_VirtualPart(void) override;
		void update_VirtualPart(void) override;
		double execute_VirtualPart(double executeTime) override;

	private:
		static unsigned int const  MAXLISTITEMS_=0x40000;
		static unsigned long long const TIMEAXELMULBASE_=1000;
		//static unsigned int const TIMEAXELSLIDERSTART_VALUE=250;
		//static unsigned int const TIMEAXELSLIDERSUBB_VALUE_=50;
		static unsigned int const TIMEAXELDECADEMULTIPLIER_MAXVALUE_=10;
		static unsigned int const TIMEAXEL_MAXVALUE_=100;

		QGridLayout* analyzerQGridLayout_;
		QGridLayout* bottomQGridLayout_;
		QScrollBar* hScrollB_;

		IOPin* pToFirstInputPin_;

		double timeCount_;
		double lastAddTime_;
		unsigned int listCount_;

		QSlider* timeAxelQSlider_;
		int timeAxelSliderValue_;
		unsigned long long timeAxelMultiplier_;
		int multiplierDecade_;
		inline unsigned long long clculateTimeAxelMultiplier(void);

		bool rasterOnValue_;
		int hScrollBValue_;

		QListBox *analyzerViewQListBox_;
		WaveViewQListBoxItem *squareWaveViewQListBoxItem_;
		TimeViewQListBoxItem *timeViewQListBoxItem_;
		ProgramViewQListBoxItem *programViewQListBoxItem_;
		int analyzerViewQListBox_Width_;

		KListBox *progarmCounterValuesKListBox_;
		int progarmCounterValuesKListBox_CurrentItem_;
		int startMachineCycle_;
		void sendProgramCounterValuesKListBoxCurrentItemAddressToParent(void);
		
		inline void clearPointedPoint(void);
		unsigned long pointedMachineCycleValue_;
		double machineCycleTime_;
		double pointedPointTime_;
		double prevPointedPointTime_;
		
		void readPaint_x_Values(void);
		unsigned long paintStart_x_;
		unsigned long paintEnd_x_;

		list<double> *timeList_; //Aikalista
		list<unsigned long> *machineCyclesValueList_;
		list<unsigned long> *programCounterValueList_;
		QPtrList<DataLogger>inputDataList_; //Aikalistaa vastaava inputtien arvolistan pointterilista.
		DataLogger *pToDataList;

		void setWindowSize(void);
		void addAnalyzerInput(void);
		void setStartTime(double const  time);
		void updateProgarmCounterValuesKListBox(void);
		void updateAnalyzerViewQListBox(bool const updateTwice);
		bool updateAnalyzerViewQListBoxTwice_;
				
		QTimer *updateAnalyzerViewQListBoxTimer_;
		
		KPushButton* rasterOnOffKPushButton_;
		LeftRightClickableKPushButton* multiplierDecadeLeftRightClickableKPushButton_;
		KPushButton* clearAndSetKPushButton_;
		KPushButton* onOffKPushButton_;
		KPixmap clearAndSetIcon_;
		KPixmap rasterOnIcon_;
		KPixmap onOffIcon_;

		KLed *analyzerViewQListBoxUpdateKLed_;
		KLed *analyzerViewQListBoxErrorKLed_;

		bool isOn_;

	private slots:
		void inputPinKListBoxSelectionChanged(void);
		void inputPinKListBoxClicked(QListBoxItem *item);
		void analyzerViewQListBoxSelectionChanged(void);
		void analyzerViewQListBoxClicked(QListBoxItem *item, const QPoint &pnt);
		void analyzerViewQListBoxRightButtonClicked(QListBoxItem *item, const QPoint &pnt);
		void progarmCounterValuesKListBoxPressed(QListBoxItem *item);
		void progarmCounterValuesKListBoxRightButtonPressed(QListBoxItem * item,const QPoint & point);
		
		void clearAndSetKPushButtonClicked(void);
		void setRasterOnOffKPushButtonClicked(void);
		void onOffKPushButtonClicked(void);
		void multiplierDecadeLeftRightClickableKPushButtonLeftButtonClicked(void);
		void multiplierDecadeLeftRightClickableKPushButtonRightButtonClicked(void);

		void analyzerViewQListBoxOnItem(QListBoxItem* );
		void timeAxelQSliderValueChanged(int);
		void hScrollBSliderReleased(void);
		void hScrollBSliderSliderPageMoved(void);

		void updateAnalyzerViewQListBoxTimer_timeout(void);
};

#endif
