      program  main
      implicit none
!     include 'omp_lib.h'

      integer a, b, c;
!$omp parallel allocate (omp_cgroup_mem_alloc:a, b) allocate (a, c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_default_mem_alloc:a, b) allocate (a)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_const_mem_alloc:a, b) allocate (c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_large_cap_mem_alloc:a, b) allocate (a, c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_high_bw_mem_alloc:a, b) allocate (a, c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_low_lat_mem_alloc:a, b) allocate (a, c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_pteam_mem_alloc:a, b) allocate (a, c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

!$omp parallel allocate (omp_thread_mem_alloc:a, b) allocate (a, c)
      print *, "This is for testing parser and AST construction, which could be only syntax correct."
c$omp end parallel

      END
