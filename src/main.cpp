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

#include <qdir.h>

#include "ksimu51.h"
#include "MyKApplication.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kpixmap.h>


static char const  description[] =I18N_NOOP("A 8051 Simulator program");

static char const  version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	//KAboutData about("ksimu51", I18N_NOOP("KSimu51"), version, description, KAboutData::License_GPL, "(C) 2006 Heikki Pulkkinen\nSärkimäentie 43\n51720 RAHULA\nFINLAND", 0, 0, "heikki.pulkkinen@surffi.fi");
	KAboutData about("ksimu51", I18N_NOOP("KSimu51"), version, description, KAboutData::License_GPL, "(C) 2006 Heikki Pulkkinen\n\n\nFINLAND", 0, 0, "heikki.pulkkinen@");

	//QDir::setCurrent("/home/heikki/pr/Ohjelmat/Kdevel/ksimu51/debug/src");

	QImage image;
	image.load ("pics/KS51hi32Intel8051.png");
	about.setProgramLogo (image);
	about.addAuthor( "Heikki Pulkkinen", 0, "heikki@x" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	MyKApplication app;

	KSimu51 *mainWin = 0;
	if (app.isRestored())
	{
		RESTORE(KSimu51);
	}
	else
	{
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

		/// @todo do something with the command line args here

		KPixmap mainIcon;
		mainIcon.load ( "pics/KS51hi32Intel8051.png", 0, KPixmap::Auto );
		mainWin = new KSimu51();
		app.setMainWidget( mainWin );
		mainWin->setIcon(mainIcon);

		mainWin->show();
		args->clear();
		mainWin->loadPrevWork();

}

	// mainWin has WDestructiveClose flag by default, so it will delete itself.
	return app.exec();
}

