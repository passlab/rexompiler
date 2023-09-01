include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

###############################################################################################################################
# AstAttributeMechanism
###############################################################################################################################

noinst_PROGRAMS += attributeTests
attributeTests_SOURCES = attributeTests.C

TEST_TARGETS += attributeTests.passed

attributeTests.passed: $(top_srcdir)/scripts/test_exit_status attributeTests
	@$(RTH_RUN)					\
		TITLE="AstAttributeMechanism [$@]"	\
		CMD="./attributeTests"			\
		$< $@


###############################################################################################################################
# Tests that mlog are working
###############################################################################################################################

noinst_PROGRAMS += testMLog
testMLog_SOURCES = testMLog.C

TEST_TARGETS += testMLog.passed

testMLog.passed: $(top_srcdir)/scripts/test_exit_status testMLog
	@$(RTH_RUN)				\
		TITLE="test mlogging facility (src/util/mlog.h/C) [$@]"	\
		CMD="./testMLog"		\
		$< $@

###############################################################################################################################
# Tests the StringUtility functions
###############################################################################################################################

noinst_PROGRAMS += testStringUtility
testStringUtility_SOURCES = testStringUtility.C

TEST_TARGETS += testStringUtility.passed

testStringUtility.passed: $(top_srcdir)/scripts/test_exit_status testStringUtility
	@$(RTH_RUN)				\
		TITLE="string utilities"	\
		CMD="./testStringUtility"	\
		$< $@

########################################################################################################################
# SourceLocation
########################################################################################################################

noinst_PROGRAMS += testSourceLocation
testSourceLocation_SOURCES = testSourceLocation.C

TEST_TARGETS += testSourceLocation.passed
testSourceLocation.passed: $(top_srcdir)/scripts/test_exit_status testSourceLocation
	@$(RTH_RUN)					\
		TITLE="test SourceLocation [$@]"	\
		CMD="./testSourceLocation"		\
		$< $@

###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
