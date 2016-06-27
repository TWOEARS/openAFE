SUBDIRS = tools Signals Filters Processors
AUTOMAKE_OPTIONS = subdir-objects

@LTLIBOBJS@
lib_LTLIBRARIES = libopenAFE.la

libopenAFE_la_CPPFLAGS= $(requires_CFLAGS)
libopenAFE_la_CPPFLAGS+= -Wall -g -O -std=c++11 -lpthread -D_REENTRANT -DHAVE_INLINE
libopenAFE_la_LDFLAGS=	$(requires_LIBS)
libopenAFE_la_LDFLAGS+=	-release $(PACKAGE_VERSION)

libopenAFE_la_SOURCES = tools/window.cpp
libopenAFE_la_SOURCES += Signals/Signal.cpp
libopenAFE_la_SOURCES += Filters/bwIIR.cpp
libopenAFE_la_SOURCES += Filters/bwCoef.cpp
libopenAFE_la_SOURCES += Filters/bwFilter.cpp
libopenAFE_la_SOURCES += Filters/GammatoneFilter.cpp
libopenAFE_la_SOURCES += Processors/Processor.cpp
libopenAFE_la_SOURCES += Processors/inputProc.cpp

libopenAFE_la_LIBADD = tools/libtools.a
libopenAFE_la_LIBADD += Signals/libsignals.a
libopenAFE_la_LIBADD += Filters/libfilters.a
libopenAFE_la_LIBADD += Processors/libprocessors.a

toolsdir=$(pkgincludedir)/tools
tools_HEADERS = tools/mathTools.hpp
tools_HEADERS += tools/window.hpp

signaldir=$(pkgincludedir)/Signals
signal_HEADERS = Signals/dataType.hpp
signal_HEADERS += Signals/circularContainer.hpp
signal_HEADERS += Signals/Signal.hpp
signal_HEADERS += Signals/TimeDomainSignal.hpp
signal_HEADERS += Signals/TimeFrequencySignal.hpp

filterdir=$(pkgincludedir)/Filters
filter_HEADERS = Filters/GenericFilter.hpp
filter_HEADERS += Filters/CascadeFilters.hpp
filter_HEADERS += Filters/GammatoneFilter.hpp
filter_HEADERS += Filters/bwIIR.hpp
filter_HEADERS += Filters/bwCoef.hpp
filter_HEADERS += Filters/bwFilter.hpp

processordir=$(pkgincludedir)/Processor
processor_HEADERS = Processors/Processor.hpp
processor_HEADERS += Processors/TDSProcessor.hpp
processor_HEADERS += Processors/TFSProcessor.hpp
processor_HEADERS += Processors/ProcessorVector.hpp
processor_HEADERS += Processors/inputProc.hpp
