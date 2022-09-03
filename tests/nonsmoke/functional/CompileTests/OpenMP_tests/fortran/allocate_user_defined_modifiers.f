      program  main
      implicit none
!     include 'omp_lib.h'

      integer a, b, c

!$omp parallel allocate (user_modi:a, b) allocate (user_modi:a, c) 
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

      END
