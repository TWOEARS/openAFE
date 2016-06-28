bin_PROGRAMS = TEST_input TEST_preProc TEST_filterbank

MATLAB_DIR = '/usr/local/MATLAB/R2015a'
MATFILESLIB_LIBS = -lmat -lmx

AM_CXXFLAGS=$(CPPFLAGS)
AM_CXXFLAGS+= -I$(MATLAB_DIR)/extern/include -L$(MATLAB_DIR)/bin/glnxa64 -Wl,-rpath=$(MATLAB_DIR)/bin/glnxa64


noinst_LIBRARIES = libmatFiles.a
libmatFiles_a_SOURCES = matFiles.hpp
libmatFiles_a_SOURCES += matFiles.cpp 

TEST_input_SOURCES = TEST_input.cpp
TEST_input_LDADD = ../src/libopenAFE.la libmatFiles.a $(MATFILESLIB_LIBS)

TEST_preProc_SOURCES = TEST_preProc.cpp
TEST_preProc_LDADD = ../src/libopenAFE.la libmatFiles.a $(MATFILESLIB_LIBS)

TEST_filterbank_SOURCES = TEST_filterbank.cpp
TEST_filterbank_LDADD = ../src/libopenAFE.la libmatFiles.a $(MATFILESLIB_LIBS)
