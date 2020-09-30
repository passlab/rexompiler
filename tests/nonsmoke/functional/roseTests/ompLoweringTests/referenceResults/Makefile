ifeq (${LLVM_OPENMP_INSTALL},)
	LLVM_OPENMP_INSTALL := /opt/llvm/llvm-10.x-install
endif
ifeq (${LLVM_OPENMP_SOURCE},)
	LLVM_OPENMP_SOURCE := /home/xyi2/llvm-source/openmp
endif

trans_barrier.out: transformed_barrier.c
	clang -fopenmp transformed_barrier.c -o trans_barrier.out
 
trans_masterSingle2.out: transformed_masterSingle2.c
	clang -fopenmp transformed_masterSingle2.c -o trans_masterSingle2.out
 


clean: 
	rm -rf ${OBJS} *.cubin *.out

