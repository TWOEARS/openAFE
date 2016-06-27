noinst_LIBRARIES = libmatFiles.a

libmatFiles_a_CPPFLAGS= $(requires_CFLAGS)
libmatFiles_a_CPPFLAGS+= -I/usr/local/MATLAB/R2015a/extern/include -L/usr/local/MATLAB/R2015a/bin/glnxa64 -o target -lmat -lmx -Wl,-rpath=/usr/local/MATLAB/R2015a/bin/glnxa64

libmatFiles_a_SOURCES = matFiles.cpp matFiles.hpp
