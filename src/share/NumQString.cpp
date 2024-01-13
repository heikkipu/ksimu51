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
#include "NumQString.h"

//inline NumQString &NumQString::setNumSi(double const fn, int prec, QString &si)
//{
//	return setNumSi((double)fn, prec, si);
//}

NumQString &NumQString::setNumSi(double const dn, int prec, QString &si)
{
	if(dn<1.0&&dn>-1.0)
	{
		int n=0;
		double d=dn;
		if(dn<0.0)
			d*=-1.0;
		do
		{
			d*=1000.0;
			n++;
		}
		while(d<=.999999999999);
		n--;
		*this->setNum(d, 'f', prec);
		if(dn<0.0)
			*this->prepend("-");
		if(n<rStr.length())
			*this+=rStr[n];
			
	}
	else
	{
		*this->setNum(dn, 'f', prec);
	}
	*this+=si;
	return *this;
}

/*
inline QString &QString::setNum( short n, int base )
{ return setNum((Q_LLONG)n, base); }

inline QString &QString::setNum( ushort n, int base )
{ return setNum((Q_ULLONG)n, base); }

inline QString &QString::setNum( int n, int base )
{ return setNum((Q_LLONG)n, base); }

inline QString &QString::setNum( uint n, int base )
{ return setNum((Q_ULLONG)n, base); }
*/
