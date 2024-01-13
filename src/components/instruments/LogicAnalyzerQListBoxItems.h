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
#ifndef LOGICANALYZERQLISBOXITEMS_H
#define LOGICANALYZERQLISBOXITEMS_H

#include "LogicAnalyzerW.h"
#include "../../share/NumQString.h"

#include <qlistbox.h>
#include <qpainter.h>
#include <cmath>

class DataLogger;
class QListBoxItem;

namespace WaveViewPainter_NameS
{
	static unsigned int const MAX_PIXELSTOPAINT_=15000;
	static unsigned int const MAX_POINTEDPOINT_PAINT_X_=5000;
}

class WaveViewPainter
{
	public:
		WaveViewPainter(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle);
		~WaveViewPainter();

		void paintW(QPainter *p);
		int height(void) const;
		int width(void) const;

		unsigned long pointedXMachineCycle_Value(int const pointed_x_value);
		double pointedXTime_Value(int const pointed_x_value);
		double pointedXPrevTime_Value(int &pointed_x_value);
		double machineCycleTime_Value(unsigned long const cycleNumber);
		unsigned long pointedPointPainted_x_Value(void);
		unsigned long prevPointedPointPainted_x_Value(void);
		unsigned long paintStart_x_Value(void)const;
		unsigned long paintEnd_x_Value(void)const;

		
	protected:
		QPainter *painter_;
		QPen pen_;
		bool dataListValue_;
		unsigned long mCycle_;
		unsigned long pCounter_;
		long long last_x_;
		long long new_x_;
		long long start_x_;
		long long end_x_;
		unsigned long lastPaintedTimeChange_x_;
		int paintStart_x_;
		int paintEnd_x_;
		double timeCal_;
		double timeAdd_;
		//inline 
		virtual void paintStartValues(void);
		virtual void paintBeforeMainPaint(void);
		virtual void paintPointedPoint(void);
		virtual void paintPrevPointedPoint(void);
		virtual void paintWhenCycle(void);
		virtual void paintWhenNotCycle(void);
		virtual void paintAfterNewXCalc(void);
		virtual void paintBeforeExit(const bool notPaintedAll);

		int thisHeight_;
		int thisWidth_;
		int yhp2_;
		int yhp4_;
		int yh3p4_;
		int *pToListBoxWidth_;
		list<double> *pToTimeList_;
		list<unsigned long> *pToMachineCyclesValueList_;
		list<unsigned long> *pToProgramCounterValueList_;
		DataLogger *pToDataList_;
		unsigned long long *pToTimeAxelMultiplier_;
		int *pToTimeAxelSliderValue_;
		bool *pToRasterOn_;
		double *pToPointedTime_x_;
		int *pToHScrollBValue_;
		int *pToStartMachineCycle_;

		unsigned long prevPointedPoint_x_;
		double pointedPointPrev_time_;
 	
 private:
		WaveViewPainter(){};
		void setProtectedStartValues(void);
		bool draw_;
		inline void startDraw(void);
		bool paintCycleLines_;
		int last_y_;
		int new_y_;
		vector<unsigned long>pointedXMachineCycleValueVector_;
		vector<double>pointedXTimeVector_;
		vector<double>machineCycleTimeVector_;
		int pointedMachineCycleValueLastPaint_x_;
		bool pointedXSet_;
		unsigned long pointedPoint_x_;
		unsigned long paintedPointedPoint_x_;
		unsigned long prevPaintedPointedPoint_x_;
		double pointedXPrevTime_;
		int paintedPointedPointPrev_x_;
		
		bool prevPointedXSet_;
		double pointedXTimeChange_;

		QColor wavePaintColor_;

};

class WaveViewQListBoxItem : public QListBoxItem
{
	public:
		WaveViewQListBoxItem(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList,DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue_, int *pToStartMachineCycle);
		~WaveViewQListBoxItem();

		int height(const QListBox *lb) const override;
		int width(const QListBox *lb) const override;
		int paintedWidth(void) const;
		long pointedXMachineCycle(int const pointed_x_value);
		double pointedXTime(int const pointed_x_value);
		double pointedXPrevTime(int &pointed_x_value);
		double machineCycleTime(unsigned long const cycleNumber);
		unsigned long pointedPointPainted_x(void);
		unsigned long prevPointedPointPainted_x(void);
		unsigned long paintStart_x(void);
		unsigned long paintEnd_x(void);
	
	protected:
		void paint(QPainter *) override;

	private:
		WaveViewPainter* wPainter_;

};

class TimeViewPainter : public WaveViewPainter
{
	public:
		TimeViewPainter(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle);
		~TimeViewPainter();
		
	protected:
		NumQString timeLinesTxt_;
		NumQString hundredsPartTimeLinesTxt_;
		double adderToTimeLines_;
		int prevTimeText_x_;
		
		void paintStartValues(void) override;
		void paintBeforeMainPaint(void) override;
		void thisPaintBeforeMainPaint(bool const paintLines, bool const paintDimension);
		void paintWhenCycle(void) override;
		void paintWhenNotCycle(void) override;
		void paintAfterNewXCalc(void) override;
		void paintBeforeExit(const bool notPaintedAll) override;
		void paintPointedPoint(void) override;

	private:
		NumQString timeTxt_;
		double tenthPartsAdder_;
		double hundredsPartsAdder_;
		bool paintCycleLine_;
		QColor paintColor_;

		void paintDimensionText(int const x1, int const x2, int const y1, int const y2, NumQString &txt);
};

class TimeViewQListBoxItem : public QListBoxItem
{
	public:
		TimeViewQListBoxItem(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle);
		~TimeViewQListBoxItem();

		int height(const QListBox *lb) const override;
		int width(const QListBox *lb) const override;
		long pointedXMachineCycle(int const pointed_x_value);
		double pointedXTime(int const pointed_x_value);
		
	protected:
		void paint(QPainter *) override;
		
	private:
		TimeViewPainter* tPainter_;
};

class ProgramViewPainter : public TimeViewPainter
{
	public:
		ProgramViewPainter(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle);
		~ProgramViewPainter();
		
	protected:
		void paintStartValues(void) override;
		void paintBeforeMainPaint(void) override;
		void paintPointedPoint(void) override;
		void paintPrevPointedPoint(void) override;
		void paintWhenCycle(void) override;
		void paintWhenNotCycle(void) override;
		void paintAfterNewXCalc(void) override;
		void paintBeforeExit(const bool notPaintedAll) override;
		
	private:
		NumQString pCounterTxt_;
		int yhp2_;
		bool paintCycleText_;
		QColor paintColor_;
};

class ProgramViewQListBoxItem : public QListBoxItem
{
	public:
		ProgramViewQListBoxItem(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle);
		~ProgramViewQListBoxItem();

		int height(const QListBox *lb) const override;
		int width(const QListBox *lb) const override;
		long pointedXMachineCycle(int const pointed_x_value);
		double pointedXTime(int const pointed_x_value);
		bool painted(bool const reset);
		
	protected:
		void paint(QPainter *) override;
		
	private:
		ProgramViewPainter* pPainter_;
		bool painted_;
};

#endif
