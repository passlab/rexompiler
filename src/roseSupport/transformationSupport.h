#ifndef ROSE_TRANSFORMATION_SUPPORT
#define ROSE_TRANSFORMATION_SUPPORT

// Access to Query Libraries
#include "roseQueryLib.h"

// include "nodeQuery.h"
// include "nameQuery.h"
// include "numberQuery.h"

 /*! \brief This class simplifies the development of queries on the AST resulting in a
            list of AST nodes.
     \ingroup transformationSupportLib

     This class holds functions that are generally useful for a broad cross-section of
     transformations.  Functionality specific to any specific transformation should be represented in
     a similar transformation support class that is a part of the preprocessor source code
     representing that specific transformation.
 */
class ROSE_DLL_API TransformationSupport
   {
     public:
      //! Get the string representing the function name (should be added to Sage III interface).
          static std::string getFunctionName ( SgFunctionCallExp* functionCallExp );

      //! Get the string representing the type name (should be added to Sage III interface).
          static std::string getTypeName ( SgType* type );

      /*! \brief Similar to getFunctionName(SgNode* astNode) above, but for member functions.

          For member fuinctions this returns the class name of the class for which this functon is a
          member and for non-member functions this function returns the name of the class associated
          with the return type (this might be done better).
       */
          static std::string getFunctionTypeName ( SgFunctionCallExp* functionCallExpression );

      /*! \brief wrap any string representing a macro into something that the unparser will unparse as a macro definition

          This mechanism is a hack for now since it embeds a substring into a 
          comment which the unparser presently will recognize.  This needs to 
          be fixed to work more cleanly.
       */
          static std::string buildMacro ( std::string s );

      /*! \brief Builds variable declarations for all variable and types defined in subtree at astNode.

          New variables and types introduced by way of a transformation need to be 
          defined in the application before they are used.  This sort of transofrmation 
          can be generally automated. 

          \internal There may be some cases where this mechanism can be outsmarted :-).
       */
          static std::string internalSupportingGlobalDeclarations( SgNode* astNode, std::string prefixString );

       // DQ (9/26/03) Added here to eliminate ProjectQueryLibrary 
       // (which as mostly an experiment with the reverse traversal)
          static SgProject* getProject( const SgNode* astNode);
          static SgDirectory* getDirectory( const SgNode* astNode);
          static SgFile* getFile( const SgNode* astNode);
          static SgSourceFile* getSourceFile( const SgNode* astNode);
          static SgGlobal* getGlobalScope( const SgNode* astNode);
          static SgStatement* getStatement(const SgNode* astNode);
          static SgFunctionDeclaration* getFunctionDeclaration( const SgNode* astNode);
          static SgFunctionDefinition* getFunctionDefinition( const SgNode* astNode);
          static SgClassDefinition* getClassDefinition( const SgNode* astNode);
          static SgModuleStatement* getModuleStatement( const SgNode* astNode);

       // DQ (7/25/2012): Modified to reflect new template design using different types of template IR nodes.
       // static SgTemplateDeclaration* getTemplateDeclaration( const SgNode* astNode);
          static SgDeclarationStatement* getTemplateDeclaration( const SgNode* astNode);

       // DQ (8/19/2014): Iterate back through the parents and scopes to find the SgType that the current node is embedded into.
      /*! \brief Find the associated type that might be a parent (through a parent chain) of the input SgNode.

          \internal Returns NULL if a SgStatement, SgFile, or SgProject is found (SgSymbol should never be found).
       */
          static SgType* getAssociatedType( const SgNode* astNode );

       // DQ (5/11/2011): This function was moved to SgTemplateArgument a long time ago and is not longer available.
       // DQ (6/9/2007): This function traverses through the parents to the first scope (used for name qualification support of template arguments)
       // static SgScopeStatement* getScope(const SgNode* astNode);

       // static SgValueExp* getValueExpr(const SgNode* astNode);
   };

// endif for ROSE_TRANSFORMATION_SUPPORT
#endif




















