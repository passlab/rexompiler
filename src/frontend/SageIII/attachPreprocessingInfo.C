
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "attachPreprocessingInfo.h"

// DQ (1/7/2021): Added to support testing of the token stream availability.
#include "tokenStreamMapping.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

#include <filesystem>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
// #ifndef USE_ROSE
// Local typedefs used in this file only...
typedef boost::wave::cpplexer::lex_token<>  token_type;
typedef std::vector<token_type>             token_container;
typedef std::list<token_type>               token_list_container;
typedef std::vector<std::list<token_type> > token_container_container;
// #endif
#endif

// DQ (11/28/2009): I think this is equivalent to "USE_ROSE"
// DQ (11/28/2008): What does this evaluate to???  Does this mix C++ constants with CPP values (does this make sense? Is "true" defined?)
// #if CAN_NOT_COMPILE_WITH_ROSE != true
// #if !CAN_NOT_COMPILE_WITH_ROSE
#ifndef USE_ROSE
#endif

#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <boost/wave.hpp>
///////////////////////////////////////////////////////////////////////////////
// Include the lexer stuff
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

#include "advanced_preprocessing_hooks.h"
#include "attributeListMap.h"
#endif

#include <boost/filesystem.hpp>         // exsits()

//Include files to get the current path
#include <unistd.h>

#ifndef _MSC_VER
#include <sys/param.h>
#endif

// #include <iostream>
// #include <fstream>
// #include <string>

// DQ (11/11/2018): Added prototype to support debugging.
void generateGraphOfIncludeFiles( SgSourceFile* sourceFile, std::string filename );


#if 0
// DQ (11/30/2008): This does not appear to be used!
std::list<SgNode*>
findNodes(SgNode* astNode)
   {
     std::list<SgNode*> returnList;

     if(isSgFile(astNode)!=NULL)
          returnList.push_back(astNode);

     return returnList;
   }
#endif

#if 0
// DQ (12/16/2008): comment out while I debug the non-wave support.

// AS (011306) Support for Wave preprocessor
void
attachPreprocessingInfo(SgSourceFile *sageFilePtr, std::map<std::string,ROSEAttributesList*>* attrMap)
   {
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Comment Processing (using Wave, inner part):");

  // Dummy attribute
     AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh;
  // AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh(NULL);

  // Make sure that the argument is not a NULL pointer
     ROSE_ASSERT(sageFilePtr);

  // DQ (12/16/2008): comment out while I debug the non-wave support.
#if 0
  // Create tree traversal object for attaching the preprocessing information (using Wave)
     AttachPreprocessingInfoTreeTrav tt(attrMap);

  // Run tree traversal on specified source file
     tt.traverseWithinFile(sageFilePtr,inh);
#else
     printf ("Wave support not implemented in new support for CPP directives and comment handling. \n");
     ROSE_ABORT();
#endif
   }
#endif


// DQ (5/4/2020): Added directly here because it is required for this function.
typedef std::map<int, ROSEAttributesList*> AttributeMapType;


// DQ (12/3/2020): We sometimes want to read a file twice, and gather the comments 
// and CPP directives twice, but the second time the file is read it is read so that 
// it can build a file with a different name. So we need to specify the name of the
// file that we want the comments and CPP directives to eventually be attached to 
// and not the one from which they were take.  This technique is used to support
// building a second file to be a dynamic library within the codeSegregation tool.
// DQ (4/5/2006): Older version not using Wave preprocessor
// This is the function to be called from the main function
// DQ: Now called by the SgFile constructor body (I think)
// void attachPreprocessingInfo(SgSourceFile *sageFilePtr)
void attachPreprocessingInfo(SgSourceFile *sageFilePtr, const std::string & new_filename )
   {
     ROSE_ASSERT(sageFilePtr != NULL);

  // DQ (02/20/2021): Using the performance tracking within ROSE.
     TimingPerformance timer_1 ("AST attachPreprocessingInfo:");

#define DEBUG_ATTACH_PREPROCESSOR_INFO 0

#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("################################################################ \n");
     printf ("################################################################ \n");
     printf ("In attachPreprocessingInfo(): wave = %s file    = %p = %s \n",sageFilePtr->get_wave() ? "true" : "false",sageFilePtr,sageFilePtr->get_sourceFileNameWithPath().c_str());
     printf (" --- unparse output filename                    = %s \n",sageFilePtr->get_unparse_output_filename().c_str());
     printf (" --- sageFilePtr->getFileName()                 = %s \n",sageFilePtr->getFileName().c_str());
     printf (" --- sageFilePtr->get_globalScope()             = %p \n",sageFilePtr->get_globalScope());
     printf (" --- sageFilePtr->get_unparse_output_filename() = %s \n",sageFilePtr->get_unparse_output_filename().c_str());
     printf (" --- new_filename                               = %s \n",new_filename.c_str());
     printf ("################################################################ \n");
     printf ("################################################################ \n");
#endif

#if 0
  // Note that this only builds the include graph starting at the first header file not the input source file.
     string dotgraph_filename = "include_file_graph_from_before_attachPreprocessingInfo";
     ROSE_ASSERT(sageFilePtr != NULL);
  // generateGraphOfIncludeFiles(sageFilePtr,dotgraph_filename);
     ROSE_ASSERT(sageFilePtr->get_parent() != NULL);
     SgProject* project = SageInterface::getProject(sageFilePtr);
     ROSE_ASSERT(project != NULL);
     generateDOTforMultipleFile(*project);
#endif

  // DQ (11/18/2019): Check the flag that indicates that this SgSourceFile has NOT yet had its CPP directives and comments added.
     ROSE_ASSERT(sageFilePtr->get_processedToIncludeCppDirectivesAndComments() == false);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif

#if 0
     if (sageFilePtr->get_sourceFileNameWithPath() == "/home/quinlan1/ROSE/ROSE_GARDEN/codeSegregation/tests/sources/test_28.h")
        {
          printf ("Exiting as a test! \n");
          ROSE_ABORT();
        }
#endif

#if 0
  // DQ (10/23/2019): This name is allowed to be unset (and is unset by default at this point).
  // DQ (10/21/2019): Output a warning in this case.
     if (sageFilePtr->get_unparse_output_filename() == "")
        {
          printf ("WARNING: sageFilePtr->get_unparse_output_filename() is EMPTY \n");
        }
  // ROSE_ASSERT(sageFilePtr->get_unparse_output_filename() != "");
#endif

  // ROSEAttributesList* headerAttributes = getListOfAttributes(fileNameId);
  // bool use_Wave = false;
  // ROSEAttributesList* commentAndCppDirectiveList = buildCommentAndCppDirectiveList(use_Wave, Sg_File_Info::getFilenameFromID(currentFileNameId) );
  // ROSEAttributesList* commentAndCppDirectiveList = buildCommentAndCppDirectiveList(use_Wave, sageFilePtr->getFileName() );
     string filename = sageFilePtr->get_sourceFileNameWithPath();
     ROSEAttributesList* commentAndCppDirectiveList = NULL;

#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("Calling AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(): \n");
     printf ("sageFilePtr->getFileName() = %s \n",sageFilePtr->getFileName().c_str());
     printf ("filename                   = %s \n",filename.c_str());
     printf ("new_filename               = %s \n",new_filename.c_str());
  // printf ("tokenVector.size() = %zu using filename     = %s \n",getTokenStream(sageFilePtr).size(),filename.c_str());
#endif

  // DQ (1/4/2021): Adding support for comments and CPP directives and tokens to use new_filename.
  // DQ (7/4/2020): This function should not be called for binaries (only for C/C++ code).
  // commentAndCppDirectiveList = getPreprocessorDirectives(filename);
     bool usingWave = false;
  // commentAndCppDirectiveList = AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(usingWave,filename);
  // commentAndCppDirectiveList = AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(usingWave,sageFilePtr,filename);
     commentAndCppDirectiveList = AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(usingWave,sageFilePtr,filename,new_filename);

     ROSE_ASSERT(commentAndCppDirectiveList != NULL);

#if 0
     printf ("In attachPreprocessingInfo(): sageFilePtr->get_tokenSubsequenceMap().size() = %zu \n",sageFilePtr->get_tokenSubsequenceMap().size());
#endif

#if 0
  // DQ (1/4/2020): Testing use of new mechanism to get the token stream associated with the correct filename.
     if (new_filename != "")
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif

  // sageFilePtr->get_preprocessorDirectivesAndCommentsList().insert()
#if 0
     printf ("Adding list for filename = %s \n",filename.c_str());
#endif

#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("Test after buildCommentAndCppDirectiveList(): sageFilePtr->getFileName() = %s tokenVector.size() = %zu \n",sageFilePtr->getFileName().c_str(),getTokenStream(sageFilePtr).size());
     printf ("tokenVector.size() = %zu using filename     = %s \n",getTokenStream(sageFilePtr).size(),filename.c_str());
#endif

  // DQ (7/2/2020): Added assertion (fails for snippet tests).
     ROSE_ASSERT(sageFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);

     sageFilePtr->get_preprocessorDirectivesAndCommentsList()->addList(filename, commentAndCppDirectiveList);

  // DQ (6/30/2020): Testing for token-based unparsing.
     ROSE_ASSERT(sageFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
     ROSEAttributesListContainerPtr filePreprocInfo = sageFilePtr->get_preprocessorDirectivesAndCommentsList();

#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("filePreprocInfo->getList().size() = %zu \n",filePreprocInfo->getList().size());
#endif

  // We should at least have the current files CPP/Comment/Token information (even if it is an empty file).
     ROSE_ASSERT(filePreprocInfo->getList().size() > 0);

#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("sageFilePtr->get_token_list().size()                                       = %zu \n",sageFilePtr->get_token_list().size());
     printf ("commentAndCppDirectiveList->get_rawTokenStream()->size()                   = %zu \n",commentAndCppDirectiveList->get_rawTokenStream()->size());
     printf ("sageFilePtr->get_preprocessorDirectivesAndCommentsList()->getList().size() = %zu \n",sageFilePtr->get_preprocessorDirectivesAndCommentsList()->getList().size());
#endif
#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("sageFilePtr->getFileName() = %s \n",sageFilePtr->getFileName().c_str());
     printf ("tokenVector.size() = %zu using filename     = %s \n",getTokenStream(sageFilePtr).size(),filename.c_str());
     printf ("tokenVector.size() = %zu using new_filename = %s \n",getTokenStream(sageFilePtr).size(),new_filename.c_str());
#endif

#if 0
     printf ("In attachPreprocessingInfo(): sageFilePtr->get_tokenSubsequenceMap().size() = %zu \n",sageFilePtr->get_tokenSubsequenceMap().size());
#endif

#if 0
  // DQ (1/8/2021): Debugging the token stream used when reading the same file twice.
     if (new_filename != "")
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif

#if 0
  // DQ (5/4/2020): Test the collection of comments and CPP directives seperately from attachng them to the AST.
     printf ("Skipping attaching the list as a test \n");
     return;
#endif

#ifndef  CXX_IS_ROSE_CODE_GENERATION
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer_2 ("AST Comment and CPP Directive Processing (not using Wave):");

  // Dummy attribute (nothing is done here since this is an empty class)
     AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh;

  // DQ (4/19/2006): Now supporting either the collection or ALL comments and CPP directives 
  // into header file AST nodes or just the collection of the comments and CPP directives 
  // into the source file.
  // printf ("sageFilePtr->get_collectAllCommentsAndDirectives() = %s \n",sageFilePtr->get_collectAllCommentsAndDirectives() ? "true" : "false");

  // bool processAllFiles = sageFilePtr->get_collectAllCommentsAndDirectives();

#if 0
  // To support initial testing we will call one phase immediately after the other.  Late we will call the second phase, header 
  // file processing, from within the unparser when we know what header files are intended to be unparsed.
     bool header_file_unparsing_optimization             = false;
     bool header_file_unparsing_optimization_source_file = false;
     bool header_file_unparsing_optimization_header_file = false;

  // DQ (4/24/2021): Trying to debug the header file optimization support.
  // if (sageFilePtr->get_header_file_unparsing_optimization() == true)
        {
          header_file_unparsing_optimization = true;

       // DQ (4/24/2021): Trying to debug the header file optimization support.
       // if (sageFilePtr->get_header_file_unparsing_optimization_source_file() == true)
             {
               ROSE_ASSERT(sageFilePtr->get_header_file_unparsing_optimization_header_file() == false);
#if 0
               printf ("In attachPreprocessingInfo(): Optimize the collection of comments and CPP directives to seperate handling of the source file from the header files \n");
#endif
               header_file_unparsing_optimization_source_file = true;
             }
            else
             {
               ROSE_ASSERT(sageFilePtr->get_header_file_unparsing_optimization_source_file() == false);

            // DQ (4/24/2021): Trying to debug the header file optimization support.
            // if (sageFilePtr->get_header_file_unparsing_optimization_header_file() == true)
                  {
#if 0
                    printf ("Optimize the collection of comments and CPP directives to seperate handling of the header files from the source file \n");
#endif
                    header_file_unparsing_optimization_header_file = true;
                  }
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ABORT();
#endif
        }

#if DEBUG_ATTACH_PREPROCESSOR_INFO
     printf ("In attachPreprocessingInfo(): header_file_unparsing_optimization_header_file                    = %s \n",header_file_unparsing_optimization_header_file ? "true" : "false");
     printf ("In attachPreprocessingInfo(): sageFilePtr->get_header_file_unparsing_optimization()             = %s \n",sageFilePtr->get_header_file_unparsing_optimization() ? "true" : "false");
     printf ("In attachPreprocessingInfo(): sageFilePtr->get_header_file_unparsing_optimization_source_file() = %s \n",sageFilePtr->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
     printf ("In attachPreprocessingInfo(): sageFilePtr->get_header_file_unparsing_optimization_header_file() = %s \n",sageFilePtr->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif

#if 0
  // DQ (4/24/2021): This was diabled in 2020 as part of the header file unparsing support.
     if (header_file_unparsing_optimization_source_file == true)
        {
#if 0
          printf ("Setting processAllFiles = false \n");
#endif
          processAllFiles = false;
        }
#endif

#else
#if DEBUG_ATTACH_PREPROCESSOR_INFO
  // DQ (4/24/2021): Trying to debug the header file optimization support.
     printf ("In attachPreprocessingInfo(): Skipping header_file_unparsing_optimization preamble \n");
#endif
#endif

#if 0
     printf ("In attachPreprocessingInfo(): processAllFiles = %s \n",processAllFiles ? "true" : "false");
#endif

#if 0
  // DQ (9/30/2019): Need to trace down where the header files are provided a global scope to support the header file unparsing.
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif

  // DQ (6/2/2020): Change the API to pass in the CPP directives and comments list.
  // Also disable boolean processAllFiles since these are no longer processed in the 
  // traversal (adding CPP directives and comments from each file is a seperate).
  // AttachPreprocessingInfoTreeTrav tt(sageFilePtr,processAllFiles);
     AttachPreprocessingInfoTreeTrav tt(sageFilePtr,commentAndCppDirectiveList);

#if 0
     printf ("Exiting as a test after AttachPreprocessingInfoTreeTrav constructor call! \n");
     ROSE_ABORT();
#endif

  // When using Wave get all the preprocessing dirctives for all the files.
     if ( sageFilePtr->get_wave() == true )
        {
#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
       // DQ (5/4/2020): Disabled use of WAVE (at least for now).
          printf ("Disabled use of WAVE (at least for now) \n");
       // attachPreprocessingInfoUsingWave(sageFilePtr, tt.get_attributeMapForAllFiles() );
#else
          printf ("Boost wave is not available within this configuration \n");
          ROSE_ABORT();
#endif
        }

#if 0
  // Note that this only builds the include graph starting at the first header file not the input source file.
     string dotgraph_filename = "include_file_graph_from_before_attachPreprocessingInfo";
     ROSE_ASSERT(sageFilePtr != NULL);
     generateGraphOfIncludeFiles(sageFilePtr,dotgraph_filename);
#endif

  // DQ (12/19/2008): Added support for Fortran CPP files.
  // If this is a Fortran file requiring CPP processing then we want to call traverse, instead of 
  // traverseWithinFile, so that the whole AST will be processed (which is in a SgSourceFile 
  // using a name without the "_preprocessed" suffix, though the statements in the file are 
  // marked with a source position from the filename with the "_preprocessed" suffix).

  // DQ (4/24/2021): This is not used and generates a compiler warning.
  // bool requiresCPP = sageFilePtr->get_requires_C_preprocessor();

#if 0
     printf ("####################################################################### \n");
     printf ("####################################################################### \n");
     printf ("In attachPreprocessingInfo(): processAllFiles = %s requiresCPP = %s \n",processAllFiles ? "true" : "false",requiresCPP ? "true" : "false");
     printf ("####################################################################### \n");
     printf ("####################################################################### \n");
#endif

  // DQ (6/29/2020): This is now a simple traversal over the whole of the AST.
     tt.traverse(sageFilePtr, inh);

  // endif for ifndef  CXX_IS_ROSE_CODE_GENERATION
#endif

#if 0
     printf ("In attachPreprocessingInfo(): build include graph: wave = %s file = %p = %s \n",sageFilePtr->get_wave() ? "true" : "false",sageFilePtr,sageFilePtr->get_sourceFileNameWithPath().c_str());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif


#if 1
  // DQ (8/26/2020): This code must be placed here (after the comments and CPP directives have not been added to the AST).
     if (SgProject::get_verbose() > 1)
        {
          printf ("Calling fixupInitializersUsingIncludeFiles() \n");
        }

     SgProject* project = SageInterface::getProject(sageFilePtr);
     ROSE_ASSERT(project != NULL);

  // DQ (8/26/2020): Remove the redundent include files for initializers.
     fixupInitializersUsingIncludeFiles(project);
#endif

#if 0
     printf ("################################################### \n");
     printf ("################################################### \n");
     printf ("Leaving attachPreprocessingInfo(): filename = %s \n",sageFilePtr->get_sourceFileNameWithPath().c_str());
     printf ("################################################### \n");
     printf ("################################################### \n");
#endif

#if 0
  // DQ (10/27/2019): Testing the regression tests.
  // if (sageFilePtr->get_sourceFileNameWithPath() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
     if (sageFilePtr->get_sourceFileNameWithPath() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersUsingTokenStream_tests/test0/Simple.h")
        {
          printf ("Exiting as a test! \n");
          ROSE_ABORT();
        }
#endif

#if 0
  // Note that this only builds the include graph starting at the first header file not the input source file.
     string dotgraph_filename = "include_file_graph_from_attachPreprocessingInfo";
     ROSE_ASSERT(sageFilePtr != NULL);
     generateGraphOfIncludeFiles(sageFilePtr,dotgraph_filename);
#endif

  // DQ (11/18/2019): Set the flag that indicates that this SgSourceFile has had its CPP directives and comments added.
     sageFilePtr->set_processedToIncludeCppDirectivesAndComments(true);

  // DQ (1/7/2021): Get the token vector using the mechanism used in buildTokenStreamMapping().
  // vector<stream_element*> tokenVector = getTokenStream(sageFilePtr);

#if DEBUG_ATTACH_PREPROCESSOR_INFO
  // printf ("tokenVector.size() = %zu \n",tokenVector.size());
     printf ("tokenVector.size() = %zu \n",getTokenStream(sageFilePtr).size());
#endif

#if 0
     printf ("Exiting as a test after either call to sageFilePtr or traverseWithinFile \n");
     ROSE_ABORT();
#endif

#if 0
     // This is pointless since at this point the last step of the traversal has reset the lists (state held in tt).
     // DQ (10/27/2007): Output debugging information
     if ( SgProject::get_verbose() >= 3 )
        {
          if (processAllFiles == true)
               tt.display("Output from collecting ALL comments and CPP directives (across source and header files)");
            else
               tt.display("Output from collecting comments and CPP directives in source file only");
     }
#endif
   }


// EOF








