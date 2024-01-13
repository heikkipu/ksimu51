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
#ifndef EXTERNALW_H
#define EXTERNALW_H

#include "../KSimu51_NameS.h"
#include "../share/WorkspaceMainW.h"
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <klistbox.h>
#include <kpixmap.h>
#include <qworkspace.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qtoolbutton.h>

#include <string>
#include <functional>

#include "../components/ComponentsLib.h"
#include "../processors/I8031.h"
#include "../processors/AnalogInputPin.h"
#include "IOPortPin.h"
#include "../share/IndexedKDoubleNumInput.h"


class QWorkspace;
class QToolBar;
class QGrid;
class QGridLayout;

class KPopupMenu;
class KListBox;

class IndexedKDoubleNumInput;
class IOPortPin;


class ExternalW: public WorkspaceMainW
{
	Q_OBJECT
	public:

		ExternalW(I8031* processor, QWidget* parent, char const * name, int wflags);
		~ExternalW();

		//DATA muistiavaruuden jutut
		void writeExternalDataMemoryAreaTable(unsigned const int  address, unsigned const int  value);
		bool isExternalDataMemoryAreaTableValueChanged(unsigned int& minAddress, unsigned int& maxAddress);
		bool isExternalDataMemoryAreaTableValueChanged(unsigned const int  address);
		unsigned int readExternalDataMemoryAreaTable(unsigned const int  address);

		//Code alueelle kytketyn komponentin jutut
		bool isExternalCodeMemoryAreaTableValueChanged(unsigned int& minAddress, unsigned int& maxAddress);
		bool isExternalCodeMemoryAreaTableValueChanged(unsigned const int  address);
		unsigned int readExternalCodeMemoryAreaTable(unsigned const int  address);

		//Prosessorin pinnit pinni/portti toolbaarissa metodit.
		void addPins(IOPinList* pinList);
		void writePins(bool const  show);
		IOPin* readPinsList(void);
		IOPin* readPinsListSampleInputs(void); //Luetaan apupinnilista joka muuttuu vain external cyclen sampleInput hetkellä.
		//Prosessorin portit pinni/portti toolbaarissa metodit.
		void addPortPin(unsigned int bitAddress, string const pinName, bool const  value);//Lisätään pinni kerrallaan
		void writePort(unsigned char const  portAddress, unsigned char const  portValue, bool const  show); //Päivitetään tiedot porteittain.
		IOPortPin* readPortPinList(void);
		IOPortPin* readPortPinListSampleInputs(void); //Luetaan apuporttipinnilista joka muuttuu vain external cyclen sampleInput hetkellä.

		//Kaikentyyppisten komponenttien yhteiset jutut
		void updateAllComponents(const ExternalCycle* extCyc, double const  stepTime, I8031_NameS::machineCycles_Type const  machineCycles, I8031_NameS::clockTicks_Type const  clockTicks, I8031_NameS::programCounter_Type programCounterValue, bool const  show);
		void clearComponents(void);
		void resetComponents(void);
		//Datapointterit
		void setDatapointerValues(unsigned const int  dptrAddress, unsigned char const  r0Address, unsigned char const  r1Address);
		void showPointerDPTR(void);
		void showPointerR0(void);
		void showPointerR1(void);

		//Näitä tarvitaan kopioitaessa prosessori ja tallennuksessa.
		QWidgetList listOfComponents(void){return windowWorkspace_->windowList();};
		void addComponents(QWidgetList listOfComponents);
		IOPin* listOfNets(void){return netPinList_.pToFirstPin();};
		void addNets(IOPin* nets);


		//WorkSaveOpen
		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		I8031* pToProcessor_;
		void mousePressEvent(QMouseEvent *e) override;
		//void enterEvent ( QEvent *e);
		//void windowActivationChange(bool oldActive);

	signals:
		//void messageToProcessor8051W(const QString&, const int , const QString&, const int , const double );
		void message_MemoryValueChanged(const QString&, const unsigned long, const int );
		void message_ToggleBit(const QString&, const QString&, const int );
		void message_WindowLevel(const QString&, const int );
		void message_FocusProgramMemoryAddress(const QString&, const unsigned long );
		void message_Update(const QString& name);

	private slots:

		void ioPinsKListBoxDoubleClicked(QListBoxItem*, const QPoint&);
		void ioPinsKListBoxClicked (QListBoxItem*, const QPoint&);
		void portsKListBoxDoubleClicked(QListBoxItem* , const QPoint&);
		void portsKListBoxClicked(QListBoxItem* , const QPoint&);
		void netKListBoxDoubleClicked(QListBoxItem*, const QPoint&);
		void netKListBoxMouseButtonClicked(int, QListBoxItem*, const QPoint&);

		void showR0(void);
		void showR1(void);
		void showDPTR(void);

		void hideShowToolsButtons(void);

		void connectUnconnectPinsButtonPressed(void);
		void showComponentChangesWhenRunningButtonPressed(void);
		void showIOChangesWhenRunningButtonPressed(void);

		void componentActivated();

		void componentCreatedFromLibrary(ExternalComponentBaseW*);

		//void animateCycleButtonPressed(void);

		//void messagesFromComponents(const QString&, const int , const QString&, const int , const double );
		void messageFromComponent_Created(const QString& name);
		void messageFromComponent_Destroyed(const QString& name);
		void messageFromComponent_PinConnect(const QString& name, const int pinID, const QString& pinName);
		void messageFromComponent_PinUnconnect(const QString& name);
		void messageFromComponent_PinConnected(const QString& name, const int pinID, const QString& pinName);
		void messageFromComponent_PinUnconnected(const QString& name, const int pinID, const QString& pinName);
		void messageFromComponent_PinCopy(const QString& name, const int pinID);
		void messageFromComponent_MemoryValueChanged(const QString& name, const unsigned long address, const int value);
		void messageFromComponent_FocusProgramMemoryAddress(const QString& name, const unsigned long address);

	private:
		double lastStepTime_;
		I8031_NameS::machineCycles_Type lastMachineCycles_;
		I8031_NameS::clockTicks_Type lastClockTicks_;
		I8031_NameS::programCounter_Type programCounterValue_;

		//Prosu->ExData
		bool someDataPointerValueChanged_;
		unsigned int dptrValue_;
		unsigned int r0Value_; //8Bittinen osoite oikeesti
		unsigned int r1Value_; //8Bittinen osoite oikeesti
		unsigned int wAddress_;
		MemoryTable* externalDataMemoryAreaInT_;
		MemoryTable* externalDataMemoryAreaOutT_;
		MemoryTable* externalCodeMemoryAreaOutT_;   //Vain luettavissa oleva alue MOVC

		ExternalCycle externalCyc_;
		unsigned char doExternalCycleCounter_;
		void doExternalCycle(unsigned char cycleCount, bool show);

		void executeAndUpdateExternalAreaComponents(double const  stepTime, bool const  cycle, bool const  doNotExecute, bool const  show);

		void addComponentsPopupMenu(void);
		KPopupMenu* contextKPopupMenu;
		KPopupMenu* addKPopupMenu;

		//ToolBaari ylös
		KToolBar* toolsTools_;

		//IOPinnien ja porttien tool baarit ikkunan sivuille:
		void createIOPinsAndPortsToolBar(void);
		KToolBar* ioPinsTools_;
		KToolBar* netTools_;   //Net toolbaari erikseen

		//Pinnien ListBoxi
		static const int  PIN_START_ID=0;
		KListBox* ioPinsKListBox_;
		IOPinList* ioPinListProcessor_;
		IOPinList* ioPinListSampleInputsList_; //SampleInput hetkellä muuttuva yksittäinen inputpinnilista
		IOPinList* ioPinsCycleList_;   //Käytetään tätä animateCyclessä
		IOPinList* lastIOPinList_;
		void updatePins(bool const  processorPointer, bool const  show);

		//Porttien ListBoxi
		static const int  PORTPIN_START_ID=1000;   //Porttien pinnit indexi alkaa tuhannesta, 0-999 = yksittäiset pinnit
		KListBox* portsKListBox_;
		bool firstTimeToCreatePortPin_;
		int portPinCounter_;   //Incrementoituu pinnien luonnilla
		IOPortPin* portPinList_; //Porttipinnilistan eka jäsen.
		IOPortPin* portPinListLastMember_; //Porttipinnilistan viimeinen jäsen.
		IOPortPin* portPinListSampleInputs_; //SampleInput hetkellä muuttuva porttipinnilista eka jäsen.
		IOPortPin* portPinListSampleInputsLastMember_; //SampleInput hetkellä muuttuva porttipinnilistan viimeinen jäsen.
		void updatePort(unsigned char const  portAddress, unsigned char const  portValue, unsigned char const  updateMask, bool const  show);
		void copyPortPinsListValues(IOPortPin* destinationPinList, IOPortPin* sourcePinList, unsigned char const  fromPort, unsigned char const  toPort);

		//Netin ListBoxi
		static const int  NETPIN_START_ID=2000; //Komponentien välille luotava apupinnijärjestelmän startti
		KListBox* netKListBox_;
		bool netPinsCreated_;
		int netPinCounter_;   //Incrementoituu pinnien luonnilla
		IOPinList netPinList_;
		IOPinList* lastNetPinList_;    //Updatea varten
		//IOPin* netPinListLastMember;
		void addNetPin(IOPin::PinTYPE const pinType, bool const  value);
		void updateNets(void);
		void changeNetTextConnectingPin(const int  index, const QString& stringToAdd);
		void changeNetTextUnconnectingPin(const int  index, const QString& stringToRemove);

		//Porttien-, pinnien- ja net listan yhteinen päivitysrutiini. Päivittää kaiken.
		void updatePinKListBox(KListBox* listBox, IOPin* pinList);

		bool showComponentChangesWhenRunning_;
		bool showIOChangesWhenRunning_;

		int toolsTools_ConnectUnconnectPins_ID_;
		int toolsTools_ShowComponentChangesWhenRunning_ID_;
		int toolsTools_ShowIOChangesWhenRunning_ID_;

		//QToolButton* ioPinsTools_AnimateCycleButton;

		bool connectUnconnectPinsIsOn_;
		IOPin* nextPinToConnect_;      //osoitin pinniin joka kytketään seuraavana, jos 0 ei kytkettävää pinniä
		QString selectedConnectionTxt_;
		void connetNextPinToComponent(void);
		void unconnetComponentActivePin(void);
		void selectConnection(const QString &connectionText);

		void addComponent(unsigned long const  type, const QString& name);

		//Kaikki ulkoisen muistialueen komponentit on externalComponent luokan ilmentymiä
		ExternalComponentBaseW *pToExternalComponentW_; //Tällä osoitetaan niihin

		ComponentsLib *componentLibrary_;

		//Ikonit
		KPixmap ledOnIconKPixmap_;
		KPixmap ledOffIconKPixmap_;

		KPixmap addComponentIconKPixmap_;
		KPixmap connectUnconnectPinsIconKPixmap_;

		KPixmap R0MarkKPixmap_;
		KPixmap R1MarkKPixmap_;
		KPixmap dptrMarkKPixmap_;

		//KPixmap animateCycleIconKPixmap;
		KPixmap showComponentChangesWhenRunningKPixmap_;
		KPixmap showIOChangesWhenRunningKPixmap_;

		void activatePointerWindow(unsigned char const  pointer);
		enum pointerToActivate
		{
			ACTIVATE_DPTR,
			ACTIVATE_R0,
			ACTIVATE_R1,
		};
};


#endif
