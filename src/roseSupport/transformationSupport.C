
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "transformationSupport.h"
#include "nameQuery.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#if 0
void
TransformationSupport::insertDeclarations (
     SgProject & globalProject,
     SgNode* astNode, 
     const SynthesizedAttributeBaseClassType & gatheredData,
     string prefixString  )
   {
#if 1
     printf ("Exiting before call to gatheredData.insertGatheredStringData() \n");
     ROSE_ABORT();

  // gatheredData.insertGatheredStringData(astNode);
#else
     list<string> variableDeclarationStrings =
          gatheredData.variableDeclarationStrings.getSourceCodeStringList();

  // list<string> variableDeclarationStrings =
  //      gatheredData.transformationStrings.buildStringList(
  //           SourceCodeStringType::LocalScope,
  //           SourceCodeStringType::TopOfScope );

     string tempString = StringUtility::listToString(variableDeclarationStrings);

  // printf ("Case BASIC_BLOCK (before): tempString = \n%s\n",tempString.c_str());
     string variableDeclarations = StringUtility::removePseudoRedundentSubstrings(tempString);

  // printf ("Case BASIC_BLOCK: variableDeclarations = \n%s\n",variableDeclarations.c_str());

     string globalDeclarationCodeString =
          TransformationSupport::internalSupportingGlobalDeclarations( astNode, prefixString );
     string localDeclarationCodeString = "";

  // Need to mark declarations so that they can be extracted properly (but this should be done elsewhere)
     string sourceCodeString      = "int VARIABLE_DECLARATIONS_MARKER_START; \n" + 
                                    variableDeclarations +
                                    "\nint VARIABLE_DECLARATIONS_MARKER_END;\n";

#if 1
     printf ("Case BASIC_BLOCK: globalDeclarationCodeString = \n%s\n",globalDeclarationCodeString.c_str());
     printf ("Case BASIC_BLOCK: localDeclarationCodeString  = \n%s\n",localDeclarationCodeString.c_str());
     printf ("Case BASIC_BLOCK: sourceCodeString            = \n%s\n",sourceCodeString.c_str());
#endif

#if 0
     printf ("Exiting before call to basicBlock->insertSourceCode() \n");
     ROSE_ABORT();
#endif

  // Push the variable declarations for all the transformations onto the top of the local scope
     bool isADeclaration     = true;
     bool insertAtTopOfBlock = true;
     astNode->insertSourceCode (globalProject,
                                sourceCodeString,
                                localDeclarationCodeString,
                                globalDeclarationCodeString,
                                insertAtTopOfBlock,
                                isADeclaration);
#endif

   }
#endif

string
TransformationSupport::getFunctionName ( SgFunctionCallExp* functionCallExp )
   {
  // SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     ROSE_ASSERT (functionCallExp != NULL);

     SgExpression* expression = functionCallExp->get_function();
     ROSE_ASSERT (expression != NULL);

     string returnNameString;

     SgDotExp* dotExp = isSgDotExp(expression);
     if (dotExp != NULL)
        {
          ROSE_ASSERT (dotExp != NULL);

          SgExpression* rhsOperand = dotExp->get_rhs_operand();
          ROSE_ASSERT (rhsOperand != NULL);

          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsOperand);
          ROSE_ASSERT (memberFunctionRefExp != NULL);

          SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(memberFunctionRefExp->get_symbol());
          ROSE_ASSERT (memberFunctionSymbol != NULL);

       // There will be a lot of different possible overloaded operators called
       // and "operator()" is just one of them (that we are debugging presently)

       // returnNameString = Rose::stringDuplicate(memberFunctionSymbol->get_name().str());
          returnNameString = memberFunctionSymbol->get_name().str();
          ROSE_ASSERT (returnNameString.length() > 0);
        }

     SgFunctionRefExp* functionReferenceExp = isSgFunctionRefExp(expression);
     if (functionReferenceExp != NULL)
       {
         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(functionReferenceExp->get_symbol());
         ROSE_ASSERT (functionSymbol != NULL);
         
         // returnNameString = Rose::stringDuplicate(functionSymbol->get_name().str());
         returnNameString = functionSymbol->get_name().str();
         
         // ROSE_ASSERT (returnNameString != NULL);
         // printf ("SgFunctionRefExp case: returnNameString = %s \n",returnNameString);
         
         // printf ("Not implemented case in getFunctionName \n");
         // ROSE_ABORT();
         ROSE_ASSERT (returnNameString.length() > 0);
       }

     SgArrowExp* arrowExp = isSgArrowExp(expression);
     if ( arrowExp != NULL)
       {
         ROSE_ASSERT (arrowExp != NULL);
         
         SgExpression* rhsOperand = arrowExp->get_rhs_operand();
         ROSE_ASSERT (rhsOperand != NULL);
         
         SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsOperand);
         ROSE_ASSERT (memberFunctionRefExp != NULL);
         
         SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(memberFunctionRefExp->get_symbol());
         ROSE_ASSERT (memberFunctionSymbol != NULL);
         
         // There will be a lot of different possible overloaded operators called
         // and "operator()" is just one of them (that we are debugging presently)
         
         // returnNameString = Rose::stringDuplicate(memberFunctionSymbol->get_name().str());
         returnNameString = memberFunctionSymbol->get_name().str();
         ROSE_ASSERT (returnNameString.length() > 0);
       }
     
     ROSE_ASSERT (returnNameString.length() > 0);
     // printf ("returnNameString = %s \n",returnNameString.c_str());
     
     return returnNameString;
   }


// DQ (3/18/2005): Improved version from Brian White (merged bugfix from Tom Epperly as well
string
TransformationSupport::getTypeName ( SgType* type )
   {
     string typeName;

  // printf ("In TransformationSupport::getTypeName(): type->sage_class_name() = %s \n",type->sage_class_name());

     switch (type->variantT())
        {
          case V_SgTypeComplex:
               typeName = "complex";
               break;
          case V_SgTypeImaginary:
               typeName = "imaginary";
               break;
          case V_SgTypeBool:
               typeName = "bool";
               break;
#if 0
          case V_SgEnumType:
            // DQ (3/2/2005): This needs to be fixed, but Tom is going to send me the fix since I'm working on Kull presently.
               typeName = "enum";
               break;
#endif
          case V_SgTypeChar:
               typeName = "char";
               break;
          case V_SgTypeVoid:
               typeName = "void";
               break;
          case V_SgTypeInt:
               typeName = "int";
               break;
          case V_SgTypeDouble:
               typeName = "double";
               break;
          case V_SgTypeFloat:
               typeName = "float";
               break;
          case V_SgTypeLong:
               typeName = "long";
               break;
          case V_SgTypeLongDouble:
               typeName = "long double";
               break;
          case V_SgTypeEllipse:
               typeName = "ellipse";
               break;
          case V_SgTypeGlobalVoid:
               typeName = "void";
               break;
          case V_SgTypeLongLong:
               typeName = "long long";
               break;
          case V_SgTypeShort:
               typeName = "short";
               break;
          case V_SgTypeSignedChar:
               typeName = "signed char";
               break;
          case V_SgTypeSignedInt:
               typeName = "signed int";
               break;
          case V_SgTypeSignedLong:
               typeName = "signed long";
               break;
          case V_SgTypeSignedShort:
               typeName = "signed short";
               break;
          case V_SgTypeString:
               typeName = "string";
               break;
          case V_SgTypeUnknown:
               typeName = "unknown";
               break;
          case V_SgTypeUnsignedChar:
               typeName = "unsigned char";
               break;
          case V_SgTypeUnsignedInt:
               typeName = "unsigned int";
               break;
          case V_SgTypeUnsignedLong:
               typeName = "unsigned long";
               break;
          case V_SgTypeUnsignedShort:
               typeName = "unsigned short";
               break;
          case V_SgTypeUnsignedLongLong:
               typeName = "unsigned long long";
               break;
          case V_SgReferenceType: 
             {
               ROSE_ASSERT ( isSgReferenceType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgReferenceType(type)->get_base_type()) + "&";
               break;
             }
          case V_SgPointerType:
             {
               ROSE_ASSERT ( isSgPointerType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgPointerType(type)->get_base_type()) + "*";
               break;
             }
          case V_SgModifierType:
             {
               ROSE_ASSERT ( isSgModifierType(type)->get_base_type() != NULL );
               SgModifierType *modifier = isSgModifierType(type);
               typeName = modifier->unparseToString();
            // typeName = getTypeName(modifier->get_base_type());
               break;
             }
          case V_SgEnumType:
          case V_SgNamedType:
             {
               SgNamedType* sageNamedType = isSgNamedType(type);
               ROSE_ASSERT( sageNamedType != NULL );
               typeName = sageNamedType->get_name().str();
               break;
             }
          case V_SgClassType:
             {
               SgClassType* sageClassType = isSgClassType(type);
               ROSE_ASSERT( sageClassType != NULL );
               typeName = sageClassType->get_name().str();
               break;
             }
          case V_SgTypedefType:
             {
               SgTypedefType* sageTypedefType = isSgTypedefType(type);
               ROSE_ASSERT( sageTypedefType != NULL );
               typeName = sageTypedefType->get_name().str();
               break;
             }
          case V_SgPointerMemberType:
             {
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(type);
               ROSE_ASSERT (pointerMemberType != NULL);
               SgClassType* classType = isSgClassType(pointerMemberType->get_class_type()->stripTypedefsAndModifiers());
               ROSE_ASSERT (classType != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT (classDeclaration != NULL);
               typeName = classDeclaration->get_name().str();
               break;
             }
          case V_SgArrayType: 
             {
               ROSE_ASSERT ( isSgArrayType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgArrayType(type)->get_base_type());
               break;
             }
          case V_SgFunctionType:
             {
               SgFunctionType* functionType = isSgFunctionType(type);
               ROSE_ASSERT(functionType != NULL);
               typeName = functionType->get_mangled_type().str();
               break;
             }
          case V_SgMemberFunctionType:
             {
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(type);
               ROSE_ASSERT (memberFunctionType != NULL);
               SgClassType* classType = isSgClassType(memberFunctionType->get_class_type()->stripTypedefsAndModifiers());
               ROSE_ASSERT (classType != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT (classDeclaration != NULL);
               typeName = classDeclaration->get_name().str();
               break;
             }
          case V_SgTypeWchar:
               typeName = "wchar";
               break;
          case V_SgTypeDefault:
               typeName = "default";
               break;
          default:
             {
               printf ("default reached in switch within TransformationSupport::getTypeName type->sage_class_name() = %s variant = %d \n",
               type->sage_class_name(),type->variant());
               ROSE_ABORT();
               break;
             }
        }

  // Fix for purify problem report
  // typeName = Rose::stringDuplicate(typeName);

     return typeName;
   }


string
TransformationSupport::getFunctionTypeName ( SgFunctionCallExp* functionCallExpression )
   {
     string associatedClassName;

  // SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
     ROSE_ASSERT (functionCallExpression != NULL);

  // string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );
  // ROSE_ASSERT (operatorName.c_str() != NULL);

     SgExpression* expression = functionCallExpression->get_function();
     ROSE_ASSERT (expression != NULL);

     string functionTypeName;
     SgDotExp* dotExpression = isSgDotExp(expression);
     SgArrowExp* arrowExpression = isSgArrowExp(expression);
     if (dotExpression != NULL)
        {
       // Get the class name associated with the member function
          SgExpression* rhsExpression = dotExpression->get_rhs_operand();
          ROSE_ASSERT (rhsExpression != NULL);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsExpression);
          ROSE_ASSERT (memberFunctionRefExp != NULL);

       // DQ (1/14/2006): Call get_type() instead of get_function_type() (part of work
       // to remove the explicit storage of the type within expressions were possible).
       // ROSE_ASSERT (memberFunctionRefExp->get_function_type() != NULL);
          ROSE_ASSERT (memberFunctionRefExp->get_type() != NULL);

       // DQ (1/14/2006): Call get_type() instead of get_function_type() (part of work
       // to remove the explicit storage of the type within expressions were possible).
       // SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_function_type());
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_type());

          ROSE_ASSERT (memberFunctionType != NULL);
       // functionTypeName = Rose::stringDuplicate(TransformationSupport::getTypeName ( memberFunctionType ));
          functionTypeName = TransformationSupport::getTypeName ( memberFunctionType );
        }
     else if ( arrowExpression != NULL )
        {
       // Get the class name associated with the member function
          SgExpression* rhsExpression = arrowExpression->get_rhs_operand();
          ROSE_ASSERT (rhsExpression != NULL);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsExpression);
          ROSE_ASSERT (memberFunctionRefExp != NULL);

       // DQ (1/14/2006): Call get_type() instead of get_function_type() (part of work
       // to remove the explicit storage of the type within expressions were possible).
       // ROSE_ASSERT (memberFunctionRefExp->get_function_type() != NULL);
       // SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_function_type());
          ROSE_ASSERT (memberFunctionRefExp->get_type() != NULL);
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_type());

          ROSE_ASSERT (memberFunctionType != NULL);
       // functionTypeName = Rose::stringDuplicate(TransformationSupport::getTypeName ( memberFunctionType ));
          functionTypeName = TransformationSupport::getTypeName ( memberFunctionType );
       }
       else
        {
       // Get the class name associated with the friend function
          SgType* functionType    = functionCallExpression->get_type();
          ROSE_ASSERT (functionType != NULL);
          functionTypeName = TransformationSupport::getTypeName (functionType);
        }

     associatedClassName = functionTypeName;
     ROSE_ASSERT (associatedClassName.length() > 0);

     return associatedClassName;
   }


string
TransformationSupport::buildMacro ( string s )
   {
  // This function wraps the string representing a macro that we want to appear in a transformation
  // so that it can be precessed by the unparser.

#if 0
     char* returnString = Rose::stringDuplicate(s);
     char* roseMacroMarkerString = "ROSE-TRANSFORMATION-MACRO:";
     returnString = Rose::stringConcatinate (roseMacroMarkerString,returnString);

  // Wrap the string into quotes for use as a C++ string expression
     returnString = Rose::stringConcatinate ("\"",returnString);
     returnString = Rose::stringConcatinate (returnString,"\";");
     ROSE_ASSERT (returnString != NULL);
#else
     string returnString = s;
     string roseMacroMarkerString = "ROSE-TRANSFORMATION-MACRO:";
     returnString = roseMacroMarkerString + returnString;

  // Wrap the string into quotes for use as a C++ string expression
     returnString = "\"" + returnString;
     returnString = returnString + "\";";
#endif

     return returnString;
   }

#if 0
// DQ (3/26/2004): I don't think this is used any more experiment with removing this
string
TransformationSupport::internalSupportingGlobalDeclarations (
     SgNode* astNode, string prefixString )
   {
  // This function generates a string that is used in the function which assembles the strings into
  // code that is called internally with the front-end to generate an AST fragements (which is
  // patched into the AST to introduce the transformation).

  // NOTE: This extra code is required to allow the string representing the transformation to be
  // compiled.  Once it is compiled, the AST is searched and only the AST fragment representing the
  // transformation is extracted (not the AST framents representing any of the code specified in
  // this function).

     printf ("In TransformationSupport::internalSupportingGlobalDeclarations(): astNode->unparseToString() = \n%s\n",astNode->unparseToString().c_str());

  // The use of a include file simplifies and shortens the 
  // declaration section of the intermediate (generated) file
     string staticStringSourceCodeTemplate = "\
\n\
$VARIABLE_DECLARATIONS\n\n\
";

     staticStringSourceCodeTemplate = prefixString + staticStringSourceCodeTemplate;

  // We need to have a string built from dynamically allocated memory using 
  // the C++ new operator since it will be deleted in the copyEdit() function
  // This avoids a purify error (so we have to call stringDuplicate())
     string sourceCodeTemplate = staticStringSourceCodeTemplate;

  // Variable declarations will have this form
     string staticVariableDeclarationString = "$TYPE_NAME $VARIABLE_NAME; \n";

  // The start of the variable declaration section of code begins with the following comment (this
  // string is appended with the actual variable declarations).
     string variableDeclarationString =
          "// Variables used in this transformation (automatically generated from simple dependence analysis of original code before transformation) \n";
     
     NameQuery::TypeOfQueryTypeOneParameter sageOneParameterEnumType = NameQuery::VariableTypeNames;
  // Nested Query:
  // Generate the list of types used within the target subtree of the AST
     list<string> typeNameStringList = NameQuery::querySubTree( astNode, sageOneParameterEnumType);

  // Loop over all the types and get list of variables of each type
  // (so they can be declared properly when the transformation is compiled)
     list<string>::iterator typeListStringElementIterator;
     for (typeListStringElementIterator = typeNameStringList.begin();
          typeListStringElementIterator != typeNameStringList.end();
          typeListStringElementIterator++)
        {
       // printf ("Type = %s \n",(*typeListStringElementIterator).c_str());

          
       // Find a list of names of variable of type (*listStringElementIterator)
          NameQuery::TypeOfQueryTypeTwoParameters sageTwoParametersEnumType = NameQuery::VariableNamesWithTypeName;
          list<string> operandNameStringList =
               NameQuery::querySubTree(astNode, *typeListStringElementIterator, sageTwoParametersEnumType );

       // Loop over all the types and get list of variable of each type
          list<string>::iterator variableListStringElementIterator;
          for (variableListStringElementIterator = operandNameStringList.begin();
               variableListStringElementIterator != operandNameStringList.end();
               variableListStringElementIterator++)
             {
#if 0
               printf ("Type = %s Variable = %s \n",
                    (*typeListStringElementIterator).c_str(),
                    (*variableListStringElementIterator).c_str());
#endif

               string variableName = Rose::stringDuplicate((*variableListStringElementIterator).c_str());
               string typeName = Rose::stringDuplicate((*typeListStringElementIterator).c_str());
               string localOperandDataTemplate =
                    SgNode::copyEdit (staticVariableDeclarationString,"$VARIABLE_NAME",variableName);
               localOperandDataTemplate =
                    SgNode::copyEdit (localOperandDataTemplate,"$TYPE_NAME",typeName);

            // Append the new variable declaration to the variableDeclarationString
            // variableDeclarationString = Rose::stringConcatinate (variableDeclarationString,localOperandDataTemplate);
               variableDeclarationString =  variableDeclarationString + localOperandDataTemplate;
             }
#if 0
          printf ("Exiting in loop internalSupportingGlobalDeclarations (type = %s) ... \n",(*typeListStringElementIterator).c_str());
          ROSE_ABORT();
#endif
        }

#if 0
     printf ("Exiting at base of internalSupportingGlobalDeclarations ... \n");
     ROSE_ABORT();
#endif

  // Substitute the code fragment representing variable declaration into the generated source code
  // representing the specification of the transformation.
     string finalSourceCodeString = SgNode::copyEdit ( sourceCodeTemplate, "$VARIABLE_DECLARATIONS" , variableDeclarationString );

     return finalSourceCodeString;
   }
#endif


// ***********************************************
// DQ (9/26/2003) Function that might be moved to SgNode (eventually)
// This is a sequence of functions which return the root nodes of different parts 
// of the AST,  we could imagine additional function like these which returned the 
// declaration appearing in global scope or more specifially the function scope
// (if the input node was in the subtree of a function) or the class declaration 
// (if the input node was in the subtree of a class declaration), etc.
// ***********************************************
SgProject*
TransformationSupport::getProject( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
  // printf ("Starting at parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
     while (parentNode->get_parent() != NULL)
        {
          parentNode = parentNode->get_parent();
       // printf ("     parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgProject(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          if (astNode == NULL)
             {
               printf ("Warning: could not trace back to SgProject node. \n");
             }
            else
             {
            // DQ (7/30/2010): This can be allowed for the expression in a SgArrayType!
               printf ("Warning: could not trace back to SgProject node from %s \n",astNode->class_name().c_str());
             }
       // ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgProject from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }
#if 0
        if (isSgProject(parentNode) == NULL &&
                        dynamic_cast<const SgType*> (parentNode) == NULL &&
                        dynamic_cast<const SgSymbol*> (parentNode) == NULL)
        {
                if (SgProject::get_verbose() > 0)
                {
                        if (astNode == NULL)
                                printf("Warning: could not trace back to SgProject node. \n");
                        else
                                printf("Warning: could not trace back to SgProject node from %s \n",
                                        astNode->class_name().c_str());
                }
        }
        else
        {
                if (dynamic_cast<const SgType*> (parentNode) != NULL || dynamic_cast<const SgSymbol*> (parentNode) != NULL)
                {
                        printf("Error: can't locate an associated SgProject from astNode = %p = %s parentNode = %p = %s \n", astNode, astNode->class_name().c_str(), parentNode, parentNode->class_name().c_str());
                        return NULL;
                }
        }
#endif

  // Make sure we have a SgProject node
     const SgProject* project = isSgProject(parentNode);
     ROSE_ASSERT (project != NULL);

  // return project;
     return const_cast<SgProject*>(project);
   }

SgDirectory*
TransformationSupport::getDirectory( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgDirectory(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgDirectory(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgDirecoty node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // DQ (9/2/2014): Only output this message if this is not in a SgArrayType.
               if (isSgArrayType(parentNode) == NULL)
                 {
                // DQ (3/6/2017): Converted to use message logging.
                   MLOG_WARN_C("roseSupport", "Can't locate an associated SgFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                 }
               return NULL;
             }
        }

  // Make sure we have a SgFile node
     const SgDirectory* directory = isSgDirectory(parentNode);

     return const_cast<SgDirectory*>(directory);
   }

SgFile*
TransformationSupport::getFile( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgFile(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFile(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgFile node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // DQ (9/2/2014): Only output this message if this is not in a SgArrayType.
               if (isSgArrayType(parentNode) == NULL)
                 {
                // DQ (3/6/2017): Converted to use message logging.
                   MLOG_WARN_C("roseSupport", "Can't locate an associated SgFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                 }
               return NULL;
             }
        }


  // Make sure we have a SgFile node
     const SgFile* file = isSgFile(parentNode);

  // DQ (8/2/2005): Allow to return NULL
  // ROSE_ASSERT (file != NULL);

  // return file;
     return const_cast<SgFile*>(file);
   }

// DQ (9/3/2008): This used to use SgFile and was switched to use SgSourceFile.
SgSourceFile*
TransformationSupport::getSourceFile( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgSourceFile(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgSourceFile(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgSourceFile node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // DQ (3/4/2009): This test will remove most of the instances of this warning (ignoring types).
               if (isSgType(parentNode) == NULL)
                  {
                    printf ("Warning: can't locate an associated SgSourceFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                  }
               return NULL;
             }
        }


  // Make sure we have a SgSourceFile node
     const SgSourceFile* file = isSgSourceFile(parentNode);

  // DQ (8/2/2005): Allow to return NULL
  // ROSE_ASSERT (file != NULL);

  // return file;
     return const_cast<SgSourceFile*>(file);
   }

SgGlobal*
TransformationSupport::getGlobalScope( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;

  // printf ("TransformationSupport::getGlobalScope(): Starting node: parentNode = %p = %s \n",parentNode,parentNode->class_name().c_str());
     while ( (isSgGlobal(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
       // printf ("parentNode = %p = %s \n",parentNode,parentNode->class_name().c_str());
        }

     ROSE_ASSERT(parentNode != NULL);

  // DQ (7/24/2010): Handle the case of an expression in an array type.
     if (isSgArrayType(parentNode) != NULL)
        {
       // printf ("TransformationSupport::getGlobalScope(): Case of expression in SgArrayType \n");
          return NULL;
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgGlobal(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {  //It is possible to find no SgGlobal during transformation, changed to warning.
          if (astNode != NULL)
             {
            // DQ (7/30/2010): This can be allowed for the expression in a SgArrayType!
            // printf ("Warning: could not trace back to SgGlobal node from %s (parentNode = %p = %s) \n",astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
             }
            else
             {
               printf ("Warning: could not trace back to SgGlobal node\n ");
             }
       // ROSE_ASSERT(false);
          return NULL;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // printf ("Error: can't locate an associated SgGlobal from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }
#if 0
        if (isSgGlobal(parentNode) == NULL &&
                        dynamic_cast<const SgType*> (parentNode) == NULL &&
                        dynamic_cast<const SgSymbol*> (parentNode) == NULL)
        {
                //It is possible to find no SgGlobal during transformation, changed to warning.
                if (SgProject::get_verbose() > 0)
                {
                        if (astNode != NULL)
                                printf("Warning: could not trace back to SgGlobal node from %s \n",
                                        astNode->class_name().c_str());
                        else
                                printf("Warning: could not trace back to SgGlobal node\n ");
                }
                return NULL;
        }
        else
        {
                if (dynamic_cast<const SgType*> (parentNode) != NULL || dynamic_cast<const SgSymbol*> (parentNode) != NULL)
                {
                        // printf ("Error: can't locate an associated SgGlobal from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                        return NULL;
                }
        }
#endif

  // Make sure we have a SgGlobal node
     const SgGlobal* globalScope = isSgGlobal(parentNode);
     ROSE_ASSERT (globalScope != NULL);

  // return globalScope;
     return const_cast<SgGlobal*>(globalScope);
   }

SgStatement*
TransformationSupport::getStatement( const SgNode* astNode )
   {
  // DQ (3/1/2009): Added assertion.
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;

  // DQ (6/27/2007): These IR nodes are not contained in any statement
     if (isSgProject(astNode) != NULL || isSgFile(astNode) != NULL)
          return NULL;

  // DQ (7/24/2010): Handle the case of an expression in an array type.
     if (parentNode->get_parent() != NULL && isSgArrayType(parentNode->get_parent()) != NULL)
        {
       // printf ("TransformationSupport::getStatement(): Case of expression in SgArrayType \n");
          return NULL;
        }

     while ( (isSgStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

     ROSE_ASSERT(parentNode != NULL);

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          if (astNode == NULL)
             {
               printf ("Error: could not trace back to SgStatement node \n");
             }
            else
             {
            // DQ (7/30/2010): This can be allowed for the expression in a SgArrayType!
            // printf ("Warning: could not trace back to SgStatement node from %s (parentNode = %p = %s) \n",astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
             }

       // ROSE_ABORT();
          return NULL;
        }
#if 0
          dynamic_cast<const SgSymbol*>(parentNode) == NULL)
        {
                if (SgProject::get_verbose() > 0)
                {
                        if (astNode == NULL)
                                printf("Error: could not trace back to SgStatement node \n");
                        else
                                printf("Warning: could not trace back to SgStatement node from %s \n", astNode->class_name().c_str());
                }

                return NULL;
        }
#endif
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // Test for SgArrayType since a value if often hidden there and it is not possible to traverse 
            // through a SgType along parent IR nodes.
               if ( dynamic_cast<const SgArrayType*>(parentNode) != NULL )
                  {
                    return NULL;
                  }

            // DQ (11/10/2007): Note that for an AST fragment (e.g. expression) not connected to the AST, this function will return NULL.
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
#endif
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgStatement* statement = isSgStatement(parentNode);
     ROSE_ASSERT (statement != NULL);

  // return statement;
     return const_cast<SgStatement*>(statement);
   }


SgType*
TransformationSupport::getAssociatedType( const SgNode* astNode )
   {
  // DQ (8/19/2014): Iterate back through the parents and scopes to find the SgType that the current node is embedded into.

     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;

  // DQ (6/27/2007): These IR nodes are not contained in any statement
     if (isSgProject(astNode) != NULL || isSgFile(astNode) != NULL)
          return NULL;

  // DQ (7/24/2010): Handle the case of an expression in an array type.
     SgArrayType* arrayType = isSgArrayType(parentNode->get_parent());
     if (parentNode->get_parent() != NULL && arrayType != NULL)
        {
#if 1
          printf ("TransformationSupport::getAssociatedType(): Case of expression in SgArrayType: arrayType = %p \n",arrayType);
#endif
          return arrayType;
        }

     while ( (isSgStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

     ROSE_ASSERT(parentNode != NULL);

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          if (astNode == NULL)
             {
               printf ("Error: could not trace back to SgStatement node \n");
             }
            else
             {
            // DQ (7/30/2010): This can be allowed for the expression in a SgArrayType!
            // printf ("Warning: could not trace back to SgStatement node from %s (parentNode = %p = %s) \n",astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
             }

       // ROSE_ABORT();
          return NULL;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // Test for SgArrayType since a value if often hidden there and it is not possible to traverse 
            // through a SgType along parent IR nodes.
               if ( dynamic_cast<const SgArrayType*>(parentNode) != NULL )
                  {
                 // const SgArrayType* arrayType = isSgArrayType(parentNode);
                 // SgArrayType* arrayType = const_cast<SgArrayType*>(parentNode);
                    SgNode* tmp_node = const_cast<SgNode*>(parentNode);
                    SgArrayType* arrayType = isSgArrayType(tmp_node);
                    return arrayType;
                  }

            // DQ (11/10/2007): Note that for an AST fragment (e.g. expression) not connected to the AST, this function will return NULL.
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
#endif
            // SgType* possibleOtherType = isSgType(parentNode);
               SgNode* other_node = const_cast<SgNode*>(parentNode);
               SgType* possibleOtherType = isSgType(other_node);
               return possibleOtherType;
             }
        }

  // Make sure we have a SgStatement node
     const SgStatement* statement = isSgStatement(parentNode);
     ROSE_ASSERT (statement != NULL);

  // DQ (8/19/2014): If we did find a statement then return NULL (since no SgType was found).
  // return statement;
  // return const_cast<SgStatement*>(statement);
     return NULL;
   }


SgFunctionDeclaration*
TransformationSupport::getFunctionDeclaration( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgFunctionDeclaration(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFunctionDeclaration(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
#if 0
          if (astNode == NULL)
               printf ("Error: could not trace back to SgFunctionDeclaration node \n");
            else
               printf ("Warning: could not trace back to SgFunctionDeclaration node from %s \n",astNode->class_name().c_str());
          ROSE_ABORT();
#endif

       // DQ (12/27/2010): This should not be an error (OK to return NULL).
          return NULL;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgFunctionDeclaration from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }


  // Make sure we have a SgFunctionDeclaration node
     const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentNode);
     ROSE_ASSERT (functionDeclaration != NULL);

  // return functionDeclaration;
     return const_cast<SgFunctionDeclaration*>(functionDeclaration);
   }

SgFunctionDefinition*
TransformationSupport::getFunctionDefinition( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;

     ROSE_ASSERT(astNode != NULL);
#if 0
     printf ("Note: astNode = %p = %s parent = %p \n",astNode,astNode->class_name().c_str(),astNode->get_parent());
#endif

     while ( (isSgFunctionDefinition(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
#if 0
          printf ("Note: parentNode = %p = %s \n",parentNode,parentNode->class_name().c_str());
#endif
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFunctionDefinition(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          if(astNode==NULL)
               printf ("Error: could not trace back to SgFunctionDefinition node \n");
            else 
               printf ("Warning: could not trace back to SgFunctionDefinition node from %s \n",astNode->class_name().c_str());
#if 0
          printf ("Error: could not trace back to SgFunctionDefinition node \n");
          else 
               printf ("Error: could not trace back to SgFunctionDefinition node from %s \n",astNode->class_name().c_str());
#endif
       // ROSE_ABORT();
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgFunctionDefinition from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgFunctionDefinition* returnFunctionScope = isSgFunctionDefinition(parentNode);
     ROSE_ASSERT (returnFunctionScope != NULL);

  // return class definition statement;
     return const_cast<SgFunctionDefinition*>(returnFunctionScope);
   }

SgClassDefinition*
TransformationSupport::getClassDefinition( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgClassDefinition(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgClassDefinition(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
#if 0
          printf ("Error: could not trace back to SgClassDefinition node \n");
          ROSE_ABORT();
#else
       // DQ (12/11/2010): It is OK to return NULL from this function (used fortran_support.C
       // SgFunctionRefExp* generateFunctionRefExp(Token_t* nameToken) for fortran support).
          return NULL;
#endif
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgClassDefinition from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgClassDefinition* returnClassScope = isSgClassDefinition(parentNode);
     ROSE_ASSERT (returnClassScope != NULL);

  // return class definition statement;
     return const_cast<SgClassDefinition*>(returnClassScope);
   }

SgModuleStatement*
TransformationSupport::getModuleStatement( const SgNode* astNode)
   {
  // DQ (11/24/2007): This function supports the unparsing of the PUBLIC, PRIVATE keywords (only permitted within modules)
  // The name of this function might change to getModuleDeclaration
     const SgNode* parentNode = astNode;
     while ( (isSgModuleStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgModuleStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL)
        {
                if (SgProject::get_verbose() > 0)
                {
                        printf("Warning: could not trace back to SgModuleStatement node \n");
                }
                return NULL;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgModuleStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgModuleStatement* returnModuleDeclaration = isSgModuleStatement(parentNode);
     ROSE_ASSERT (returnModuleDeclaration != NULL);

     return const_cast<SgModuleStatement*>(returnModuleDeclaration);
   }


// SgTemplateDeclaration*
SgDeclarationStatement*
TransformationSupport::getTemplateDeclaration( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;

#if 0
     printf ("In TransformationSupport::getTemplateDeclaration(): astNode = %p = %s \n",astNode,astNode != NULL ? astNode->class_name().c_str() : "NULL");
#endif

  // DQ (7/25/2012): Updated to reflect new template design using different types or template IR nodes.
  // while ( (isSgTemplateDeclaration(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
     while ( (isSgTemplateDeclaration(parentNode) == NULL)         && (isSgTemplateClassDeclaration(parentNode) == NULL) && 
             (isSgTemplateFunctionDeclaration(parentNode) == NULL) && (isSgTemplateMemberFunctionDeclaration(parentNode) == NULL) && 
             (isSgTemplateVariableDeclaration(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
#if 0
          printf ("In TransformationSupport::getTemplateDeclaration(): parentNode = %p = %s \n",parentNode,parentNode != NULL ? parentNode->class_name().c_str() : "NULL");
#endif
        }

  // DQ (7/25/2012): Updated to reflect new template design using different types or template IR nodes.
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgTemplateDeclaration(parentNode) == NULL &&
          isSgTemplateClassDeclaration(parentNode) == NULL &&
          isSgTemplateFunctionDeclaration(parentNode) == NULL &&
          isSgTemplateMemberFunctionDeclaration(parentNode) == NULL &&
          isSgTemplateVariableDeclaration(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
#if 1
          if (astNode == NULL)
             {
               printf ("Error: could not trace back to SgTemplateDeclaration node \n");
               ROSE_ABORT();
             }
            else
             {
               printf ("Warning: In TransformationSupport::getTemplateDeclaration(): could not trace back to template declaration node from %s \n",astNode->class_name().c_str());
            // ROSE_ASSERT(false);
             }
       // DQ (6/6/2013): commented this out since it is OK to return NULL (I think).
       // ROSE_ASSERT(false);
#endif

       // DQ (12/27/2010): This should not be an error (OK to return NULL).
          return NULL;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgTemplateDeclaration from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgFunctionDeclaration node
  // const SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(parentNode);
     const SgDeclarationStatement* templateDeclaration = isSgDeclarationStatement(parentNode);
     ROSE_ASSERT (templateDeclaration != NULL);

  // return const_cast<SgTemplateDeclaration*>(templateDeclaration);
     return const_cast<SgDeclarationStatement*>(templateDeclaration);
   }



#if 0
// DQ (11/24/2020): corrected this comment.
// Moved to sageInterface.C file
SgScopeStatement*
TransformationSupport::getScope( const SgNode* astNode )
   {

#error "DEAD CODE!"

  // DQ (6/9/2007): This function traverses through the parents to the first scope (used for name qualification support of template arguments)

     const SgNode* parentNode = astNode;
     while ( (isSgScopeStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

#error "DEAD CODE!"

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgScopeStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          printf ("Error: In TransformationSupport::getScope(): could not trace back to SgScopeStatement node \n");
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

#error "DEAD CODE!"

  // Make sure we have a SgStatement node
     const SgScopeStatement* scopeStatement = isSgScopeStatement(parentNode);
     ROSE_ASSERT (scopeStatement != NULL);

  // return statement;
     return const_cast<SgScopeStatement*>(scopeStatement);
   }
#endif










