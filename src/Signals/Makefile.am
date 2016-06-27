noinst_LIBRARIES = libsignals.a

libsignals_a_CPPFLAGS= $(requires_CFLAGS)
libsignals_a_CPPFLAGS+= -Wall -g -O -std=c++11 -D_REENTRANT -DHAVE_INLINE

libsignals_a_SOURCES = dataType.hpp
libsignals_a_SOURCES += circularContainer.hpp
libsignals_a_SOURCES += Signal.hpp Signal.cpp
libsignals_a_SOURCES += TimeDomainSignal.hpp
libsignals_a_SOURCES += TimeFrequencySignal.hpp
