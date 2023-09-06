/**
 *  \file Outliner.cc
 *  \brief An outlining implementation.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>


#include "NameGenerator.hh"
#include "Outliner.hh"
#include "Preprocess.hh"
//#include "Transform.hh"
#include "commandline_processing.h"

namespace bfs=std::filesystem;
// =====================================================================

using namespace std;
using namespace Rose;

namespace Outliner {
  //! A set of flags to control the internal behavior of the outliner
  bool enable_classic=false;
  // use a wrapper for all variables or one parameter for a variable or a wrapper for all variables
  bool useParameterWrapper=false;  // use an array of pointers wrapper for parameters of the outlined function
  bool useStructureWrapper=false;  // use a structure wrapper for parameters of the outlined function
  bool preproc_only_=false;  // preprocessing only
  bool useNewFile=false; // generate the outlined function into a new source file
  bool copy_origFile=false; // when generating the new file to store outlined function, copy entire original file to it.
  bool temp_variable=false; // use temporary variables to reduce pointer dereferencing
  bool enable_liveness =false;
#if 1
  bool enable_debug=false; // 
#else
  // DQ (11/25/2020): Trying to turn on debugging of outliner.
  bool enable_debug=true; // 
#endif
  bool exclude_headers=false;
  bool use_dlopen=false; // Outlining the target to a separated file and calling it using a dlopen() scheme. It turns on useNewFile.
  bool use_dlopen_simple=false; // Same as use_dlopen, except that using a simple call convention to find and call the outlined function. 
  bool enable_template=false; // Outlining code blocks inside C++ templates
  bool select_omp_loop = false;  // Find OpenMP for loops and outline them. This is used for testing purposes.
  std::string output_path=""; // default output path is the original file's directory
  std::vector<int> lines;   // line positions of outlining targets, given by command line option -rose:outline:line for each

// DQ (3/19/2019): Suppress the output of the #include "autotuning_lib.h" since some tools will want to define there own supporting libraries and header files.
  bool suppress_autotuning_header = false; // when generating the new file to store outlined function, suppress output of #include "autotuning_lib.h".
};

// =====================================================================

//! Factory for unique outlined function names.
//prefix+id+suffix
static NameGenerator g_outlined_func_names ("OUT__", 0, "__");
static NameGenerator g_outlined_func_names2 ("OUT_", 0, "");
static NameGenerator g_outlined_arg_names ("__out_argv", 0, "__");

//! Hash a string into an unsigned long integer.
static
unsigned long
hashStringToULong (const string& s)
{
  unsigned long sum = 0;
  for (size_t i = 0; i < s.length (); ++i)
    sum += (unsigned long)s[i];
  return sum;
}

string
Outliner::generateFuncName (const SgStatement* stmt)
{
  // Generate a prefix.
  stringstream s;
  if (use_dlopen)  
    s<< g_outlined_func_names2.next();
  else
    s << g_outlined_func_names.next ();

  // Use the statement's raw filename to come up with a file-specific
  // tag.
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);
  if (use_dlopen)
  {
    const string file_name = info->get_raw_filename ();
    const string file_name2 = StringUtility::stripPathFromFileName(file_name);
    string base_name = StringUtility::stripFileSuffixFromFileName(file_name2);
    // base name may contain '-', replace it with '_' to get legal identifier
    for (size_t i=0; i<base_name.size(); i++) 
    {
      //cout<<"file base name:"<<base_name[i]<<endl;
      if (base_name[i]=='-')
        base_name[i]='_';
    }

    s<<"_"<<base_name<<"_"<<info->get_line();
  }
  else
    s << hashStringToULong (info->get_raw_filename ()) << "__";

  return s.str ();
}
string
Outliner::generateFuncArgName (const SgStatement* stmt)
{
  // Generate a prefix.
  stringstream s;
  s << g_outlined_arg_names.next ();

  // Use the statement's raw filename to come up with a file-specific
  // tag.
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);
  s << hashStringToULong (info->get_raw_filename ()) << "__";

  return s.str ();
}
// =====================================================================

Outliner::Result
Outliner::outline (SgStatement* s)
{
#ifdef __linux__
  if (enable_debug)  
    cout<<"Entering "<< __PRETTY_FUNCTION__ <<endl;
#endif
  string func_name = generateFuncName (s);
  return outline (s, func_name);
}

Outliner::Result
Outliner::outline (SgStatement* s, const std::string& func_name)
{
//cout<<"Debug Outliner::outline() input statement is:"<<s<<endl;  
  SgBasicBlock* s_post = preprocess (s);
//cout<<"Debug Outliner::outline() preprocessed statement is:"<<s_post<<endl;  
  ROSE_ASSERT (s_post);
#if 0
  //Liao, enable -rose:outline:preproc-only, 
  // then any translator can accept it even if their drivers do not handle it individually
  // Internal usage only for debugging
  static bool preproc_only_ = false; 
  SgFile * file= SageInterface::getEnclosingFileNode(s);
  SgStringList argvList = file->get_originalCommandLineArgumentList ();	


  if (CommandlineProcessing::isOption (argvList,"-rose:outline:",
                                     "preproc-only",true))
  {
    cout << "==> Running the outliner's preprocessing phase only." << endl;
    preproc_only_ = true;
  // Avoid passing the option to the backend compiler 
    file->set_originalCommandLineArgumentList(argvList);
  }  
#endif
  if (preproc_only_)
  {
    Outliner::Result fake;
    return fake;
  }  
  else
  {
 // return Transform::outlineBlock (s_post, func_name);

#if 0
    printf ("Calling outline block(): func_name = %s \n",func_name.c_str());
#endif

    Outliner::Result returnResult = outlineBlock (s_post, func_name);

#if 0
    printf ("DONE: Calling outline block(): func_name = %s \n",func_name.c_str());
#endif

#if 0
// This is now done in ASTtools::replaceStatement().
// DQ (2/24/2009): I think that at this point we should delete the subtree represented by "s_post"
// But it might have made more sense to not do a deep copy on "s_post" in the first place.
// Why is there a deep copy on "s_post"?
   SageInterface::deleteAST(s_post);
#else
   if (enable_debug)
     printf ("############ Skipped deletion of AST subtree at s_post = %p = %s \n",s_post,s_post->class_name().c_str());
#endif

   return returnResult;
  }  
}

//! Validate outliner settings. This should be called after outliner settings are adjusted (directly or by command-line
//  parsing) and before the outliner is used to outline source code.
void
Outliner::validateSettings() 
   {
#if 0
     printf ("In Outliner::validateSettings(): before building output_path directory: output_path = %s \n",output_path.c_str());
#endif

    if (!output_path.empty()) {
        // remove trailing '/'
        while (output_path[output_path.size()-1]=='/')
            output_path.erase(output_path.end()-1);
        // Create such path if not exists

#if 0
        printf ("After erasing directoriies in output_path: output_path = %s \n",output_path.c_str());
#endif
        bfs::path my_path(output_path);
        if (!bfs::exists(my_path))
          {
#if 0
            printf ("Before bfs::create_directory(): my_path.string() = %s \n",my_path.string().c_str());
#endif
            bfs::create_directory(my_path);
#if 0
            printf ("Exiting as a test! \n");
            ROSE_ASSERT(false);
#endif
          }

        if (!bfs::is_directory(my_path)) {
            cerr<<"output_path:"<<output_path<<" is not a path!"<<endl;
            ROSE_ASSERT(false);
        }
    }

    //------------ handle side effects of options-----------
    if (useStructureWrapper) {
        useParameterWrapper = true;
    }
    //    use_dlopen = false;
    if (use_dlopen|| use_dlopen_simple) {
        // turn on useNewFile as a side effect
        useNewFile= true;
        use_dlopen = true; // if use_dlopen_simple, we also turn on use_dlopen
        // also use parameter wrapper to simplify the call
        useParameterWrapper = true;
        temp_variable = true;
        exclude_headers = true;
        if (output_path.empty()) {
            output_path="/tmp";
        }
    }
}

//! Set internal options based on command line options
void Outliner::commandLineProcessing(std::vector<std::string> &argvList)
{
#if 0
    printf ("In Outliner::commandLineProcessing(): Processing the outliner output_path \n");
#endif

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","enable_debug",true))
  {
    cout<<"Enabling debugging mode for outlined functions..."<<endl;
    enable_debug= true;
  }
  //  else // may be set to true by external module directly
  //    enable_debug= false;

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","preproc-only",true))
  {
    if (enable_debug)
      cout<<"Enabling proprocessing only ..."<<endl;
    preproc_only_ = true;
  }
  //  else 
  //    preproc_only_ = false;

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","parameter_wrapper",true))
  {
    if (enable_debug)
      cout<<"Enabling parameter wrapping..."<<endl;
    useParameterWrapper= true;
  }
  //  else
  //    useParameterWrapper= false;
  //
  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","structure_wrapper",true))
  {
    if (enable_debug)
      cout<<"Enabling parameter wrapping using a structure..."<<endl;
    useStructureWrapper= true;
  }

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","new_file",true))
  {
    if (enable_debug)
      cout<<"Enabling new source file for outlined functions..."<<endl;
    useNewFile= true;
  }
  //  else
  //    useNewFile= false;
  
    if (CommandlineProcessing::isOption (argvList,"-rose:outline:","copy_orig_file",true))
  {
    if (enable_debug)
      cout<<"Enabling copying the original input file into the new source file for storing outlined functions..."<<endl;
    copy_origFile= true;
  }
  
  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","exclude_headers",true))
  {
    if (enable_debug)
      cout<<"Excluding headers in the new file containing outlined functions.."<<endl;
    exclude_headers= true;
  }
  //  else
  //    exclude_headers= false;

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","enable_classic",true))
  {
    if (enable_debug)
      cout<<"Enabling a classic way for outlined functions..."<<endl;
    enable_classic = true;
  }
  //  else
  //    enable_classic = false;
  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","enable_template",true))
  {
    if (enable_debug)
      cout<<"Enabling outlining code blocks inside C++ templates..."<<endl;
    enable_template= true;
  }
 
  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","temp_variable",true))
  {
    if (enable_debug)
      cout<<"Enabling using temp variables to reduce pointer dereferencing for outlined functions..."<<endl;
    temp_variable = true;
  }
  //  else
  //    temp_variable = false;
 
  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","use_dlopen",true))
  {
    if (enable_debug)
      cout<<"Using dlopen() to find an outilned function saved into a new source file ..."<<endl;
    use_dlopen = true;
  }

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","use_dlopen_simple",true))
  {
    if (enable_debug)
      cout<<"Using simple dlopen() call convention to find and call an outilned function saved into a new source file ..."<<endl;
    use_dlopen_simple = true;
  }
 
  //  else   // this option may be set by other module
  std::string lineStr;
  if (CommandlineProcessing::isOptionWithParameter (argvList,"-rose:outline:","line",lineStr, true))
  {
    if (enable_debug)
    {
      cout<<"Enabling using source code lines (line1,line2,line3,...) to specify targets for outlining."<<endl;
      cout<<"Found position at line:"<<lineStr<<endl;
    }

    //parse a list of line numbers in the format of line1,line2,... to actual integer line numbers
    stringstream lineStrStream(lineStr);
    std::string ln;
    while(getline(lineStrStream, ln, ',')){ //use comma as delim for cutting string
      lines.push_back(stoi(ln));
    }
  }

  if (CommandlineProcessing::isOptionWithParameter (argvList,"-rose:outline:","output_path",output_path, true))
  {
    if (enable_debug)
      cout<<"Using a custom output path:"<<output_path<<endl;
  }
//  else  //reset to NULL if useNewFile is not true
//    output_path="";

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","select_omp_loop",true))
  {
    if (enable_debug)
      cout<<"Select OpenMP loops for outlining  ..."<<endl;
    select_omp_loop = true;
    // turn on OpenMP parsing and AST creation
    argvList.push_back("-rose:openmp:ast_only");
  }
 
 if (use_dlopen || temp_variable || use_dlopen_simple)    
  {
    if (CommandlineProcessing::isOption (argvList,"-rose:outline:","enable_liveness",true))
      enable_liveness = true;
    //     else
    //        enable_liveness = false;
  }

  // keep --help option after processing, let other modules respond also
  if ((CommandlineProcessing::isOption (argvList,"--help","",false)) ||
      (CommandlineProcessing::isOption (argvList,"-help","",false)))
  {
    cout<<"Outliner-specific options"<<endl;
    cout<<"Usage: outline [OPTION]... FILENAME..."<<endl;
    cout<<"Main operation mode:"<<endl;
    cout<<"\t-rose:outline:preproc-only                     preprocessing only, no actual outlining"<<endl;
    cout<<"\t-rose:outline:line line_numbers                using source code lines (\"line1,line2,line3,...\") to specify targets for outlining"<<endl;
    cout<<"\t-rose:outline:parameter_wrapper                use an array of pointers to pack the variables to be passed"<<endl;
    cout<<"\t-rose:outline:structure_wrapper                use a data structure to pack the variables to be passed"<<endl;
    cout<<"\t-rose:outline:enable_classic                   use parameters directly in the outlined function body without transferring statement, C only"<<endl;
    cout<<"\t-rose:outline:temp_variable                    use temp variables to reduce pointer dereferencing for the variables to be passed"<<endl;
    cout<<"\t-rose:outline:enable_liveness                  use liveness analysis to reduce restoring statements if temp_variable is turned on"<<endl;
    cout<<"\t-rose:outline:new_file                         use a new source file for the generated outlined function"<<endl;
    cout<<"\t-rose:outline:output_path                      the path to store newly generated files for outlined functions, if requested by new_file. The original source file's path is used by default."<<endl;
    cout<<"\t-rose:outline:exclude_headers                  do not include any headers in the new file for outlined functions"<<endl;
    cout<<"\t-rose:outline:use_dlopen                       use dlopen() to find the outlined functions saved in new files.It will turn on new_file and parameter_wrapper flags internally"<<endl;
    cout<<"\t-rose:outline:use_dlopen_simple                use simple dlopen() call convention to find and call the outlined functions saved in new files.It will turn on new_file and parameter_wrapper flags internally"<<endl;
    cout<<"\t-rose:outline:copy_orig_file                   used with dlopen(): single lib source file copied from the entire original input file. All generated outlined functions are appended to the lib source file"<<endl;
    cout<<"\t-rose:outline:enable_template                  support outlining code blocks inside C++ templates (experimental)"<<endl;
    cout<<"\t-rose:outline:enable_debug                     run outliner in a debugging mode"<<endl;
    cout<<"\t-rose:outline:select_omp_loop                  select OpenMP for loops for outlining, used for testing purpose"<<endl;
    cout <<"---------------------------------------------------------------"<<endl;
  }

  validateSettings();
}


SgBasicBlock *
Outliner::preprocess (SgStatement* s)
{
#ifdef __linux__
  if (enable_debug)  
    cout<<"Entering "<< __PRETTY_FUNCTION__ <<endl;
#endif
  // bool b = isOutlineable (s, enable_debug);
  bool b = isOutlineable (s, SgProject::get_verbose () >= 1);
  if (b!= true)
  {
    cerr<<"Outliner::preprocess() Input statement:"<<s->unparseToString()<<"\n is not outlineable!"<<endl;
    ROSE_ASSERT(b);
  //  ROSE_ASSERT (isOutlineable (s, SgProject::get_verbose () >= 1));
  }
  SgBasicBlock* s_post = Preprocess::preprocessOutlineTarget (s);
  ROSE_ASSERT (s_post); 
  return s_post;
}

/* =====================================================================
 *  Container to store the results of one outlining transformation.
 */

// DQ (8/7/2019): Store data required to support defering the transformation to insert the outlined function prototypes.
Outliner::Result::Result (void)
  : decl_ (0), call_ (0),target_class_member(NULL),new_function_prototype(NULL) 
{
}

#if 0
// DQ (8/7/2019): Store data required to support defering the transformation to insert the outlined function prototypes.
Outliner::Result::Result (SgFunctionDeclaration* decl,
                             SgStatement* call, SgFile* file/*=NULL*/)
  : decl_ (decl), call_ (call), file_(file),target_class_member(NULL),new_function_prototype(NULL)
{
}
#else
#if 0
  // DQ (11/19/2020): Original code before moving the DeferredTransformation support to SageInterface.
  // DQ (8/15/2019): Adding support to defere the transformations in header files (a performance improvement).
Outliner::Result::Result (SgFunctionDeclaration* decl,
                          SgStatement* call, SgFile* file/*=NULL*/, DeferredTransformation input_deferredTransformation)
  : decl_ (decl), call_ (call), file_(file),target_class_member(NULL),new_function_prototype(NULL),deferredTransformation(input_deferredTransformation)
{
}
#else
  // DQ (11/19/2020): New code after moving the DeferredTransformation support to SageInterface.
Outliner::Result::Result (SgFunctionDeclaration* decl,
                          SgStatement* call, SgFile* file/*=NULL*/,
                          SageInterface::DeferredTransformation input_deferredTransformation)
   : decl_ (decl), call_ (call), file_(file),target_class_member(NULL),new_function_prototype(NULL),deferredTransformation(input_deferredTransformation)
{
}
#endif
#endif

#if 0
// DQ (8/7/2019): Store data required to support defering the transformation to insert the outlined function prototypes.
Outliner::Result::Result (const Result& b)
  : decl_ (b.decl_), call_ (b.call_),target_class_member(b.target_class_member),new_function_prototype(b.target_class_member)
{
}
#else
// DQ (8/15/2019): Adding support to defere the transformations in header files (a performance improvement).
// DQ (8/7/2019): Store data required to support defering the transformation to insert the outlined function prototypes.
Outliner::Result::Result (const Result& b)
  : decl_ (b.decl_), call_ (b.call_),target_class_member(b.target_class_member),new_function_prototype(b.target_class_member),deferredTransformation(b.deferredTransformation)
{
}
#endif


bool
Outliner::Result::isValid (void) const
{
  return decl_ && call_;
}


/* =====================================================================
 *  Container to store the support for defering the transformations to later (on header files that we will want to unparse).
 */

#if 0
// DQ (11/19/2020): We need to expand the use of this to cover deffered transformations of common SageInterface transformations (e.g. replaceStatement).
// So I need to move this out of being specific to the outliner and make it more generally data structure in the SageInterface.

Outliner::DeferredTransformation::DeferredTransformation()
   : class_definition(NULL),
     target_class_member(NULL),
     new_function_prototype(NULL)
   {
   }

Outliner::DeferredTransformation::DeferredTransformation(
   SgClassDefinition* input_class_definition, 
   SgDeclarationStatement* input_target_class_member, 
   SgDeclarationStatement* input_new_function_prototype)
   : class_definition(input_class_definition),
     target_class_member(input_target_class_member),
     new_function_prototype(input_new_function_prototype)
   {
   }

Outliner::DeferredTransformation::DeferredTransformation (const DeferredTransformation& X)
   : class_definition(X.class_definition),
     target_class_member(X.target_class_member),
     new_function_prototype(X.new_function_prototype),
     targetClasses(X.targetClasses),
     targetFriends(X.targetFriends)
   {
   }

Outliner::DeferredTransformation & Outliner::DeferredTransformation::operator= (const DeferredTransformation& X)
   {
#if 0
     printf ("Inside of Outliner::DeferredTransformation::operator= (const DeferredTransformation& X) \n");
#endif

     targetFriends = X.targetFriends;
     targetClasses = X.targetClasses;
     return *this;
   }


Outliner::DeferredTransformation::~DeferredTransformation (void) {}; //! Shallow; does not delete fields.
#endif

// eof
