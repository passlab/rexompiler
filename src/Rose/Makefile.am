# This Makefile is only for things in the ::Rose namespace. If
# you're adding something that's not in ::Rose you're doing it wrong!

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc subdir-objects

SUBDIRS=AST CodeGen

# LicenseString.h is included by CommandLine/License.C but is not installed.
#
CommandLine/LicenseString.h: $(top_srcdir)/LicenseInformation/ROSE_BSD_License.txt
	mkdir -p CommandLine
	perl $(top_srcdir)/scripts/to-c-string.pl < $< > $@
BUILT_SOURCES = CommandLine/LicenseString.h

# .C files for the ::Rose namespace (see top comment)
AM_CPPFLAGS = $(ROSE_INCLUDES)
noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES =								\
	Color/Color.C									\
	CommandLine/DurationParser.C							\
	CommandLine/License.C								\
	CommandLine/Parser.C								\
	CommandLine/SelfTest.C								\
	CommandLine/Version.C								\
	Diagnostics/Diagnostics.C							\
	FileSystem/FileSystem.C								\
	FormattedTable.C								\
	Progress.C									\
	StringUtility/Convert.C								\
	StringUtility/Diagnostics.C							\
	StringUtility/Escape.C								\
	StringUtility/NumberToString.C							\
	StringUtility/Predicate.C							\
	StringUtility/SplitJoin.C							\
	StringUtility/StringToNumber.C							\
	Yaml.C

# Public header files are installed from parent makefiles.
