#ifndef Omp_Support_h_INCLUDED
#define Omp_Support_h_INCLUDED
/*!
 * ROSE persistent attribute to represent OpenMP 3.0 C/C++/Fortran directives
 * in the OpenMP parser 
 *  Ideally, this should be transparent to users since we want to them to 
 *  operate on dedicated OpenMP sage nodes instead.
 *
 * All things go to the OmpSupport namespace to avoid conflicts 
 */
//
// Liao 9/17, 2008
//

#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <vector>
#include <deque>
class SgNode;
/** Types and functions to support OpenMP
*
*  OpenMP specific data types and functions are put into this namespace */
namespace OmpSupport
{
  // OpenMP construct name list
  //-------------------------------------------------------------------
  // We put all directive and clause types into one enumerate type
  // since some internal data structure(map) have to access 
  // both directives and clauses uniformly
  enum  omp_construct_enum 
  {
    e_unknown = 0, 

    // 16 directives as OpenMP 3.0
    e_parallel,
    e_for,
    e_for_simd,
    e_do,
    e_workshare,
    e_sections,
    e_section,
    e_single,

    e_master, 
    e_critical,
    e_barrier,
    e_atomic,
    e_flush,

    // Liao, 1/15/2013, experimental implementation for the draft OpenMP Accelerator Model technical report 
    e_target, 
    e_target_declare,
    e_target_data,
    e_target_update, 
    e_map, // map clauses
    e_device,
    e_begin, // 10/29/2015, experimental begin/end directives for SPMD code blocks, without changing variable scopes
    e_end,

    e_threadprivate,
    e_parallel_for,
    e_parallel_for_simd,
    e_parallel_do,
    e_parallel_sections,
    e_parallel_workshare,
    e_task,
    e_taskwait, 
    // we have both ordered directive and ordered clause, 
    //so make the name explicit
    e_ordered_directive,

    // Fortran only end directives
    e_end_critical,
    e_end_do,
    e_end_master,
    e_end_ordered,
    e_end_parallel_do,
    e_end_parallel_sections,
    e_end_parallel_workshare,
    e_end_parallel,
    e_end_sections,
    e_end_single,
    e_end_task,
    e_end_workshare,

    // 15 clauses for OpenMP 3.0
    // 7 data-sharing attributes clauses
    e_default, // the clause
    e_shared,
    e_private,
    e_firstprivate,
    e_lastprivate,
    e_copyin,
    e_copyprivate,
    e_proc_bind, 

    //8 misc clauses
    e_if, // used with omp parallel or omp task
    e_num_threads, // for omp parallel only
    e_nowait,
    e_ordered_clause,
    e_reduction,
    e_schedule,
    e_collapse,
    e_untied, 
    e_mergeable, 
    e_final, 
    e_priority, 
    e_atomic_clause, 
    e_inbranch,
    e_notinbranch,

    e_depend, // OpenMP 4.0 task clauses

    // Simple values for some clauses

    //4 values for default clause 
    //C/C++ default values
    e_default_none,
    e_default_shared,
    //Fortran default values
    e_default_private,
    e_default_firstprivate,

    // proc_bind(master|close|spread)
    e_proc_bind_master, 
    e_proc_bind_close, 
    e_proc_bind_spread, 

    e_atomic_read, 
    e_atomic_write, 
    e_atomic_update, 
    e_atomic_capture,
 
    // in_reduction operations
    e_in_reduction_identifier_plus, //+
    e_in_reduction_identifier_mul,  //* 
    e_in_reduction_identifier_minus, // -
    e_in_reduction_identifier_bitand, // &  
    e_in_reduction_identifier_bitor,  // | 
    e_in_reduction_identifier_bitxor,  // ^  
    e_in_reduction_identifier_logand,  // &&  
    e_in_reduction_identifier_logor,   // || 
    e_in_reduction_identifier_and, // .and.
    e_in_reduction_identifier_or, // .or.
    e_in_reduction_identifier_eqv,   // fortran .eqv. 
    e_in_reduction_identifier_neqv,   // fortran .neqv.
    e_in_reduction_identifier_max,
    e_in_reduction_identifier_min, 
    e_in_reduction_identifier_iand,
    e_in_reduction_identifier_ior,
    e_in_reduction_identifier_ieor,

    // task_reduction operations
    e_task_reduction_identifier_plus, //+
    e_task_reduction_identifier_mul,  //* 
    e_task_reduction_identifier_minus, // -
    e_task_reduction_identifier_bitand, // &  
    e_task_reduction_identifier_bitor,  // | 
    e_task_reduction_identifier_bitxor,  // ^  
    e_task_reduction_identifier_logand,  // &&  
    e_task_reduction_identifier_logor,   // || 
    e_task_reduction_identifier_and, // .and.
    e_task_reduction_identifier_or, // .or.
    e_task_reduction_identifier_eqv,   // fortran .eqv. 
    e_task_reduction_identifier_neqv,   // fortran .neqv.
    e_task_reduction_identifier_max,
    e_task_reduction_identifier_min, 
    e_task_reduction_identifier_iand,
    e_task_reduction_identifier_ior,
    e_task_reduction_identifier_ieor,

    // reduction operations
    //8 operand for C/C++
     //  shared 3 common operators for both C and Fortran
    e_reduction_plus, //+
    e_reduction_mul,  //* 
    e_reduction_minus, // -

      // C/C++ only
    e_reduction_bitand, // &  
    e_reduction_bitor,  // | 
    e_reduction_bitxor,  // ^  
    e_reduction_logand,  // &&  
    e_reduction_logor,   // || 

    // Fortran operator
    e_reduction_and, // .and.
    e_reduction_or, // .or.
    e_reduction_eqv,   // fortran .eqv. 
    e_reduction_neqv,   // fortran .neqv.

    // reduction intrinsic procedure name for Fortran  
    // min, max also for C
    e_reduction_max,
    e_reduction_min, 

    e_reduction_iand,
    e_reduction_ior,
    e_reduction_ieor,

    // reduction modifiers in OpenMP 5.0
    e_reduction_inscan,
    e_reduction_task,
    e_reduction_default,

    // lastprivate modifiers in OpenMP 5.0
    e_lastprivate_conditional,

    // device modifiers in OpenMP 5.0
    e_device_ancestor,
    e_device_device_num,

    // linear modifiers in OpenMP 5.0
    e_linear_ref,
    e_linear_val,
    e_linear_uval,
    
    // enum for all the user defined parameters
    e_user_defined_parameter,

    //5 schedule policies for
    //---------------------
    e_schedule_none,
    e_schedule_static,
    e_schedule_dynamic,
    e_schedule_guided,
    e_schedule_auto,
    e_schedule_runtime,

    // 4 device map variants
    //----------------------
    e_map_alloc,
    e_map_to,
    e_map_from,
    e_map_tofrom,

    // experimental dist_data clause dist_data(dim1_policy, dim2_policy, dim3_policy)
    // A policy can be block(n), cyclic(n), or duplicate
    e_dist_data, 
    e_duplicate, 
    e_block,
    e_cyclic,

    // experimental SIMD directive, phlin 8/5/2013
    e_simd,
    e_declare_simd,
    e_safelen,
    e_simdlen,
    e_uniform,
    e_aligned,
    e_linear,

    // task dependence type
    e_depend_in, 
    e_depend_out, 
    e_depend_inout, 
    e_depend_mutexinoutset, 
    e_depend_depobj, 
    
    // task depend modifier
    e_omp_depend_modifier_iterator, 

    // OpenMP 5.0 clause
    e_allocate,
    e_allocate_default_mem_alloc,
    e_allocate_large_cap_mem_alloc,
    e_allocate_const_mem_alloc,
    e_allocate_high_bw_mem_alloc,
    e_allocate_low_lat_mem_alloc,
    e_allocate_cgroup_mem_alloc,
    e_allocate_pteam_mem_alloc,
    e_allocate_thread_mem_alloc,

    // not an OpenMP construct
    e_not_omp

  }; //end omp_construct_enum

  // A new variable to communicate the context of OpenMP parser
  // what directive is being parsed right now. 
  // This is useful for rare case of parsing "declare simd"
  extern omp_construct_enum cur_omp_directive; 
   
  //-------------------------------------------------------------------
  // some utility functions

  //! Output omp_construct_enum to a string: 
  // Better using OmpSupport::toString() to avoid ambiguous 
  std::string toString(omp_construct_enum omp_type);

  //! Check if the construct is a Fortran END ... directive
  bool isFortranEndDirective(omp_construct_enum omp_type);

  //! Check if the construct is a Fortran directive which can (optionally) have a corresponding END directive
  bool isFortranBeginDirective(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a directive
  bool isDirective(omp_construct_enum omp_type);

  //! Check if an OpenMP directive has a structured body
  bool isDirectiveWithBody(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a clause
  bool isClause(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a reduction operator
  bool isReductionOperator(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a dependence type for omp task depend 
  bool isDependenceType(omp_construct_enum omp_type);

  //! Attach an OpenMP clause to directive
  void addOmpClause(SgStatement* node, SgOmpClause* clause);

  // We use objects of this class to store parameters of those clauses that take one or two additional
  // parameters other than variable or expression list. E.g. reduction([reduction_modifier,]reduction_identifier:list).
  // We call this kind of clause as ComplexClause, compared with other clauses which just take one parameter or a list
  // of variable/expression.
  class ComplexClause {

    public:
      omp_construct_enum clause_type;
      omp_construct_enum first_parameter;
      omp_construct_enum second_parameter;
      omp_construct_enum third_parameter;
      std::pair < std::string, SgExpression* > user_defined_parameter;
      std::pair < std::string, SgExpression* > expression;
      std::vector < std::pair < std::string, SgNode* > > variable_list;

      ComplexClause(omp_construct_enum first=e_unknown, omp_construct_enum second=e_unknown, omp_construct_enum third=e_unknown) : first_parameter(first), second_parameter(second), third_parameter(third) {};

  };

 // We reuse the list later on to build OpenMP AST for Fortran
} //end namespace OmpSupport

#endif //Omp_Support_h_INCLUDED

