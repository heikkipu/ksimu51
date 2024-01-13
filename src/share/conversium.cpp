/**************************************************************************
*   Copyright (C) 1991 - ... Heikki Pulkkinen                             *
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
#include "conversium.h"


char hexToAsc(unsigned char const  hLuku)
{
	char retVal=hLuku;
	retVal&=0x0F;
	if ((retVal>=0x0)&&(retVal<0x0A))
		return retVal+0x30;
	else
		return  retVal-0x0A+0x41;
}

unsigned char ascToHex(char const  aMerkki)
{
	unsigned char retVal=aMerkki;
	if ((retVal>='0')&&(retVal<='9'))
		return retVal&0x0f;
	else
		if((retVal>='A')&&(retVal<='F'))
			return retVal-0x41+0x0A;
		else
			if((retVal>='a')&&(retVal<='f'))
				return retVal-0x61+0x0A;
	return 0;
}

int aschex (char a,char b)
// Aliohjelma joka muuttaa sille tuodut kakasi HEX merkkiä binääriluvuksi
{
if ((a>='0')&&(a<='9'))
	a=a&0x0f;
if ((a>='A')&&(a<='F'))
{ a=a&0x0f;
	a+=9;
}
if ((b>='0')&&(b<='9'))
	b=b&0x0f;
if ((b>='A')&&(b<='F'))
{ b=b&0x0f;
	b+=9;
}
a=a<<4;
return (a+b);
}

std::string hexToString(unsigned int hLuku, unsigned int palautusPituus)
{
	std::string s("");
	if((hLuku>0x0FFF) || (palautusPituus>3))
		s+=hexToAsc((hLuku>>12)&0x000F);
	if((hLuku>0x00FF) || (palautusPituus>2))
		s+=hexToAsc((hLuku>>8)&0x000F);
	if((hLuku>0x000F) || (palautusPituus>1))
		s+=hexToAsc((hLuku>>4)&0x000F);
	s+=hexToAsc(hLuku&0x000F);
	return s;
}

std::string binToString(unsigned int bLuku, unsigned int palautusPituus)
{
	std::string s("");
	if(palautusPituus>4)
		return s;
	int n,r=0x1;
	r<<=palautusPituus*8;
	r>>=1;
	for(n=0;n<(8*palautusPituus);n++)
	{

		if((bLuku)&((r)>>n))
			s+="1";
		else
			s+="0";
	}
	return s;
}

std::string decToString(unsigned int dLuku, unsigned int palautusPituus, char etumerkki)
{
	std::string s("");
	unsigned int al=dLuku;
	if((dLuku<100000) && (palautusPituus>=6))
		s+=etumerkki;
	if(dLuku>9999)
	{
		s+=hexToAsc(al/10000);
		al%=10000;
	}
	if((dLuku<10000) && (palautusPituus>=5))
		s+=etumerkki;
	if(dLuku>999)
	{
		s+=hexToAsc(al/1000);
		al%=1000;
	}
	if((dLuku<1000) && (palautusPituus>=4))
		s+=etumerkki;
	if(dLuku>99)
	{
		s+=hexToAsc(al/100);
		al%=100;
	}
	if((dLuku<100) && (palautusPituus>=3))
		s+=etumerkki;
	if(dLuku>9)
	{
		s+=hexToAsc(al/10);
		al%=10;
	}
	if((dLuku<10) && (palautusPituus>=2))
		s+=etumerkki;
	s+=hexToAsc(al);
	return s;
}

int hexStringToHex(std::string& strHexLuku)
{
	int hexLuku=0;
	for(int n=0;n<strHexLuku.length();n++)
	{
		hexLuku<<=4;
		hexLuku|=ascToHex(strHexLuku[n]);
	}
	return hexLuku;
}

int decStringToDec(std::string& strDecLuku)
{
	int decLuku=0;
	for(int n=0;n<strDecLuku.length();n++)
	{
		decLuku*=10;
		decLuku+=ascToHex(strDecLuku[n]);
	}
	return decLuku;
}

unsigned int binStringToUInt(std::string& strBinLuku)
{
	int n;
	unsigned int retVal=0;
	double d;
	int binLenght=strBinLuku.length();
	for(n=0;n<binLenght;n++)
	{
		if(strBinLuku[n]=='1')
		{
			d=pow(2.0,(binLenght-n-1));
			retVal+=d;
		}
	}
	return retVal;
}

bool isStringBin(std::string& strBinLuku)
{
	bool retVal=true;
	for(int n=0;n<strBinLuku.length();n++)
	{
		if(!((strBinLuku[n]=='0')||(strBinLuku[n]=='1')))
			retVal=false;
	}
	return retVal;
}

bool isStringDec(std::string& strDecLuku)
//huomioi etutyhjät
{
	bool l=true;
	bool ekaluku=true;
	bool ekaonluku=false;
	char c;
	if(strDecLuku!="")
	{
		for(int n=strDecLuku.length()-1;n>0;n--)
		{
			c=strDecLuku[n];
			if((c>='0')&&(c<='9'))
			{
				if(ekaluku)
					ekaonluku=true;
			}
			else
			{
				if(ekaonluku)
				{
					if(c!=' ')
						l=false;
				}
				else
					l=false;
			}
		}
	}
	else
		l=false;
	return l;
}

bool isStringHex(std::string& strHexLuku)
//huomioi etutyhjät
{
	bool l=true;
	bool ekaluku=true;
	bool ekaonluku=false;
	char c;
	for(int n=strHexLuku.length()-1;n>=0;n--)
	{
		c=strHexLuku[n];
		if(((c>='0')&&(c<='9'))||((c>='A')&&(c<='F'))||((c>='a')&&(c<='f')))
		{
			if(ekaluku)
				ekaonluku=true;
		}
		else
		{
			if(ekaonluku)
			{
				if(c!=' ')
					l=false;
			}
			else
				l=false;
		}
		ekaluku=false;
	}
	return l;
}
