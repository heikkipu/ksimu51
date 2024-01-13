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
#ifndef NUMQSTRING_H_
#define NUMQSTRING_H_

#include <qstring.h>

class NumQString : public QString
{
	public:
		NumQString(){initialize();};
		NumQString(QChar ch) : QString(ch){initialize();};
		NumQString(const QString & s) : QString(s){initialize();};
		NumQString(const QByteArray & ba) : QString(ba){initialize();};
		NumQString(const QChar * unicode, uint length) : QString(unicode, length){initialize();};
		NumQString(const char *str) : QString(str){initialize();};
		NumQString(const std::string & str) : QString(str){initialize();};
		~NumQString(){};
		
		//NumQString &setNumSi(double const fn, int prec, QString &si);
		NumQString &setNumSi(double const dn, int prec, QString &si);
/*
		QString &setNum(short, int base=10 );
		QString &setNum(ushort, int base=10 );
		QString &setNum(int, int base=10 );
		QString &setNum(uint, int base=10 );
		QString &setNum(long, int base=10 );
		QString &setNum(ulong, int base=10 );
		QString &setNum(Q_LLONG, int base=10 );
		QString &setNum(Q_ULLONG, int base=10 );
		QString &setNum(double, char f='g', int prec=6 );
*/	
	private:
		QString rStr;
		void initialize(void){rStr="munpfazy";rStr[1]=181;};//181=myy
	
};

#endif
