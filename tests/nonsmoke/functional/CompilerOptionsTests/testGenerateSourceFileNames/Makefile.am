include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if !OS_MACOSX

TEST_TRANSLATOR = $(top_builddir)/tests/nonsmoke/functional/testTranslator $(ROSE_FLAGS)

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

# This rule is run after automake's internal check rule (which we don't want to use)
test:
	@echo "#################################################"
	@echo "    Test ROSE's parsing of file names from       "
	@echo "#################################################"
	@echo "** The 'roseTranslator -c file -o obj' should compile and not disassemble obj **"
	$(CXX)  -c $(srcdir)/test1.C -o test1.o 
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1.C -o test1.o 
	sh $(srcdir)/doesFilesExist.sh ./rose_test1.C
	sh $(srcdir)/filesNotExist.sh  ./rose_test1.o.s
	$(MAKE) clean
	@echo	"** The 'roseTranslator obj' should produce an executable and not disassemble obj **"
	$(CXX) -c $(srcdir)/test1.C -o test1.o 
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 test1.o 
	sh $(srcdir)/doesFilesExist.sh ./a.out
	sh $(srcdir)/filesNotExist.sh  ./rose_test1.o.s
	$(MAKE) clean
	@echo	"** The 'roseTranslator source' should compile source **"
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:verbose 0 $(srcdir)/test1.C
	sh $(srcdir)/doesFilesExist.sh ./a.out ./rose_test1.C
	$(MAKE) clean
	@echo "*****************************************************************************************************************"
	@echo "*  ROSE/tests/CompilerOptionTests/testGenerateSourceFileNames: make check rule complete (terminated normally)   *"
	@echo "*****************************************************************************************************************"

conditional-check-local:
	$(MAKE) test
	@echo "*****************************************************************************************************************"
	@echo "*  ROSE/tests/CompilerOptionTests/testGenerateSourceFileNames: ############# TESTS COMMENTED OUT ############   *"
	@echo "*****************************************************************************************************************"

else
conditional-check-local:
endif

EXTRA_DIST = doesFilesExist.sh  filesNotExist.sh test1.C

check-local: conditional-check-local

clean-local:
	rm -f *.o test1 *.s rm *.out rose_*.C *.C.pdf *.dot core local_test-isystemOption.C *.dump *.new


