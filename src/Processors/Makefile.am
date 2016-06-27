noinst_LIBRARIES = libprocessors.a

libprocessors_a_CPPFLAGS= $(requires_CFLAGS)
libprocessors_a_CPPFLAGS+= -Wall -g -O -std=c++11 -lpthread -D_REENTRANT -DHAVE_INLINE

libprocessors_a_SOURCES = Processor.hpp
libprocessors_a_SOURCES += Processor.cpp
libprocessors_a_SOURCES += TDSProcessor.hpp
libprocessors_a_SOURCES += TFSProcessor.hpp
libprocessors_a_SOURCES += ProcessorVector.hpp
libprocessors_a_SOURCES += inputProc.hpp
libprocessors_a_SOURCES += inputProc.cpp
