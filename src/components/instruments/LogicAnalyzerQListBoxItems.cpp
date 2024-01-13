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
#include "LogicAnalyzerQListBoxItems.h"

WaveViewPainter::WaveViewPainter(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle)
{
	thisHeight_=itemHeight;
	pToListBoxWidth_=pToListBoxWidth;
	pToTimeList_=pToTimeList;
	pToMachineCyclesValueList_=pToMachineCyclesValueList;
	pToProgramCounterValueList_=pToProgramCounterValueList;
	pToDataList_=pToDataList;
	pToTimeAxelMultiplier_=pToTimeAxelMultiplier;
	pToTimeAxelSliderValue_=pToTimeAxelSliderValue;
	pToRasterOn_=pToRasterOnValue;
	pToPointedTime_x_=pToPointedTime_x;
	pToHScrollBValue_=pToHScrollBValue;
	pToStartMachineCycle_=pToStartMachineCycle;

	thisWidth_=0;

	pointedXSet_=false;

	prevPointedPoint_x_=0;
	pointedPointPrev_time_=.0;
	pointedXTimeChange_=.0;
	prevPointedXSet_=false;
}

WaveViewPainter::~WaveViewPainter()
{
}

inline void WaveViewPainter::setProtectedStartValues(void)
{
	last_x_=0;
	new_x_=0;
	start_x_=0;
	paintStart_x_=0;
	thisWidth_=0;
	yhp2_=thisHeight_/2;
	yhp4_=yhp2_>>1;
	yh3p4_=thisHeight_-yhp4_;
	timeAdd_=.0;
	pointedMachineCycleValueLastPaint_x_=0;
	draw_=false;
}

void WaveViewPainter::startDraw(void)
{
	draw_=true;
	start_x_=last_x_;
	paintStart_x_=0;
	paintEnd_x_=new_x_-last_x_;
	pointedMachineCycleValueLastPaint_x_=paintStart_x_;
	lastPaintedTimeChange_x_=paintStart_x_;
	pointedXMachineCycleValueVector_.push_back(mCycle_);
	pointedXTimeVector_.push_back(timeAdd_);
	pointedXSet_=false;
	prevPointedXSet_=false;
	paintedPointedPoint_x_=0;
	prevPaintedPointedPoint_x_=0;
}

void WaveViewPainter::paintW(QPainter *p)
{
	if(!pToTimeList_->empty())
	{
		painter_=p;
		pen_=painter_->pen();
		setProtectedStartValues();
		list<double>::iterator time_it=pToTimeList_->begin();
		list<unsigned long>::iterator machineC_it=pToMachineCyclesValueList_->begin();
		list<unsigned long>::iterator programC_it=pToProgramCounterValueList_->begin();
		list<bool>::iterator value_it=pToDataList_->begin();
		pointedXMachineCycleValueVector_.clear();
		pointedXTimeVector_.clear();
		machineCycleTimeVector_.clear();
		uint numTimes=pToTimeList_->size();
		double prevTimeAdd=timeAdd_;
		pointedXPrevTime_=prevTimeAdd;
		timeCal_=*time_it;
		unsigned long lastMCycle=numeric_limits<unsigned long >::max();
		unsigned long cycleCount=0;
		bool doNotDrawAnymore=false;
		//Osoitettu x akseli alkulaskenta
		pointedPoint_x_=*pToPointedTime_x_**pToTimeAxelMultiplier_;
		long long startDPoint=(long long)pointedPoint_x_-(long long)WaveViewPainter_NameS::MAX_POINTEDPOINT_PAINT_X_;
		if(startDPoint<0)
			startDPoint=0;
		//Edellinen osoitetttu x.
		if(pointedXTimeChange_!=*pToPointedTime_x_)
		{
			pointedPointPrev_time_=pointedXTimeChange_;
			pointedXTimeChange_=*pToPointedTime_x_;
		}
		prevPointedPoint_x_=pointedPointPrev_time_**pToTimeAxelMultiplier_;
		paintStartValues();
		int prevLastPaintedTimeChange_x=lastPaintedTimeChange_x_;
		paintedPointedPointPrev_x_=paintStart_x_;
		//Piirto ennen varsinaista piirtoa
		paintBeforeMainPaint();
		unsigned long n;
		for(n=0; n<numTimes; n++)
		{
			dataListValue_=*value_it++;
			mCycle_=*machineC_it++;
			pCounter_=*programC_it++;
			//MachineCyclen muutos
			if((lastMCycle!=mCycle_)&&n)
			{
				cycleCount++;
				machineCycleTimeVector_.push_back(timeAdd_);
				if(!doNotDrawAnymore)
				{
					//Aloitusehto jos ei osoitettua x koordinaattia.
					if(!draw_)
						if(!pointedPoint_x_)
							startDraw();
					//Piirretään machine cyclen kohta.
					if(draw_)
					{
						paintWhenCycle();
						//Rukataan klikattua pistettä vastaavaa machine cycle vektoria, jotta mc alkaa oikein.
						if(cycleCount>1)
						{
							while(pointedMachineCycleValueLastPaint_x_>prevLastPaintedTimeChange_x)
							{
								pointedXMachineCycleValueVector_.pop_back();
								pointedMachineCycleValueLastPaint_x_--;
							}
							while(pointedMachineCycleValueLastPaint_x_<paintStart_x_)
							{
								pointedXMachineCycleValueVector_.push_back(mCycle_);
								pointedMachineCycleValueLastPaint_x_++;
							}
						}
					}
				}
				lastMCycle=mCycle_;
			}
			else
				//Piirretään ajanmuutoksen kohta.(pystyviivat)
				if(!doNotDrawAnymore)
					if(draw_)
					{
						paintWhenNotCycle();
						prevLastPaintedTimeChange_x=lastPaintedTimeChange_x_;
						lastPaintedTimeChange_x_=paintStart_x_;
					}
			//Lasketaan uusi x koordinaatti.
			timeAdd_+=*time_it-timeCal_;
			new_x_=timeAdd_**pToTimeAxelMultiplier_;
			paintEnd_x_=new_x_-start_x_;
			if(!doNotDrawAnymore)
			{
				end_x_=new_x_;
				if(!draw_)
				{
					//Aloitusehto jos osoitettu x piste.
					if(pointedPoint_x_)
					{
						if(new_x_>=startDPoint)
							startDraw();
						else
							paintEnd_x_=0;
					}
				}
				if(draw_)
				{
					//Piirron lopetusehto.
					if(paintEnd_x_>WaveViewPainter_NameS::MAX_PIXELSTOPAINT_)
					{
						end_x_-=paintEnd_x_-WaveViewPainter_NameS::MAX_PIXELSTOPAINT_;
						paintEnd_x_=WaveViewPainter_NameS::MAX_PIXELSTOPAINT_;
						doNotDrawAnymore=true;
						//Jos menee yli alueen, rajotetaan vähän.
						if(pointedPoint_x_)
							if(end_x_<pointedPoint_x_)
							{
								start_x_=startDPoint;
								paintEnd_x_=WaveViewPainter_NameS::MAX_POINTEDPOINT_PAINT_X_;
								doNotDrawAnymore=false;
							}
					}
					//Piirretään alku- ja loppupisteiden väli.
					paintAfterNewXCalc();
					//Kasvatetaan osoitettua x pistettä vastaavia vektoreita.
					while(pointedMachineCycleValueLastPaint_x_<paintEnd_x_)
					{
						pointedXMachineCycleValueVector_.push_back(mCycle_);
						pointedXTimeVector_.push_back(timeAdd_);
						pointedMachineCycleValueLastPaint_x_++;
					}
					//Piirretään osoitettu pisteviiva.
					if(pointedPoint_x_)
					{
						if(!pointedXSet_)
						{
							if(last_x_>=pointedPoint_x_)
							{
								paintedPointedPoint_x_=paintStart_x_;
								pointedXSet_=true;
								paintPointedPoint();
							}
							if((new_x_>=pointedPoint_x_)&&!pointedXSet_)
							{
								pointedXPrevTime_=prevTimeAdd;
								paintedPointedPointPrev_x_=paintStart_x_;
							}
						}
						//Piirretään edellinen osoitettu viiva
						if(!prevPointedXSet_)
							if(last_x_>=prevPointedPoint_x_)
							{
								prevPointedXSet_=true;
								prevPaintedPointedPoint_x_=paintStart_x_;
								paintPrevPointedPoint();
							}
					}
				}
			}
			last_x_=new_x_;
			paintStart_x_=paintEnd_x_;
			prevTimeAdd=timeAdd_;
			timeCal_=*time_it++;
		}
		//Piirretään poistuttaessa.
		if(draw_)
			paintBeforeExit(doNotDrawAnymore);
	}
}

unsigned long WaveViewPainter::pointedXMachineCycle_Value(int const pointed_x_value)
{
	if(pointed_x_value<pointedXMachineCycleValueVector_.size())
		return pointedXMachineCycleValueVector_[pointed_x_value];
	return 0;
}

double WaveViewPainter::pointedXTime_Value(int const pointed_x_value)
{
	if(pointed_x_value<pointedXTimeVector_.size())
		return pointedXTimeVector_[pointed_x_value];
	return 0.0;
}

double WaveViewPainter::pointedXPrevTime_Value(int &pointed_x_value)
{
	pointed_x_value=paintedPointedPointPrev_x_;
	return pointedXPrevTime_;
}

double WaveViewPainter::machineCycleTime_Value(unsigned long const cycleNumber)
{
	if(cycleNumber<machineCycleTimeVector_.size())
		return machineCycleTimeVector_[cycleNumber];
	return 0.0;
}

unsigned long WaveViewPainter::pointedPointPainted_x_Value(void)
{
	if(pointedXSet_)
		return paintedPointedPoint_x_;
	return 0;
}

unsigned long WaveViewPainter::prevPointedPointPainted_x_Value(void)
{
	if(prevPointedXSet_)
		return prevPaintedPointedPoint_x_;
	return 0;
}

unsigned long WaveViewPainter::paintStart_x_Value(void) const
{
	return start_x_;
}

unsigned long WaveViewPainter::paintEnd_x_Value(void) const
{
	return end_x_;
}

inline void WaveViewPainter::paintStartValues(void)
{
	last_y_=thisHeight_-4;
	new_y_=thisHeight_-4;
	
	paintCycleLines_=false;
	if(*pToTimeAxelMultiplier_>=1.0e8)
		paintCycleLines_=true;

	wavePaintColor_=QColor(Qt::blue);
}

inline void WaveViewPainter::paintBeforeMainPaint(void)
{
	pen_.setColor(QColor(Qt::black));
	pen_.setWidth(1);
	painter_->setPen(pen_);
	painter_->drawLine(0, thisHeight_-1, WaveViewPainter_NameS::MAX_PIXELSTOPAINT_, thisHeight_-1);
	thisWidth_=WaveViewPainter_NameS::MAX_PIXELSTOPAINT_+50;
}

inline void WaveViewPainter::paintPointedPoint(void)
{
	pen_.setColor(QColor(Qt::red));
	if(*pToRasterOn_)
		pen_.setWidth(3);
	else
		pen_.setWidth(2);
	painter_->setPen(pen_);
	painter_->drawLine(paintStart_x_, 0, paintStart_x_, thisHeight_);
}

inline void WaveViewPainter::paintPrevPointedPoint(void)
{
	pen_.setColor(QColor(Qt::green));
	if(*pToRasterOn_)
		pen_.setWidth(3);
	else
		pen_.setWidth(2);
	painter_->setPen(pen_);
	painter_->drawLine(paintStart_x_, 0, paintStart_x_, thisHeight_);
}

inline void WaveViewPainter::paintWhenCycle(void)
{
	if(paintCycleLines_)
	{
		pen_.setColor(QColor(Qt::black));
		if(*pToRasterOn_)
			pen_.setWidth(2);
		else
			pen_.setWidth(1);
		painter_->setPen(pen_);
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, thisHeight_);
	}
}

inline void WaveViewPainter::paintWhenNotCycle(void)
{
	if(*pToRasterOn_&&paintCycleLines_)
	{
		pen_.setColor(QColor(Qt::gray)); //Muuten sitten harmaata kappeeta
		pen_.setWidth(1);
		painter_->setPen(pen_);
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, thisHeight_);
	}
}

inline void WaveViewPainter::paintAfterNewXCalc(void)
{
	new_y_=thisHeight_-4;
	if(dataListValue_)
		new_y_=4;
	pen_.setColor(wavePaintColor_);
	pen_.setWidth(2);
	painter_->setPen(pen_);
	painter_->drawLine(paintStart_x_, last_y_, paintStart_x_, new_y_);
	painter_->drawLine(paintStart_x_, new_y_, paintEnd_x_, new_y_);
	last_y_=new_y_;
}

inline void WaveViewPainter::paintBeforeExit(const bool notPaintedAll)
{
	if(!notPaintedAll)
	{
		pen_.setColor(QColor(Qt::black));
		pen_.setWidth(2);
		painter_->setPen(pen_);
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, thisHeight_);
	}
}

int WaveViewPainter::height(void) const
{
	return thisHeight_;
}

int WaveViewPainter::width(void) const
{
	return thisWidth_;
}

WaveViewQListBoxItem::WaveViewQListBoxItem(int const itemHeight, int *pToListBoxWidth, list<double>* pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger* pToDataList, unsigned long long* pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool* pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle) : QListBoxItem()
{
	wPainter_=new WaveViewPainter(itemHeight, pToListBoxWidth, pToTimeList, pToMachineCyclesValueList, pToProgramCounterValueList, pToDataList, pToTimeAxelMultiplier, pToTimeAxelSliderValue, pToRasterOnValue, pToPointedTime_x, pToHScrollBValue, pToStartMachineCycle);
}

WaveViewQListBoxItem::~WaveViewQListBoxItem()
{
	delete wPainter_;
}

void WaveViewQListBoxItem::paint(QPainter *p)
{
	wPainter_->paintW(p);
}

int WaveViewQListBoxItem::height(const QListBox * lb) const
{
	return wPainter_->height();
}

int WaveViewQListBoxItem::width(const QListBox * lb) const
{
	return wPainter_->width();
}

long WaveViewQListBoxItem::pointedXMachineCycle(int const pointed_x_value)
{
	return wPainter_->pointedXMachineCycle_Value(pointed_x_value);
}

double WaveViewQListBoxItem::pointedXTime(int const pointed_x_value)
{
	return wPainter_->pointedXTime_Value(pointed_x_value);
}

double WaveViewQListBoxItem::pointedXPrevTime(int &pointed_x_value)
{
	return wPainter_->pointedXPrevTime_Value(pointed_x_value);
}

double WaveViewQListBoxItem::machineCycleTime(unsigned long const cycleNumber)
{
	return wPainter_->machineCycleTime_Value(cycleNumber);
}

unsigned long WaveViewQListBoxItem::pointedPointPainted_x(void)
{
	return wPainter_->pointedPointPainted_x_Value();
}

unsigned long WaveViewQListBoxItem::prevPointedPointPainted_x(void)
{
	return wPainter_->prevPointedPointPainted_x_Value();
}

unsigned long WaveViewQListBoxItem::paintStart_x(void)
{
	return wPainter_->paintStart_x_Value();
}

unsigned long WaveViewQListBoxItem::paintEnd_x(void)
{
	return wPainter_->paintEnd_x_Value();
}

//Time listboxi-----------------------------------------
TimeViewPainter::TimeViewPainter(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle) : WaveViewPainter(itemHeight, pToListBoxWidth, pToTimeList, pToMachineCyclesValueList, pToProgramCounterValueList, pToDataList, pToTimeAxelMultiplier, pToTimeAxelSliderValue, pToRasterOnValue, pToPointedTime_x, pToHScrollBValue, pToStartMachineCycle)
{
}

TimeViewPainter::~TimeViewPainter()
{
}

inline void TimeViewPainter::paintStartValues(void)
{
	paintCycleLine_=false;
	if(*pToTimeAxelMultiplier_>=1.0e7)
		paintCycleLine_=true;
	
	adderToTimeLines_=1.0;
	timeLinesTxt_="1s";
	unsigned long long tam=*pToTimeAxelMultiplier_;
	int n=0;
	double div=1000.0;
	while(tam>0)
	{
		n++;
		tam/=10;
		div/=10.0;
	}
	QString qstr="s";
	timeLinesTxt_.setNumSi(div, 0, qstr);
	hundredsPartTimeLinesTxt_.setNumSi(div/10.0, 0, qstr);
	adderToTimeLines_=div**pToTimeAxelMultiplier_;
	tenthPartsAdder_=adderToTimeLines_/10.0;
	hundredsPartsAdder_=tenthPartsAdder_/10.0;

	paintColor_=QColor(Qt::gray);
}

inline void TimeViewPainter::paintDimensionText(int const x1, int const x2, int const y1, int const y2, NumQString &txt)
{
	int tlen=txt.length();
	int halfStrPix=tlen*8/2;
	int spx2=x1+(x2-x1)/2;
	int y3=y1+(y2-y1)/2;
	painter_->drawLine(x1, y1, x1, y2);
	painter_->drawLine(x1, y3, spx2-halfStrPix-5, y3);
	painter_->drawLine(spx2+halfStrPix+5, y3, x2, y3);
	painter_->drawLine(x2, y1, x2, y2);
	painter_->drawText(spx2-halfStrPix, y2-1, txt, -1, QPainter::LTR);
}

void TimeViewPainter::thisPaintBeforeMainPaint(bool const paintLines, bool const paintDimension)
{
	pen_.setColor(paintColor_);
	pen_.setWidth(1);
	painter_->setPen(pen_);
	double xToTimeLinesCount_=.0;
	prevTimeText_x_=int(xToTimeLinesCount_);
	bool paintHundredsParts=false;
	if(hundredsPartsAdder_>=3.0)
		paintHundredsParts=true;
	bool textDrawn=false;
	while(int(xToTimeLinesCount_)<WaveViewPainter_NameS::MAX_PIXELSTOPAINT_)
	{
		if(paintLines)
		{
			painter_->drawLine(int(xToTimeLinesCount_), 0, int(xToTimeLinesCount_), thisHeight_);
			for(int z=1; z<11;z++)
			{
				int zx=int(xToTimeLinesCount_)+tenthPartsAdder_*z;
				if((z<10)&&(zx<WaveViewPainter_NameS::MAX_PIXELSTOPAINT_))
					painter_->drawLine(zx, 0, zx, yhp2_);
				if(paintHundredsParts)
					for(int hp=1; hp<10; hp++)
					{
						int hpx=zx-hp*hundredsPartsAdder_;
						if(hpx<WaveViewPainter_NameS::MAX_PIXELSTOPAINT_)
							painter_->drawLine(hpx, 0, hpx, yhp4_);
					}
			}
		}
		if(paintDimension)
		{
			if(!textDrawn&&(*pToHScrollBValue_<=prevTimeText_x_)&&(prevTimeText_x_>0))
			{
				paintDimensionText(prevTimeText_x_, int(xToTimeLinesCount_), yhp2_, thisHeight_, timeLinesTxt_);
				textDrawn=true;
				if(paintHundredsParts)
				{
					prevTimeText_x_=prevTimeText_x_-tenthPartsAdder_*2;
					paintDimensionText(prevTimeText_x_, prevTimeText_x_+tenthPartsAdder_, yhp2_, thisHeight_, hundredsPartTimeLinesTxt_);
		}
			}
			prevTimeText_x_=int(xToTimeLinesCount_);
		}
		xToTimeLinesCount_+=adderToTimeLines_;
	}
}

inline void TimeViewPainter::paintBeforeMainPaint(void)
{
	thisPaintBeforeMainPaint(true, false);

}

inline void TimeViewPainter::paintWhenCycle(void)
{
	if(paintCycleLine_)
	{
		pen_.setWidth(1);
		pen_.setColor(paintColor_);
		painter_->setPen(pen_);
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, thisHeight_);
	}
}

inline void TimeViewPainter::paintWhenNotCycle(void)
{
	//Nothing to do!
}

inline void TimeViewPainter::paintAfterNewXCalc(void)
{
/*	if(*pToRasterOn_&&paintCycleText_)
	{
		pen_.setColor(paintColor_);
		pen_.setWidth(1);
		painter_->setPen(pen_);
	}
 */
}

inline void TimeViewPainter::paintBeforeExit(const bool notPaintedAll)
{
	if(!notPaintedAll)
	{
		pen_.setColor(paintColor_);
		pen_.setWidth(1);
		painter_->setPen(pen_);
		QString qstr="s";
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, yhp2_);
		painter_->eraseRect(paintStart_x_-1, 0, timeTxt_.length()*8+4, yhp2_);
		timeTxt_.setNumSi(timeCal_, 3, qstr);
		painter_->drawText(paintStart_x_, yhp2_, timeTxt_, -1, QPainter::LTR);
	}
}

inline void TimeViewPainter::paintPointedPoint(void)
{
	if(prevPointedPoint_x_)
	{
		pen_.setColor(QColor(Qt::red));
		if(*pToRasterOn_)
			pen_.setWidth(3);
		else
			pen_.setWidth(2);
		painter_->setPen(pen_);
		QString qstr="s";
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, yhp2_);
		timeTxt_.setNumSi(*pToPointedTime_x_-pointedPointPrev_time_, 3, qstr);
		painter_->eraseRect(paintStart_x_-1, yhp2_, timeTxt_.length()*8+4, yhp2_);
		painter_->drawText(paintStart_x_, thisHeight_-1, timeTxt_, -1, QPainter::LTR);
	}
	else
		WaveViewPainter::paintPointedPoint();
}

TimeViewQListBoxItem::TimeViewQListBoxItem(int const itemHeight, int *pToListBoxWidth, list<double>* pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle) : QListBoxItem()
{
	tPainter_=new TimeViewPainter(itemHeight, pToListBoxWidth, pToTimeList, pToMachineCyclesValueList, pToProgramCounterValueList, pToDataList, pToTimeAxelMultiplier, pToTimeAxelSliderValue, pToRasterOnValue, pToPointedTime_x, pToHScrollBValue, pToStartMachineCycle);
	
}

TimeViewQListBoxItem::~TimeViewQListBoxItem()
{
	delete tPainter_;
}

void TimeViewQListBoxItem::paint(QPainter *p)
{
	tPainter_->paintW(p);
}

int TimeViewQListBoxItem::height(const QListBox * lb) const
{
	return tPainter_->height();
}

int TimeViewQListBoxItem::width(const QListBox * lb) const
{
	return tPainter_->width();
}

long TimeViewQListBoxItem::pointedXMachineCycle(int const pointed_x_value)
{
	return tPainter_->pointedXMachineCycle_Value(pointed_x_value);
}

double TimeViewQListBoxItem::pointedXTime(int const pointed_x_value)
{
	return tPainter_->pointedXTime_Value(pointed_x_value);
}

ProgramViewPainter::ProgramViewPainter(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle): TimeViewPainter(itemHeight, pToListBoxWidth, pToTimeList, pToMachineCyclesValueList, pToProgramCounterValueList, pToDataList, pToTimeAxelMultiplier, pToTimeAxelSliderValue, pToRasterOnValue, pToPointedTime_x, pToHScrollBValue, pToStartMachineCycle)
{
}

ProgramViewPainter::~ProgramViewPainter(){};


inline void ProgramViewPainter::paintStartValues(void)
{
	TimeViewPainter::paintStartValues();
	
	paintCycleText_=false;
	if(*pToTimeAxelMultiplier_>=1.0e7)
		paintCycleText_=true;

	yhp2_=thisHeight_/2;
	paintColor_=QColor(Qt::gray);
}

inline void ProgramViewPainter::paintBeforeMainPaint(void)
{
	thisPaintBeforeMainPaint(false, true);
}

inline void ProgramViewPainter::paintPointedPoint(void)
{
}

inline void ProgramViewPainter::paintPrevPointedPoint(void)
{
}

inline void ProgramViewPainter::paintWhenCycle(void)
{
	if(paintCycleText_)
	{
		pen_.setColor(paintColor_);
		painter_->setPen(pen_);
		painter_->drawLine(paintStart_x_, 0, paintStart_x_, yhp2_);
		pCounterTxt_.setNum(pCounter_, 16);
		painter_->drawText(paintStart_x_+2, yhp2_, pCounterTxt_.upper(), -1, QPainter::LTR);
	}
}

inline void ProgramViewPainter::paintWhenNotCycle(void)
{
}

inline void ProgramViewPainter::paintAfterNewXCalc(void)
{
}

inline void ProgramViewPainter::paintBeforeExit(const bool notPaintedAll)
{
}

ProgramViewQListBoxItem::ProgramViewQListBoxItem(int const itemHeight, int *pToListBoxWidth, list<double> *pToTimeList, list<unsigned long> *pToMachineCyclesValueList, list<unsigned long> *pToProgramCounterValueList, DataLogger *pToDataList, unsigned long long *pToTimeAxelMultiplier, int *pToTimeAxelSliderValue, bool *pToRasterOnValue, double *pToPointedTime_x, int *pToHScrollBValue, int *pToStartMachineCycle): QListBoxItem()
{
	pPainter_=new ProgramViewPainter(itemHeight, pToListBoxWidth, pToTimeList, pToMachineCyclesValueList, pToProgramCounterValueList, pToDataList, pToTimeAxelMultiplier, pToTimeAxelSliderValue, pToRasterOnValue, pToPointedTime_x, pToHScrollBValue, pToStartMachineCycle);
	
	painted_=false;
}

ProgramViewQListBoxItem::~ProgramViewQListBoxItem()
{
}

int ProgramViewQListBoxItem::height(const QListBox *lb) const
{
	return pPainter_->height();
}

int ProgramViewQListBoxItem::width(const QListBox *lb) const
{
	return pPainter_->width();
}

long ProgramViewQListBoxItem::pointedXMachineCycle(int const pointed_x_value)
{
	return pPainter_->pointedXMachineCycle_Value(pointed_x_value);
}

double ProgramViewQListBoxItem::pointedXTime(int const pointed_x_value)
{
	return pPainter_->pointedXTime_Value(pointed_x_value);
}

bool ProgramViewQListBoxItem::painted(bool const reset)
{
	if(reset)
		painted_=false;
	return painted_;
}		
		
void ProgramViewQListBoxItem::paint(QPainter *p)
{
	pPainter_->paintW(p);
	painted_=true;
}
