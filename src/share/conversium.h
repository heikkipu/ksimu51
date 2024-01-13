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
//Jotain vanhaa vuodelta -91 ja sillee. hp

#ifndef CONVERSIUM_H
#define CONVERSIUM_H

#include <string>
#include <cmath>

char hexToAsc(unsigned char const  hLuku);   //Heksaluku asciimerkiksi
unsigned char ascToHex(char const  aMerkki);   //ascii merkki heksaluvuksi
int aschex (char a,char b);   //Kaksi acsii merkkiä yhdeksi heksaluvuksi
std::string hexToString(unsigned int hLuku, unsigned int palautusPituus);   //Heksaluku ascii merkkijonoksi
std::string decToString(unsigned int dLuku, unsigned int palautusPituus, char etumerkki);   //Dec luku merkkijonoksi
std::string binToString(unsigned int bLuku, unsigned int palautusPituus);   //Binääriluku merkkijonoksi

unsigned int binStringToUInt(std::string& strBinLuku);   //Binäärimerkkijon luvuksi
int hexStringToHex(std::string& strHexLuku);   //Heksamerkkijono luvuksi
int decStringToDec(std::string& strDecLuku);   //Desimaalimerkkijono luvuksi

bool isStringBin(std::string& strBinLuku);   //Tutkii onko merkkijono binääriluku
bool isStringDec(std::string& strDecLuku);   //Tutkii onko merkkijono desimaaliluku
bool isStringHex(std::string& strHexLuku);   //Tutkii onko merkkijono heksaluku

#endif
