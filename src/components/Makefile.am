INCLUDES = $(all_includes)
METASOURCES = AUTO
libBaseComponents_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libBaseComponents.la

SUBDIRS = displays instruments keysButtons logic74 logicGALPAL memory misc
libBaseComponents_la_LIBADD = \
	$(top_builddir)/src/components/displays/libdisplays.la $(top_builddir)/src/components/instruments/libInstruments.la \
	$(top_builddir)/src/components/keysButtons/libKeysButtons.la $(top_builddir)/src/components/logic74/libLogic74.la \
	$(top_builddir)/src/components/memory/libMemory.la $(top_builddir)/src/components/misc/libmiscComponents.la \
	$(top_builddir)/src/components/logicGALPAL/liblogiGALPAL.la
noinst_HEADERS = Components.h ComponentsLib.h ExternalComponentBaseW.h
libBaseComponents_la_SOURCES = Components.cpp ComponentsLib.cpp \
	ExternalComponentBaseW.cpp
