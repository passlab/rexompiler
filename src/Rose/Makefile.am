# This Makefile is only for things in the ::Rose namespace. If
# you're adding something that's not in ::Rose you're doing it wrong!

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc subdir-objects

SUBDIRS=AST

# .C files for the ::Rose namespace (see top comment)
AM_CPPFLAGS = $(ROSE_INCLUDES)
noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES =								\
	StringUtility/Convert.C								\
	StringUtility/Escape.C								\
	StringUtility/NumberToString.C							\
	StringUtility/Predicate.C							\
	StringUtility/SplitJoin.C							\
	StringUtility/StringToNumber.C

# Public header files are installed from parent makefiles.
