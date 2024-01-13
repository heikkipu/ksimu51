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
#ifndef I51BASEINTERNALROMW_H
#define I51BASEINTERNALROMW_H

#include "I51BaseW.h"
#include "DisassembleW.h"
#include "AssemblerSourceW.h"

class DisassembleW;
class AssemblerSourceW;
class I51BaseW;

class I51BaseInternalROMW : virtual public I51BaseW
{
	Q_OBJECT
	public:
		I51BaseInternalROMW(QWidget* parent, char const * name, int wflags, int const  index, int const  type);
		~I51BaseInternalROMW();

		bool step(bool const  disassemble, double const  stepTime) override;
		void powerOnReset(void) override;
		void showPointerPC(void) override;

		void loadProgramAndCreateWindows(const QString& hexFilename) override;
		QString readEAPinPointingProgramMemoryFilename(void) const override;

	protected:
		void updateProgramMemoryWindows(void) override;
		void updateProcessorWindows_VirtualPart(void) override;
		void createDynamicWindows_VirtualPart(void) override;

		void copy_VirtualPart(I51BaseW* sourceProcessorW) override;

		bool isAtBreakpoint_VirtualPart(void) override;
		void showBreakpoint_VirtualPart(void) override;

		void setCaptionAndIconToProgramMemoryWs(KPixmap const &icon) override;

		void messageFromWorkspaceW_SetExecutionPoint_VirtualPart(const QString& name, const unsigned long address) override;
		void messageFromWorkspaceW_ToggleBreakpointBit_VirtualPart(const QString& name, int const  bitAddress, bool const bitValue) override;
		void messageFromWorkspaceW_FocusProgramMemoryAddress_VirtualPart(const QString& name, const unsigned long address) override;

		RomW* internal_romw_;

		void saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream) override;
		void readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
										  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;
		void rewriteProgramMemory(void) override;

	private:
		QString internalProgramMemoryHexFilename_;
		std::vector<std::tuple<unsigned int, unsigned char>> internalLoadedProgram_;

		DisassembleW* internal_disassemblew_;
		AssemblerSourceW* internal_assemblersourcew_;

		bool isProgramCounterPointingInternalProgramMemory(void);

		#define WINDOWNAME_INTERNAL_DISASSEMBLE "internalDisassembleW"
		#define WINDOWNAME_INTERNAL_ASM_SOURCE "internalASMSourceW"
		#define WINDOWNAME_INTERNAL_ROM "internalROMW"
};

#endif
