#include "sage3basic.h"

#include "untypedBuilder.h"
//#include "general_language_translation.h"

namespace UntypedBuilder {

SgFile::languageOption_enum language_enum = SgFile::e_error_language;

void set_language(SgFile::languageOption_enum current_language)
{
   language_enum = current_language;
}


template <class ScopeClass>
ScopeClass* buildScope()
{
   return buildScope<ScopeClass>("");
}

template <class ScopeClass>
ScopeClass* buildScope(const std::string & label)
{
   SgUntypedDeclarationStatementList* decl_list = new SgUntypedDeclarationStatementList();
   ROSE_ASSERT(decl_list);
   SageInterface::setSourcePosition(decl_list);

   SgUntypedStatementList* stmt_list = new SgUntypedStatementList();
   ROSE_ASSERT(stmt_list);
   SageInterface::setSourcePosition(stmt_list);

   SgUntypedFunctionDeclarationList* func_list = new SgUntypedFunctionDeclarationList();
   ROSE_ASSERT(func_list);
   SageInterface::setSourcePosition(func_list);

   ScopeClass* scope = new ScopeClass(label, decl_list, stmt_list, func_list);
   ROSE_ASSERT(scope);
   SageInterface::setSourcePosition(scope);

   return scope;
}

// Explicit instantiations for library usage
//
template SgUntypedScope* buildScope<SgUntypedScope>();
template SgUntypedScope* buildScope<SgUntypedScope>(const std::string & label);

template SgUntypedGlobalScope* buildScope<SgUntypedGlobalScope>();
template SgUntypedGlobalScope* buildScope<SgUntypedGlobalScope>(const std::string & label);

template SgUntypedFunctionScope* buildScope<SgUntypedFunctionScope>();
template SgUntypedFunctionScope* buildScope<SgUntypedFunctionScope>(const std::string & label);


SgUntypedType* buildType(SgUntypedType::type_enum type_enum, std::string name)
{
   SgUntypedExpression* type_kind = NULL;
   bool has_kind = false;
   bool is_literal = false;
   bool is_class = false;
   bool is_intrinsic = true;
   bool is_constant = false;
   bool is_user_defined = false;
   SgUntypedExpression* char_length_expr = NULL;
   std::string char_length;
   bool char_length_is_string = false;

   SgUntypedExprListExpression* modifiers = new SgUntypedExprListExpression();
   ROSE_ASSERT(modifiers);
   SageInterface::setSourcePosition(modifiers);

   SgUntypedType* type = NULL;

   switch(language_enum)
    {
      case SgFile::e_Fortran_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_unknown:
               {
                 type = new SgUntypedType("UNKNOWN",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_implicit:
               {
                 type = new SgUntypedType("IMPLICIT",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_void:
               {
                 type = new SgUntypedType("void",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_int:
               {
                 type = new SgUntypedType("integer",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_float:
               {
                 type = new SgUntypedType("real",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_complex:
               {
                 type = new SgUntypedType("complex",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_bool:
               {
                 type = new SgUntypedType("logical",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_char:
             case SgUntypedType::e_string:
               {
                 type = new SgUntypedType("character",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildType: unimplemented for Fortran type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      default:
        {
          fprintf(stderr, "UntypedBuilder::buildType: unimplemented for language_enum %d \n", language_enum);
          ROSE_ASSERT(0);  // NOT IMPLEMENTED
        }

    } // switch(language_enum)

   return type;
}

SgUntypedArrayType* buildArrayType(SgUntypedType::type_enum type_enum, SgUntypedExprListExpression* shape, int rank)
{
   SgUntypedExpression* type_kind = NULL;
   bool has_kind = false;
   bool is_literal = false;
   bool is_class = false;
   bool is_intrinsic = true;
   bool is_constant = false;
   bool is_user_defined = false;
   SgUntypedExpression* char_length_expr = NULL;
   std::string char_length;
   bool char_length_is_string = false;

   SgUntypedExprListExpression* modifiers = new SgUntypedExprListExpression();
   ROSE_ASSERT(modifiers);
   SageInterface::setSourcePosition(modifiers);

   SgUntypedArrayType* type = NULL;

   ROSE_ASSERT(shape != NULL);

   switch(language_enum)
    {
      case SgFile::e_Fortran_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_unknown:
               {
                 type = new SgUntypedArrayType("UNKNOWN",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_implicit:
               {
                 type = new SgUntypedArrayType("IMPLICIT",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_void:
               {
                 type = new SgUntypedArrayType("void",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_int:
               {
                 type = new SgUntypedArrayType("integer",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_float:
               {
                 type = new SgUntypedArrayType("real",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_bool:
               {
                 type = new SgUntypedArrayType("logical",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildArrayType: unimplemented for Fortran type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      default:
        {
          fprintf(stderr, "UntypedBuilder::buildArrayType: unimplemented for language_enum %d \n", language_enum);
          ROSE_ASSERT(0);  // NOT IMPLEMENTED
        }

    } // switch(language_enum)

   return type;
}


SgUntypedInitializedName*
buildInitializedName(const std::string & name, SgUntypedType* type, SgUntypedExpression* initializer)
{
   ROSE_ASSERT(type);

   SgUntypedInitializedName* initialized_name = new SgUntypedInitializedName(type, name);
   ROSE_ASSERT(initialized_name);
   SageInterface::setSourcePosition(initialized_name);

   if (initializer != NULL) {
      initialized_name->set_has_initializer(true);
      initialized_name->set_initializer(initializer);
   }

   return initialized_name;
}


SgUntypedInitializedNameList*
buildInitializedNameList(SgUntypedInitializedName* initialized_name)
{
   SgUntypedInitializedNameList* name_list = new SgUntypedInitializedNameList();
   ROSE_ASSERT(name_list);
   SageInterface::setSourcePosition(name_list);

   if (initialized_name != NULL)
      {
      // This case works when there is only one variable
         name_list->get_name_list().push_back(initialized_name);
      }

   return name_list;
}


SgUntypedVariableDeclaration*
buildVariableDeclaration(const std::string & name, SgUntypedType* type, SgUntypedExprListExpression* attr_list, SgUntypedExpression* initializer)
{
   ROSE_ASSERT(type);
   ROSE_ASSERT(attr_list);

   std::string label = "";

   bool has_base_type = false;
   SgUntypedDeclarationStatement* base_type_decl = NULL;

   SgUntypedInitializedName* initialized_name = buildInitializedName(name, type, initializer);
   ROSE_ASSERT(initialized_name);

   SgUntypedInitializedNameList* var_name_list = buildInitializedNameList(initialized_name);
   ROSE_ASSERT(var_name_list);

   SgUntypedVariableDeclaration*
   variable_decl = new SgUntypedVariableDeclaration(label, type, base_type_decl, has_base_type, attr_list, var_name_list);
   ROSE_ASSERT(variable_decl);
   SageInterface::setSourcePosition(variable_decl);

   return variable_decl;
}

SgUntypedVariableDeclaration*
buildVariableDeclaration(const std::string & name, SgUntypedType* type, SgUntypedStructureDeclaration* base_type_decl, SgUntypedExprListExpression* attr_list, SgUntypedExpression* initializer)
{
   ROSE_ASSERT(type);
   ROSE_ASSERT(base_type_decl);
   ROSE_ASSERT(attr_list);

   SgUntypedVariableDeclaration* variable_decl = buildVariableDeclaration(name, type, attr_list, initializer);
   ROSE_ASSERT(variable_decl != NULL);

   // Set the base-type declaration
   variable_decl->set_has_base_type(true);
   variable_decl->set_base_type_declaration(base_type_decl);

   return variable_decl;
}


SgUntypedNullExpression* buildUntypedNullExpression()
{
   SgUntypedNullExpression* expr = new SgUntypedNullExpression();
   ROSE_ASSERT(expr);
   SageInterface::setSourcePosition(expr);

   return expr;
}


} // namespace UntypedBuilder
