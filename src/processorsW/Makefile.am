INCLUDES = $(all_includes)
METASOURCES = AUTO
SUBDIRS = atmelW intelW maximW philipsW texasInstrumentsW
noinst_LTLIBRARIES = libprocessorsW.la
noinst_HEADERS = AssemblerSourceW.h BitAddressableAreaW.h BitChangeBreakpoint.h \
	DisassembleW.h ExternalW.h I51BaseADConverterW.h I51BaseInternalFLASHW.h \
	I51BaseInternalIRAMW.h I51BaseInternalROMW.h I51BaseInternalXRAMW.h I51BaseTimer2W.h \
	I51BaseW.h IOPortPin.h InternalRAMW.h InternalXRAMW.h MiscW.h ProcessorsWC.h \
	ProcessorsWLib.h RegisterBankAreaW.h RegsW.h RomW.h SFRBitW.h SfrW.h
libprocessorsW_la_SOURCES = AssemblerSourceW.cpp BitAddressableAreaW.cpp \
	BitChangeBreakpoint.cpp DisassembleW.cpp ExternalW.cpp I51BaseADConverterW.cpp \
	I51BaseInternalFLASHW.cpp I51BaseInternalIRAMW.cpp I51BaseInternalROMW.cpp \
	I51BaseInternalXRAMW.cpp I51BaseTimer2W.cpp I51BaseW.cpp IOPortPin.cpp InternalRAMW.cpp \
	InternalXRAMW.cpp MiscW.cpp ProcessorsWC.cpp ProcessorsWLib.cpp RegisterBankAreaW.cpp \
	RegsW.cpp RomW.cpp SFRBitW.cpp SfrW.cpp
libprocessorsW_la_LIBADD = $(top_builddir)/src/share/libShare.la \
	$(top_builddir)/src/processors/libBaseProcessors.la $(top_builddir)/src/processorsW/atmelW/libatmelW.la \
	$(top_builddir)/src/processorsW/intelW/libintelW.la $(top_builddir)/src/processorsW/maximW/libmaximW.la \
	$(top_builddir)/src/processorsW/philipsW/libphilipsW.la \
	$(top_builddir)/src/processorsW/texasInstrumentsW/libtexasInstrumentsW.la $(top_builddir)/src/components/libBaseComponents.la \
	$(top_builddir)/src/processors/Maxim/libMaximProcessors.la $(top_builddir)/src/processors/Atmel/libAtmelProcessors.la \
	$(top_builddir)/src/components/misc/libmiscComponents.la $(top_builddir)/src/components/memory/libMemory.la \
	$(top_builddir)/src/components/logicGALPAL/liblogiGALPAL.la $(top_builddir)/src/components/logic74/libLogic74.la \
	$(top_builddir)/src/components/keysButtons/libKeysButtons.la $(top_builddir)/src/components/instruments/libInstruments.la \
	$(top_builddir)/src/components/displays/Maxim/libDisplaysMaxim.la $(top_builddir)/src/components/displays/Hitachi/libDisplaysHitachi.la \
	$(top_builddir)/src/components/displays/libdisplays.la
