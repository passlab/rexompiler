include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

# Tell automake install to copy this library
noinst_LTLIBRARIES = libRoseStringUtility.la
libRoseStringUtility_la_SOURCES =  \
	Convert.C								\
	Escape.C								\
	FileNameClassifier.C								\
	FileUtility.C								\
	NumberToString.C							\
	Predicate.C							\
	Replace.C							\
	SplitJoin.C							\
	StringToNumber.C

pkginclude_HEADERS = \
	BitOps.h							\
	Constants.h						\
	StringUtility.h						\
	Convert.h					\
	Escape.h					\
	FileUtility.h								\
	NumberToString.h				\
	Predicate.h					\
	Replace.h					\
	SplitJoin.h					\
	StringToNumber.h

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.loT *.int.c

distclean-local:
	rm -rf Templates.DB

EXTRA_DIST = CMakeLists.txt stringSupportDocumentation.docs