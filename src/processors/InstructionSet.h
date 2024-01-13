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
#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

#include <string>
#include "InstructionSet_NameS.h"
#include "I8031_NameS.h"
#include "Global_NameS.h"

using namespace Global_NameS;

class Instruction
{
	public:
		Instruction();
		~Instruction();

		void setInstruction(char const *instName, char const *instMnemonic, I8031_NameS::machineCycles_Type const numCycles);
		I8031_NameS::machineCycles_Type readMachineCycles(void);
		void setMachineCycles(I8031_NameS::machineCycles_Type const mCycles);
		string readName(void);
		string readMnemonic(void);

	private:
		string name_;
		string mnemonic_;
		I8031_NameS::machineCycles_Type machineCycles_;
};

class InstructionSet
{
	public:
		InstructionSet();
		~InstructionSet();

		I8031_NameS::machineCycles_Type readMachineCycle(unsigned int const instructionValue);
		bool isInstruction(char const *instStr);
		void setNewMachineCycles(char const *mnemonicStr, int const numCycles);

	private:
		//Käskytaulukko.
		//Assebler käskyt, ei muuta järkeä, kuin voidaan kysyä löytyykö ko. käsky processorista.
		static unsigned int const INSTRUCTION_TABLE_SIZE=256;
		Instruction instructionTable_[INSTRUCTION_TABLE_SIZE];
		void createInstructions(void);
};


#endif
