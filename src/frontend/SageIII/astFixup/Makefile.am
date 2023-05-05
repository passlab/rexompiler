include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastFixup.la

libastFixup_la_SOURCES      = \
     AstFixup.C \
     edge_ptr_repl.C \
     fixupforGnuBackendCompiler.C \
     fixupInClassDataInitialization.C \
     removeInitializedNamePtr.C \
     fixupDeclarations2.C \
     fixupFunctionDefinitions.C \
     fixupStorageAccessOfForwardTemplateDeclarations.C \
     fixupSourcePositionInformation.C \
     fixupEnumValues.C \
     fixupFriendTemplateDeclarations.C \
     fixupPrettyFunction.C


libastFixup_la_LIBADD       = 
libastFixup_la_DEPENDENCIES = 

pkginclude_HEADERS = \
     AstFixup.h \
     edge_ptr_repl.h \
     fixupFunctionDefinitions.h \
     fixupStorageAccessOfForwardTemplateDeclarations.h \
     removeInitializedNamePtr.h \
     fixupforGnuBackendCompiler.h \
     fixupInClassDataInitialization.h \
     fixupTemplateDeclarations.h \
     fixupSourcePositionInformation.h \
     fixupEnumValues.h \
     fixupFriendTemplateDeclarations.h \
     fixupPrettyFunction.h

EXTRA_DIST = CMakeLists.txt
