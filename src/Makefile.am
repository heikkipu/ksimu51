# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ksimu51.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ksimu51

# the application source, library search path, and link libraries
ksimu51_SOURCES = MyKApplication.cpp ksimu51.cpp main.cpp
ksimu51_LDFLAGS = $(KDE_RPATH) $(all_libraries)
ksimu51_LDADD = $(top_builddir)/src/components/libBaseComponents.la \
	$(top_builddir)/src/processorsW/libprocessorsW.la $(top_builddir)/src/share/libShare.la $(LIB_KDEUI) $(LIB_KIO) \
	$(LIB_KUTILS)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = ksimu51.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/ksimu51
shellrc_DATA = ksimu51ui.rc

SUBDIRS = share components processors processorsW
noinst_HEADERS = KSimu51_NameS.h MyKApplication.h ksimu51.h
