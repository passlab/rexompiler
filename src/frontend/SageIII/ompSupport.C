// Refined OmpAttribute connected to a parser using Bison
//Liao, 9/18/2008
//

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "ompSupport.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

namespace OmpSupport
{
  omp_construct_enum cur_omp_directive;

 
  // OmpSupport::toString()
  string toString(omp_construct_enum omp_type)
  {
    string result;
    switch (omp_type)
    {
      /*
         case e_: result = ""; break;
         */
      //directives
      case e_unknown: result ="unknown" ; break;
      case e_parallel: result = "parallel" ; break;
      case e_for: result = "for"; break;
      case e_for_simd: result = "for simd"; break;
      case e_do: result = "do"; break;
      case e_workshare: result = "workshare"; break;
      case e_sections: result = "sections"; break;
      case e_section: result = "section"; break;
      case e_single: result = "single"; break;

      case e_master: result = "master"; break;
      case e_critical: result = "critical"; break;
      case e_barrier: result = "barrier"; break;
      case e_atomic: result = "atomic"; break;
      case e_flush: result = "flush"; break;


      case e_threadprivate: result = "threadprivate"; break;
      case e_parallel_for: result = "parallel for"; break;
      case e_parallel_for_simd: result = "parallel for simd"; break;
      case e_parallel_do: result = "parallel do"; break;
      case e_parallel_sections: result = "parallel sections"; break;
      case e_parallel_workshare: result = "parallel workshare"; break;
      case e_task: result = "task"; break;
      case e_taskwait: result = "taskwait"; break;
      case e_ordered_directive: result = "ordered"; break;

      case e_target: result = "target"; break;
      case e_target_declare : result = "target declare"; break;
      case e_target_data: result = "target data"; break;
      case e_target_update: result = "target update"; break;

                                // Fortran only end directives
      case e_end_critical: result = "end critical"; break;
      case e_end_do: result = "end do"; break;
      case e_end_master: result = "end master"; break;
      case e_end_ordered:result = "end ordered"; break;
      case e_end_parallel_do:result = "end parallel do"; break;
      case e_end_parallel_sections:result = "end parallel sections"; break;
      case e_end_parallel_workshare:result = "end parallel workshare"; break;
      case e_end_parallel:result = "end parallel"; break;
      case e_end_sections:result = "end sections"; break;
      case e_end_single:result = "end single"; break;
      case e_end_task:result = "end task"; break;
      case e_end_workshare:result = "end workshare"; break;

      // clauses
      case e_default: result = "default"; break;
      case e_shared: result = "shared"; break;
      case e_private: result = "private"; break;
      case e_firstprivate: result = "firstprivate"; break;
      case e_lastprivate: result = "lastprivate"; break;
      case e_copyin: result = "copyin"; break;
      case e_copyprivate: result = "copyprivate"; break;
      case e_proc_bind:        result = "proc_bind"; break;
      case e_atomic_clause:    result = "atomic_clause" ; break; //TODO

      case e_if: result = "if"; break;
      case e_num_threads: result = "num_threads"; break;
      case e_nowait: result = "nowait"; break;
      case e_ordered_clause: result = "ordered"; break;
      case e_reduction: result = "reduction"; break;
      case e_schedule: result = "schedule"; break;
      case e_collapse: result = "collapse"; break;
      case e_untied: result = "untied"; break;
      case e_mergeable: result = "mergeable"; break;

      case e_map: result = "map"; break;
      case e_device: result = "device"; break;

                     // values
      case e_default_none: result = "none"; break;
      case e_default_shared: result = "shared"; break;
      case e_default_private: result = "private"; break;
      case e_default_firstprivate: result = "firstprivate"; break;


      case e_proc_bind_master: result = "master"; break;
      case e_proc_bind_close:  result = "close"; break;
      case e_proc_bind_spread: result = "spread"; break;

      case e_atomic_read:    result = "read" ; break; 
      case e_atomic_write:    result = "write" ; break; 
      case e_atomic_update:    result = "update" ; break; 
      case e_atomic_capture:    result = "capture" ; break; 

      case e_reduction_plus: result = "+"; break;
      case e_reduction_minus: result = "-"; break;
      case e_reduction_mul: result = "*"; break;
      case e_reduction_bitand: result = "&"; break;
      case e_reduction_bitor: result = "|"; break;

      case e_reduction_bitxor: result = "^"; break;
      case e_reduction_logand: result = "&&"; break;
      case e_reduction_logor: result = "||"; break;

      case e_reduction_min: result = "min"; break;
      case e_reduction_max: result = "max"; break;

      case e_reduction_and: result = ".and."; break;
      case e_reduction_or: result = ".or."; break;
      case e_reduction_eqv: result = ".eqv."; break;

      case e_reduction_neqv: result = ".neqv."; break;
      case e_reduction_iand: result = "iand"; break;

      case e_reduction_ior: result = "ior"; break;
      case e_reduction_ieor: result = "ieor"; break;

      case e_schedule_none: result = "not-specified"; break;
      case e_schedule_static: result = "static"; break;
      case e_schedule_dynamic: result = "dynamic"; break;
      case e_schedule_guided: result = "guided"; break;
      case e_schedule_auto: result = "auto"; break;
      case e_schedule_runtime: result = "runtime"; break;

      case e_map_alloc: result = "alloc"; break;
      case e_map_to: result = "to"; break;
      case e_map_from: result = "from"; break;
      case e_map_tofrom: result = "tofrom"; break;

      case e_dist_data: result = "dist_data"; break;
      case e_duplicate: result = "duplicate"; break;
      case e_cyclic:    result = "cyclic"; break;
      case e_block:     result = "block"; break;
       

      case e_simd: result = "simd"; break;
      case e_declare_simd: result = "declare simd"; break;
      case e_safelen: result = "safelen"; break;
      case e_simdlen: result = "simdlen"; break;
      case e_linear: result = "linear"; break;
      case e_uniform: result = "uniform"; break;
      case e_aligned: result = "aligned"; break;

      case e_begin: result = "begin"; break;
      case e_end:   result = "end";   break;

      case e_inbranch: result = "inbranch"; break;
      case e_notinbranch:   result = "notinbranch";   break;

      case e_depend:       result = "depend";   break;
      case e_depend_in:    result = "int";   break;
      case e_depend_out:   result = "out";   break;
      case e_depend_inout: result = "inout";   break;

      case e_final : result = "final";   break;
      case e_priority: result = "priority";   break;

      case e_allocate: result = "allocate";   break;

      case e_not_omp: result = "not_omp"; break;
      default: 
      {
        cerr<<"OmpSupport::toString(): unrecognized enumerate value:"<<omp_type<<endl;
        ROSE_ABORT ();
      }
    }
    // Not true for Fortran!!
    //    if (isDirective(omp_type))
    //      result= "omp " + result;
    return result;
  }
  bool isDirective(omp_construct_enum omp_type)
  {
    bool result = false;
    switch (omp_type)
    { // 16 directives as OpenMP 3.0
      //+2 for Fortran
      case e_parallel:
      case e_for:
      case e_for_simd:
      case e_do:
      case e_workshare:
      case e_sections:
      case e_section:
      case e_single:

      case e_master: 
      case e_critical:
      case e_barrier:
      case e_atomic:
      case e_flush:

      case e_threadprivate:
      case e_parallel_for:
      case e_parallel_for_simd:
      case e_parallel_do: //fortran
      case e_parallel_sections:
      case e_parallel_workshare://fortran
      case e_task:
      case e_taskwait:

      case e_ordered_directive:

        // Fortran only end directives
      case e_end_critical:
      case e_end_do:
      case e_end_master:
      case e_end_ordered:
      case e_end_parallel_do:
      case e_end_parallel_sections:
      case e_end_parallel_workshare:
      case e_end_parallel:
      case e_end_sections:
      case e_end_single:
      case e_end_task:
      case e_end_workshare:

      // Experimental OpenMP Accelerator directives
      case e_target:
      case e_target_declare:
      case e_target_data:
      case e_target_update: //TODO more later
      case e_simd:
      case e_declare_simd:

        result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;
  }

  //! Get the corresponding begin construct enum from an end construct enum
  omp_construct_enum getBeginOmpConstructEnum (omp_construct_enum end_enum)
  {
    ROSE_ASSERT (isFortranEndDirective (end_enum));
    omp_construct_enum begin_enum;
    switch (end_enum)
    {
      case e_end_parallel:
        begin_enum = e_parallel;
        break;
      case e_end_do:
        begin_enum = e_do;
        break;
      case e_end_sections:
        begin_enum = e_sections;
        break;
      case e_end_single:
        begin_enum = e_single;
        break;
      case e_end_workshare:
        begin_enum = e_workshare;
        break;
      case e_end_parallel_do:
        begin_enum = e_parallel_do;
        break;
      case e_end_parallel_sections:
        begin_enum = e_parallel_sections;
        break;
      case e_end_parallel_workshare:
        begin_enum = e_parallel_workshare;
        break;
      case e_end_task:
        begin_enum = e_task;
        break;
      case e_end_master:
        begin_enum = e_master;
        break;
      case e_end_critical:
        begin_enum = e_critical;
        break;
      case e_end_ordered:
        begin_enum = e_ordered_directive;
        break;
      default:
        {
          ROSE_ABORT ();
        }
    } // end switch
    return begin_enum;
  }

  //! Check if the construct is a Fortran directive which can (optionally) have a corresponding END directive
  bool isFortranBeginDirective(omp_construct_enum omp_type)
  {
    bool rt = false;
    switch (omp_type)
    {
      case e_parallel:
      case e_do:
      case e_sections:
      case e_single:
      case e_workshare:
      case e_parallel_do:
      case e_parallel_sections:
      case e_parallel_workshare:
      case e_task:
      case e_master:
      case e_critical:
      case e_ordered_directive:
        {
          rt = true;
          break;
        }
      default:
        {
          break;
        }
    }
    return rt;
  }
  //! Get the corresponding end construct enum from a begin construct enum
  omp_construct_enum getEndOmpConstructEnum (omp_construct_enum begin_enum)
  {
    omp_construct_enum rt; 
    switch (begin_enum)
    {
      case e_parallel:
        rt = e_end_parallel;
        break;
      case e_do:
        rt = e_end_do;
        break;
      case e_sections:
        rt = e_end_sections;
        break;
      case e_single:
        rt = e_end_single;
        break;
      case e_workshare:
        rt = e_end_workshare;
        break;
      case e_parallel_do:
        rt = e_end_parallel_do;
        break;
      case e_parallel_sections:
        rt = e_end_parallel_sections;
        break;
      case e_parallel_workshare:
        rt = e_end_parallel_workshare;
        break;
      case e_task:
        rt = e_end_task;
        break; 
      case e_master:
        rt = e_end_master;
        break;
      case e_critical:
        rt = e_end_critical;
        break;
      case e_ordered_directive:
        {
          rt = e_end_ordered;
          break;
        }
      default:
        {
          cerr<<"In getEndOmpConstructEnum(): illegal begin enum is found:"<< toString(begin_enum)<<endl;
          ROSE_ABORT ();
        }
    } // end switch
    return rt;
  }

  //! Check if the construct is a Fortran END ... directive
  bool isFortranEndDirective(omp_construct_enum omp_type)
  {
    bool rt = false;
    switch (omp_type)
    { // 16 directives as OpenMP 3.0
      //+2 for Fortran
      case e_end_critical:
      case e_end_do:
      case e_end_master:
      case e_end_ordered:
      case e_end_parallel_do:
      case e_end_parallel_sections:
      case e_end_parallel_workshare:
      case e_end_parallel:
      case e_end_sections:
      case e_end_single:
      case e_end_task:
      case e_end_workshare:
        rt = true; 
        break; 
      default:
        break;
    } 
    return rt;  
  }

  //! Check if an OpenMP directive has a structured body
  bool isDirectiveWithBody(omp_construct_enum omp_type)
  {
    bool result = false;
    ROSE_ASSERT(isDirective(omp_type));
    switch (omp_type)
    { // 16 directives as OpenMP 3.0
      //+2 for Fortran
      case e_parallel:
      case e_for:
      case e_for_simd:
      case e_do:
      case e_workshare:
      case e_sections:
      case e_section:
      case e_single:

      case e_target:
      case e_target_data:

      case e_master: 
      case e_critical:
      case e_barrier:
      case e_atomic:
        //    case e_flush:

        //      case e_threadprivate:
      case e_parallel_for:
      case e_parallel_for_simd:
      case e_parallel_do: //fortran
      case e_parallel_sections:
      case e_parallel_workshare://fortran
      case e_task:
        //      case e_taskwait:

      case e_ordered_directive:

      case e_simd:

        // Fortran only end directives
        //      case e_end_critical:
        //      case e_end_do:
        //      case e_end_master:
        //      case e_end_ordered:
        //      case e_end_parallel_do:
        //      case e_end_parallel_sections:
        //      case e_end_parallel_workshare:
        //      case e_end_parallel:
        //      case e_end_sections:
        //      case e_end_single:
        //      case e_end_task:
        //      case e_end_workshare:

        result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;

  }

  bool isClause(omp_construct_enum clause_type)
  {
    bool result = false;

    switch (clause_type)
    { //total 15 possible clauses as OpenMP 3.0
      case e_default:
      case e_shared:
      case e_private:
      case e_firstprivate:
      case e_lastprivate:

      case e_copyin:
      case e_copyprivate:
      case e_if:
      case e_num_threads:
      case e_nowait:

      case e_ordered_clause:
      case e_reduction:

      case e_schedule:
      case e_collapse:
      case e_untied:
      case e_mergeable:

      case e_proc_bind:
      case e_atomic_clause:

     // experimental accelerator clauses 
      case e_map:
      case e_dist_data:
      case e_device:
    // experimental ones for SPMD begin/end
      case e_begin:
      case e_end:

      case e_safelen:
      case e_simdlen:
      case e_linear:
      case e_uniform:
      case e_aligned:

      case e_final:
      case e_priority:
      case e_inbranch:
      case e_notinbranch:

      case e_depend:
      // OpenMP 5.0 new clause
      case e_allocate:
        result = true; 
        break;
      default:
        result = false;
        break;
    }
    return result; 
  }


  bool isReductionOperator(omp_construct_enum omp_type)
  {
    bool result = false;
    switch (omp_type)
    {
      case e_reduction_plus:
      case e_reduction_minus:
      case e_reduction_mul:
      case e_reduction_bitand:
      case e_reduction_bitor:

      case e_reduction_bitxor:
      case e_reduction_logand:
      case e_reduction_logor: 
        // TODO more reduction intrinsic procedure name for Fortran  
      case e_reduction_min: //?
      case e_reduction_max:
        result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;
  }

  bool isDependenceType(omp_construct_enum omp_type)
  {
    bool result = false;
    switch (omp_type)
    {
      case e_depend_in:
      case e_depend_out:
      case e_depend_inout:
       result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;
  }
} //end namespace OmpSupport
