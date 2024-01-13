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
//Cmos versio 8031:stä
//Perii ominaisuutensa luokista i80C31 ja i8032

#ifndef I80C32_H
#define I80C32_H

#include "I80C31.h"
#include "I8032.h"
#include "I80C32_NameS.h"


class I80C32: virtual public I8032, virtual public I80C31
{
	public:
		I80C32(void);
		virtual ~I80C32(void);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void reset_VirtualPart(void) override;
		
		//Uudelleenmääritys koska SMOD bitti kolla voidaan nopeuttaa baudeja
		//Uudelleenmääritys 80C31:stä Timer 2 baudrate geniksenä
		//Lähetetään sarjaportista, tekee keskytyksen TI
		void serialPortRxTxHandling(void) override;
};

#endif
