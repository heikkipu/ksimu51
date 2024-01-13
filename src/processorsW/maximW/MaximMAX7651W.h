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
#ifndef MAXIMMAX7651W_H
#define MAXIMMAX7651W_H

#include "../I51BaseW.h"
#include "../I51BaseInternalIRAMW.h"
#include "../I51BaseADConverterW.h"
#include "../I51BaseInternalFLASHW.h"
#include "../I51BaseTimer2W.h"


#include "../../processors/Maxim/MAX7651.h"
class MAX7651;

class MaximMAX7651W :  virtual public I51BaseW, public I51BaseInternalIRAMW, public I51BaseADConverterW, public I51BaseInternalFLASHW, public I51BaseTimer2W
{
   Q_OBJECT
   public:
      MaximMAX7651W(QWidget* parent, char const * name, int wflags, int const  index, int const  type);
      ~MaximMAX7651W();
   
   protected:
      void updateProcessorWindows_VirtualPart(void) override;
		void createStaticWindows_VirtualPart(void) override;
      
      void messageFromWorkspaceW_MemoryValueChanged_VirtualPart(const QString& name, const unsigned long address, const int value) override;

      void saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream) override;
      void readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
                                        std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;
      
};

#endif
