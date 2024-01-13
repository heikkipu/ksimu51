INCLUDES = $(all_includes)
METASOURCES = AUTO
SUBDIRS = Atmel Maxim
libBaseProcessors_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libBaseProcessors.la
noinst_HEADERS = AnalogInputPin.h AnalogInputPinList.h ExternalCycle.h \
	Global_NameS.h I8031.h I8031_ADConverter.h I8031_NameS.h I8031_Xram.h I8032.h \
	I8032_NameS.h I8051.h I8051_ExternalCycle.h I8051_Flash.h I8051_NameS.h I8052.h \
	I80C31.h I80C31_NameS.h I80C32.h I80C32_NameS.h I80C51.h I80C52.h I80C52_NameS.h \
	IOPin.h IOPinList.h InstructionSet.h InstructionSet_NameS.h InternalRAM.h \
	Memory.h SerialPort.h TimersCounters.h TimersCounters2.h
libBaseProcessors_la_LIBADD = \
	$(top_builddir)/src/processors/Maxim/libMaximProcessors.la $(top_builddir)/src/processors/Atmel/libAtmelProcessors.la
libBaseProcessors_la_SOURCES = AnalogInputPin.cpp AnalogInputPinList.cpp \
	ExternalCycle.cpp I8031.cpp I8031_ADConverter.cpp I8031_Xram.cpp I8032.cpp I8051.cpp \
	I8051_ExternalCycle.cpp I8051_Flash.cpp I8052.cpp I80C31.cpp I80C32.cpp I80C51.cpp I80C52.cpp \
	IOPin.cpp IOPinList.cpp InstructionSet.cpp InternalRAM.cpp Memory.cpp \
	SerialPort.cpp TimersCounters.cpp TimersCounters2.cpp
