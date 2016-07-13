openAFE
===========================

openAFE is a C++ library to extract a subset of common auditory representations from a binaural recording or from a stream of binaural audio data, based on the [Two!Ears Auditory Front-End](https://github.com/TWOEARS/auditory-front-end)

## Installation

* If you have an other version of Matlab rather than the 2015a, please modify the Matlab path in the examples/Makefile.am file.
* Please be sure that this path is included to the LD_LIBRARY_PATH.

```Shell
>> cd openAFE
>> mkdir build
>> cd build
>> ../configure
>> make
>> sudo make install
```

If you don't have Matlab at all :

The demo examples uses .mat files as input, so they need Matlab to be installed on your computer. If you don't have Matlab, you can still install the library without installing the DEMOs. To do so, please do the following modifications before the install :

in Makefile.am file :
change the 

```Makefile
SUBDIRS = src examples
```
to
```Makefile
SUBDIRS = src
```

in configure.ac file :
change the

```Makefile
AC_CONFIG_FILES([Makefile src/Makefile src/tools/Makefile src/Signals/Makefile src/Filters/Makefile src/Processors/Makefile examples/Makefile])
'''
to
```Makefile
AC_CONFIG_FILES([Makefile src/Makefile src/tools/Makefile src/Signals/Makefile src/Filters/Makefile src/Processors/Makefile])
```

## Examples
Please check the [examples](examples/README.md) directory to see how to use DEMO files.
