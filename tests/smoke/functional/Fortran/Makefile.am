include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

########################################################################################################################
## Fortran Parser Tests
#
# These tests quickly check whether the ATerm and Open Fortran Parser files are installed correctly
#

fortran_parser_tests =

# Rasmussen (11.17.2017): This test only checks if the Fortran parse table, Fortran.tbl, is installed.
# It should actually check for stratego and ATerm libraries as well.  But this should be done in
# the configure scripts so doesn't need to be here. And, in any case, the macro variable OFP_BIN_PATH
# is no longer used.
#if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#fortran_parser_tests += testBafio
#noinst_PROGRAMS      += testBafio
#endif
#
#testBafio_SOURCES = testBafio.C
#testBafio_SOURCES =

## Test targets
#
fortran_targets = $(addsuffix .passed, $(fortran_parser_tests))
TEST_TARGETS += $(fortran_targets)

## Test rules
#
$(fortran_targets): %.passed: %
	@$(RTH_RUN)                                             \
                TITLE="Fortran parser test [$@]"                \
                CMD="$$(pwd)/$^"                                \
                $(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-fortran
check-fortran: $(FORTRAN_TEST_TARGETS)


###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
