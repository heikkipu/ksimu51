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
//Base class
#ifndef EXTERNALCOMPONENTW_H
#define EXTERNALCOMPONENTW_H

#include <string>
#include <cstdlib>

#include "../share/ShareMainW.h"

#include <kpixmap.h>
#include <klocale.h>
#include <klistbox.h>
#include <ktoolbar.h>
#include <knuminput.h>

#include <qtable.h>
#include <qcheckbox.h>
#include <qtooltip.h>

//Käytetään jotain prosessoriluokan sisäisiä luokkia
#include "../processors/I8031_NameS.h"
#include "../processors/IOPin.h"
#include "../processors/Memory.h"
#include "../processors/IOPinList.h"
#include "../processors/ExternalCycle.h"


//Perusluokka ulkoisen muisti- ja IO- avaruuden komponenteille.
class ExternalComponentBaseW : public ShareMainW
{
	Q_OBJECT
	public:
		ExternalComponentBaseW(QWidget* parent, char const * name, int wflags, unsigned const int  componentNumber, unsigned const int  componentType);
		ExternalComponentBaseW(QWidget* parent, char const * name, int wflags, unsigned const int  componentNumber, unsigned const int  componentType, unsigned const int  externalDataSize);
		ExternalComponentBaseW(QWidget* parent, char const * name, int wflags, unsigned const int  componentNumber, unsigned const int  componentType, unsigned const int  externalDataSize, unsigned const int  externalCodeSize);
		~ExternalComponentBaseW();

		const unsigned int readType(void) const {return type_;};
		const QString *readName(void) const {return &componentName_;};
		void setPlainCaption( const QString &caption ) override; //UM viiveen näyttämistä varten.

		virtual void clear(void);
		void reset(void);

		//Ulkoisen muistiavaruuden liityntä
		bool isAtExternalDataSpace(void){return atExternalDataSpace_;};
		unsigned int readExternalDataSpaceStartAddress(void) const {return externalDataSpaceStartAddress_;};
		unsigned long readExternalDataSpaceSize(void);
		virtual void writeExternalDataSpaceStartAddress(unsigned const int  address){externalDataSpaceStartAddress_=address;};
		virtual void setExternalDataSpaceValue(unsigned const int  address, unsigned char const  value);
		void writeExternalDataSpaceValue(unsigned const int  address, unsigned char const  value, bool const  changedAfterClearBit); //Ei muuta change bittiä, mutta voidaan muuttaa change after clear.
		bool isExternalDataSpaceValueChanged(unsigned const int  address);
		bool isExternalDataSpaceValueChanged(unsigned int& minAddress, unsigned int& maxAddress);
		unsigned char readExternalDataSpaceValue(unsigned const int  address);

		//Ulkoisen CODE spacen liityntä.
		bool isAtExternalCodeSpace(void){return atExternalCodeSpace_;};
		unsigned int readExternalCodeSpaceStartAddress(void) const {return externalDataSpaceStartAddress_;};
		unsigned long readExternalCodeSpaceSize(void);
		virtual void writeExternalCodeSpaceStartAddress(unsigned const int  address){externalCodeSpaceStartAddress_=address;};
		bool isExternalCodeSpaceValueChanged(unsigned const int  address);
		bool isExternalCodeSpaceValueChanged(unsigned int& minAddress, unsigned int& maxAddress);
		unsigned char readExternalCodeSpaceValue(unsigned const int  address);


		//I/O liityntä
		bool isAtIOSpace(void){return atIOSpace_;}; //Palauttaa tiedon siitä onko kytkettävissä IO pinneihin.
		IOPinList* readIOPinsList(void);
		void copyIOPins(IOPinList* listToWrite);   //Tarvitaan kopioitaessa.
		void writeInputPinValue(QString& ioPinName, bool const  value, bool const  setFlag);
		void writeInputPinValue(IOPin* pointerToPin, bool const  value, bool const  setFlag);
		//double readDelayTime(void){return delayTime;};
		//Pinnien kytkentä
		void connectPin(IOPin* pinToConnect);
		void unconnectPin(void);   //Unkonnektoitava on pinlistan aktiivinen pinni
		void showConnectedPin(const QString &connectedPinTextToShow);
		
		//Datapointterien liityntä
		//Viedään pointterit, tarttee ja pitää tehdä vain kerran komponentin luonnin jälkeen
		void writePointers(ExternalCycle* pToExternalCycle, bool* pToSomeDatapointerValueChanged, unsigned int* pToDPTRValue, unsigned int* pToR0Value, unsigned int* pToR1Value, double* pToLastStepTime, I8031_NameS::machineCycles_Type *pToLastMachineCycles, I8031_NameS::programCounter_Type *pToProgramCounterValue);
		void setDatapointers(void); //Asettaa pointterit
		virtual void showPointerDPTR(void);
		virtual void showPointerR0(void);
		virtual void showPointerR1(void);

		//Päivitetään ikkunan tiedot näkyviin.
		void updateComponent(bool const  updateIO);   //update_VirtualPart on virtuaaliosa perivien luokkien määritykseen
		//Tehdään kaikki mitä komponentti tekee ajoitettuna steppiaikaan.
		double executeComponent(double executeTime); //execute_VirtualPart on virtuaaliosa periville luokille

		bool hasChanges(void);

		//työtilanteen tallennus ja luku.
		//Määritellään joka komponentille erikseen jotta toimisi oikein
		//Perivän komponentin on kutsuttava kantaluokan tätä jotta perusjutut tallentuu.
		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream,
								  std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

		QSize getMaximumSize(void) override;

	protected:

		#define FONTFAMILY "MonoSpace"

		QString componentName_;
		unsigned int type_;   //Komponentin tyyppi
		unsigned int number_;

		void clearDatapointers(void);
		virtual void updateDatapointers(void){;}; //UM saattaa vaihdella toteutustapa komponenteittain

		virtual void reset_VirtualPart(void){};
		
		virtual void clearExternalDataSpace(void);
		virtual void clearExternalCodeSpace(void);

		//Päivitetään ikkunan tiedot näkyviin.
		virtual void update_VirtualPart(void){;};//updaten virtuaaliosa
		//Tehdään kaikki mitä komponentti tekee ajoitettuna steppiaikaan.
		virtual double execute_VirtualPart(double executeTime){return executeTime+1.0;};//Exec virtuaaliosa
		double execute_VirtualPart_ExecuteInputChange(double executeTime);//Standardi toiminto inputin muutoksen mukaan toimimiseen. Käytä execute_VirtualPartissa
		virtual void execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart(void){;};//Käytetään edellisen kanssa. Kutsuu kun execution valmis.

		bool isExecuting_;
		double executionStartStepTime_; //Toiminnan aloituksen askellusaika. Jos pitää laskea viiveitä.
		double executionReadyTime_;

		//Viiveajat sekunneissa.
		double delayTime_; //Käytetään viiveen laskemiseen tätä: = delayTimeTyp+delayTimeAddRandom
		double delayTimeTyp_;   //Komponentin tyypillinen viive executionin alusta. Komponenteissa josta voidaan muutttaa viivettä
		double delayTimeRandom_; //Satunnaislukulisä delayTimeen. Antaa luonnollista epämääräisyyttä.
		static double constexpr DEFAULT_DELAYTIME_TYP = 0.000001; //Oletusarvo komponentin viiveelle. 10 mikrosekuntia.
		inline void delayTimeCalculate(void) {
			delayTime_ = delayTimeTyp_ + delayTimeRandom_;
			setPlainCaption(caption());
		}

		KDoubleNumInput* delayTimeTypNumInput_; //Jos ei halua käyttää tätä: deletoi ja anna sille arvo 0 perivässä luokassa.
		void randomizeDelayTime(double maxNanoS); //Arvotaan lisäviive kymmenissä pikosekunneissa.%&%&%& random väli nanosekunneissa 5.0 = 0.0-5.0ns
		KDoubleNumInput* delayTimeRandomNumInput_;

		externalDataMemoryTable* externalDataSpaceValueT_; //Data muistavaruuden arvotaulukko
		externalDataMemoryTable* externalCodeSpaceValueT_; //Code muistavaruuden arvotaulukko
		void setExternalCodeSpaceValue(unsigned const int  address, unsigned char const  value);   //Vain perivien luokkien käyttöön

		bool memoryAreaChanged_;
		bool changes_;   //On muutoksia jotka täytyy päivittää executen jälkeen.

		IOPinList ioList_;   //I/O Pinnien lista
		KToolBar* leftKToolBar_;
		KToolBar* rightKToolBar_;
		KListBox* inputPinKListBox_; //inputPinnien ilmentymä lista
		KListBox* outputPinKListBox_; //outputtien ilmentymälista
		KListBox* bidirPinKListBoxInLeftKToolBar_; //kaksisuuntasten ilmentymälista
		KListBox* bidirPinKListBoxInRightKToolBar_; //kaksisuuntasten ilmentymälista
		void addInputPin(char const *pinName);
		void addOutputPin(char const *pinName);
		void addBidirPinInLeftKToolBar(char const *pinName);
		void addBidirPinInRightKToolBar(char const *pinName);
		int inputPinCounter_;
		int outputPinCounter_;
		int bidirPinCounterInLeftKToolBar_;
		int bidirPinCounterInRightKToolBar_;

		bool atExternalDataSpace_; //On liityntä muistiavaruuteen DATA, ADDRESS, CS. Luetaan/Kirjoitetaan MOVX käskyllä.
		bool atExternalCodeSpace_; //On liityntä ulkoiseen muistiavaruuteen. Vain luku MOVC käskyllä. PSEN aktiivi.
		bool atIOSpace_;      //On I/O pinnejä jotka vaikuttavat toimintaan.

		unsigned int externalDataSpaceStartAddress_; //Data muistiavaruuden alkuosoite
		unsigned int externalCodeSpaceStartAddress_; //Code muistiavaruuden alkuosoite

		bool isDPTR_;
		bool isR0_;
		bool isR1_;
		//Pointterit ExternalW luokan tietoihin
		bool *pToSomeDatapointerValueChanged_;
		unsigned int *pToDPTRValue_;
		unsigned int *pToR0Value_;
		unsigned int *pToR1Value_;
		double *pToLastStepTime_;
		I8031_NameS::machineCycles_Type *pToLastMachineCycles_;
		I8031_NameS::programCounter_Type *pToProgramCounterValue_;

		KPixmap dptrMark_;
		KPixmap r0Mark_;
		KPixmap r1Mark_;
		KPixmap dptrR0Mark_;
		KPixmap dptrR1Mark_;
		KPixmap r0r1Mark_;
		KPixmap dptrR0R1Mark_;

		QSize maxWSize_; //Tällä tuodaan perivän komponentit maksimikoko. Asetetaan konstruktorissa.

		void closeEvent(QCloseEvent *e) override;
		void mousePressEvent(QMouseEvent * me) override
		{
			if(me->button()==Qt::RightButton)
				int a = 0;
			ShareMainW::mousePressEvent(me);
		}

		//Voidan määritellä perivissä luokissa: Jos hiiren oikeaa painiketta painettu IO- listboksin tyhjässä kohdassa.
		virtual void inputPinListEmptyItemRightMouseButtonClicked(void){;};
		virtual void outputPinListEmptyItemRightMouseButtonClicked(void){;};
		virtual void bidirPinListEmptyItemRightMouseButtonClickedInLeftKToolBar(void){;};
		virtual void bidirPinListEmptyItemRightMouseButtonClickedInRightKToolBar(void){;};

protected slots:
		void delayTimeTypValueChanged(double timeNanoS);
		void delayTimeRandomValueChanged(double timeNanoS);

signals:
		//void messageToEDMW(const QString&, const int , const QString&, const int , const double  );
		void message_Created(const QString& );
		void message_Destroyed(const QString& );
		void message_PinConnect(const QString&, const int , const QString& );
		void message_PinUnconnect(const QString& );
		void message_PinConnected(const QString&, const int , const QString& );
		void message_PinUnconnected(const QString&, const int , const QString& );
		void message_PinCopy(const QString&, const int );
		void message_MemoryValueChanged(const QString&, const unsigned long, const int);
		void message_FocusProgramMemoryAddress(const QString&, unsigned long const );

	private slots:
		void inputPinListMouseButtonClicked(int button, QListBoxItem* item, const QPoint & pos);
		void outputPinListMouseButtonClicked(int button, QListBoxItem* item, const QPoint & pos);
		void bidirPinListMouseButtonClickedInLeftKToolBar(int button, QListBoxItem* item, const QPoint & pos);
		void bidirPinListMouseButtonClickedInRightKToolBar(int button, QListBoxItem* item, const QPoint & pos);

	private:
		void constructorsCommon(unsigned const int  componentNumber, unsigned const int  componentType);

		IOPin::PinID pinToConnectUnconnectID_;    //ID: 0-4999 = inputti, 5000-9999 = outputti, 10000-14999= bidirLeft 15000-= bidirRight, -1= ei pinniä
		void addPin(char const * pinName, IOPin::PinTYPE const pinType);

		ExternalCycle* pToExternalCycle_; //Tieto externalCyclestä executea varten, Edellinen!

		KPixmap inputLedOnIcon_;
		KPixmap inputLedOffIcon_;
		KPixmap outputLedOnIcon_;
		KPixmap outputLedOffIcon_;
		KPixmap hizLedIcon_;

		bool firsTimeToExecute_;

//---------------------------------- VAKIOT YMS. -----------------------------------
	public:
		static const int  INPUTPIN_ID_OFFSET=0;
		static const int  OUTPUTPIN_ID_OFFSET=5000;
		static const int  BIDIRPIN_ID_OFFSET_LEFT=10000;
		static const int  BIDIRPIN_ID_OFFSET_RIGHT=15000;


		//Komponentti pointterit
		enum componentPointerToActivate
		{
			ACTIVATE_DPTR,
			ACTIVATE_R0,
			ACTIVATE_R1
		};
};

#endif
