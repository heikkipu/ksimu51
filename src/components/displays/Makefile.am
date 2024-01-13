INCLUDES = $(all_includes)
METASOURCES = AUTO
libdisplays_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libdisplays.la
SUBDIRS = Hitachi Maxim
libdisplays_la_LIBADD = \
	$(top_builddir)/src/components/displays/Maxim/libDisplaysMaxim.la $(top_builddir)/src/components/displays/Hitachi/libDisplaysHitachi.la
noinst_HEADERS = CharacterDotMatrixLCD.h DisplaysC.h
libdisplays_la_SOURCES = CharacterDotMatrixLCD.cpp DisplaysC.cpp
