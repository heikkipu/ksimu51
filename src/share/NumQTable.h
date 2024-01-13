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
#ifndef NUMQTABLE_H
#define NUMQTABLE_H

#include"MyQTableItem.h"

#include <qtable.h>
#include <vector>
#include <cmath>
#include <limits>


class ColumnBase
{
	public:
		ColumnBase(int const base, int const numbers){base_=base;numbers_=numbers;};
		~ColumnBase(){;};
	
		enum numBase{DEC, HEX, OCT, BIN};
		int base(void){return base_;};
		int numbers(void){return numbers_;};
		
	private:
		int base_;
		int numbers_;	
};

class NumQTable : public QTable
{
	Q_OBJECT
	public:
		NumQTable(QWidget *parent, const char *name, const char *fontFamily, int const fontPointSize);
		~NumQTable();

		void addColumn(int const base, int const numbers);
		void addRow(const QString& name);
		void setValue(int const row, long const value);
		
		signals:
			void numTableValueChanged(int, int);
			
	protected:
	
	private slots:
		void thisTableMouseDoubleClicked(int row, int col, int button, const QPoint& mousePos);
		void thisTableCurrentChanged(int row, int col);
		void thisTableValueChanged( int row, int col);
		
	
	private:
		int fontPointSize_;
		MyQTableItem *pToNumTableItem_;
		ColumnBase *pToColumnBase_;
		std::vector<ColumnBase*>columnBaseVec_;
		QString thisTableCurrentCellText_;
		unsigned long maxTableValue_;

		void setValueDec(int const row, int const col, long const value);
		void setValueHex(int const row, int const col, long const value);
		void setValueOct(int const row, int const col, long const value);
		void setValueBin(int const row, int const col, long const value);
};

#endif
