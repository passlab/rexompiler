#include "sage3basic.h"

using namespace SageBuilder;
using namespace SageInterface;

namespace OmpSupport
{

    void analyzeOmpFor(SgNode* node)
    {
        ROSE_ASSERT(node != NULL);
        SgOmpForStatement* target1 = isSgOmpForStatement(node);
        SgOmpDoStatement* target2 = isSgOmpDoStatement(node);

        SgOmpClauseBodyStatement* target = (target1 != NULL ? (SgOmpClauseBodyStatement*)target1 : (SgOmpClauseBodyStatement*)target2);
        ROSE_ASSERT(target != NULL);

        SgScopeStatement* p_scope = target->get_scope();
        ROSE_ASSERT(p_scope != NULL);

        SgStatement* body = target->get_body();
        ROSE_ASSERT(body != NULL);
        // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
        SgForStatement * for_loop = isSgForStatement(body);
        SgFortranDo * do_loop = isSgFortranDo(body);

        SgStatement* loop = (for_loop != NULL ? (SgStatement*)for_loop : (SgStatement*)do_loop);
        ROSE_ASSERT(loop != NULL);

        Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpScheduleClause);
        if (clauses.size() != 0)
        {
            SgOmpScheduleClause* s_clause = isSgOmpScheduleClause(clauses[0]);
            ROSE_ASSERT(s_clause);
            SgOmpClause::omp_schedule_kind_enum sg_kind = s_clause->get_kind();
            SgExpression* orig_chunk_size = s_clause->get_chunk_size();
            if (!orig_chunk_size )
            {
                if (sg_kind == SgOmpClause::e_omp_schedule_kind_dynamic || sg_kind == SgOmpClause::e_omp_schedule_kind_guided)
                {
                    printf("A default chunk size is added.\n");
                    SgExpression* chunk_size = buildIntVal(1);
                    s_clause->set_chunk_size(chunk_size);
                }

                printf("A default schedule modifier is added.\n");
                SgOmpClause::omp_schedule_modifier_enum sg_modifier1 = s_clause->get_modifier1();
                if (sg_modifier1 == SgOmpClause::e_omp_schedule_modifier_unspecified)
                {
                    sg_modifier1 = SgOmpClause::e_omp_schedule_modifier_nonmonotonic;
                }
                s_clause->set_modifier1(sg_modifier1);
            }
        }
        else
        {
            SgOmpClause::omp_schedule_modifier_enum sg_modifier1 = SgOmpClause::e_omp_schedule_modifier_nonmonotonic;
            SgOmpClause::omp_schedule_modifier_enum sg_modifier2 = SgOmpClause::e_omp_schedule_modifier_unspecified;
            SgOmpClause::omp_schedule_kind_enum sg_kind = SgOmpClause::e_omp_schedule_kind_static;
            SgExpression* chunk_size = NULL;
            SgOmpScheduleClause* sg_clause = new SgOmpScheduleClause(sg_modifier1, sg_modifier2, sg_kind, chunk_size);

            ROSE_ASSERT(sg_clause);
            setOneSourcePositionForTransformation(sg_clause);
            target->get_clauses().push_back(sg_clause);
            sg_clause->set_parent(target);
            printf("A default schedule clause is added.\n");
        }
    } // end analyze omp for

    void analyze_omp(SgSourceFile* file)
    {
        std::cout << "Omp Analysis\n";

        Rose_STL_Container<SgNode*> node_list = NodeQuery::querySubTree(file, V_SgStatement);
        Rose_STL_Container<SgNode*>::reverse_iterator node_list_iterator;
        for (node_list_iterator = node_list.rbegin() ;node_list_iterator != node_list.rend(); node_list_iterator++)
        {
            SgStatement* node = isSgStatement(*node_list_iterator);
            ROSE_ASSERT(node != NULL);
            switch (node->variantT())
            {
                case V_SgOmpForStatement:
                {
                    analyzeOmpFor(node);
                    break;
                }
                default:
                {
                    //std::cout << "Nothing happened.\n";
                }
            }// switch
        }
    }
}

