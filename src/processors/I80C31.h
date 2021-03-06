//
// C++ Interface: I80C31
//
// Description:
//
//
// Author: Heikki Pulkkinen <heikki@CompaQ7113EA>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//CMOS version 8031
//Lisänä PCON rekkari
#ifndef I80C31_H
#define I80C31_H

#include "I8031.h"
#include "I80C31_NameS.h"
#include "I8031_NameS.h"

class I80C31: virtual public I8031
{
	public:
		I80C31();
		virtual ~I80C31();

		//Uudelleen määritetty, koska IDLE ja POWER DOWN MODET
		void step(bool const disassemble, bool const run);

		void saveDataToFileStream(QDataStream& saveFileStream);
		void readDataFromFileStream(QDataStream& readFileStream);

	protected:
		void reset_VirtualPart(void);
		
		//Uudelleenmääritys koska SMOD bitti kolla voidaan nopeuttaa baudeja
		//Lähetetään sarjaportista, tekee keskytyksen TI
		void serialPortRxTxHandling(void);

	private:
		bool idleMode_;      //Keskeytys tai resetti lopettaa idle moden
		bool powerDownMode_; //Vain resetti poistaa
		I8031_NameS::programCounter_Type programCounterValueInPowerSaveMode_;

		void createSFRBitNameTable(void);
		void createSFRNameTable(void);

};

#endif
