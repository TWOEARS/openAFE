# We don't want to follow GNU file setup (no README, no AUTHORS, etc)
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -g -O -std=c++11 -lpthread -D_REENTRANT -DHAVE_INLINE

# We have two dirs with code and (Makefile.am's) in them
SUBDIRS = my_inc src examples
