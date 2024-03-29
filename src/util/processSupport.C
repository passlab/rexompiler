
// DQ (9/28/2017): Removed this to avoid dependence on ROSETTA generated code.
// DQ (9/8/2017): Added header file suppport to allow for SgProject to be referenced for debugging.
#include "rosePublicConfig.h"
// #include "rose_config.h"
// #include "sage3basic.h"

#include "rose_msvc.h"

#include "processSupport.h"

#if !ROSE_MICROSOFT_OS
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#endif

#include <cstdlib>
#include <cstring>
#include "rosedll.h"

using namespace std;

int systemFromVector(const vector<string>& argv)
   {
     assert (!argv.empty());

#if !ROSE_MICROSOFT_OS
     pid_t pid = fork();

     if (pid == -1) {perror("fork"); abort();}

     if (pid == 0)
        { // Child
          vector<const char*> argvC(argv.size() + 1);
          size_t length = argv.size();

       // DQ (6/22/2020): Truncate as a test!
       // length = 5;

       // for (size_t i = 0; i < argv.size(); ++i)
          for (size_t i = 0; i < length; ++i)
             {
               argvC[i] = strdup(argv[i].c_str());
#if 0
               printf ("In systemFromVector(): loop: argvC[%zu] = %s \n",i,argvC[i]);
#endif
             }
          argvC.back() = NULL;
          execvp(argv[0].c_str(), (char* const*)&argvC[0]);

#if 0
       // DQ (9/12/2017): This is one approach to debugging this (which was required for Ada because the Ada compiler is a call to gnat with the extra option "compile".
       // execvp(argv[0].c_str(), (char* const*)&argvC[0]);
       // execvp("/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin/bin/gnat compile",(char* const*)&argvC[0]);
          execvp("/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin/bin/gnat",(char* const*)&argvC[0]);
#endif

          perror(("execvp in systemFromVector: " + argv[0]).c_str());
          exit(1); // Should not get here normally
        }
       else
        { // Parent
          int status;
          pid_t err = waitpid(pid, &status, 0);
#if 0
          printf ("In systemFromVector(): status = %d \n",status);
#endif
          if (err == -1) {perror("waitpid"); abort();}

          return status;
        }
#else
     std::string commandLine = argv[0];
     for (size_t i = 1; i < argv.size(); ++i)
        {
          commandLine += " " + argv[i];
        }

     STARTUPINFO si;
     PROCESS_INFORMATION pi;
     ZeroMemory(&si,sizeof(si));
     si.cb=sizeof(si);
     ZeroMemory(&pi,sizeof(pi));

     if(!CreateProcess(NULL, (char*)commandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
          printf ("Error running MSVS compiler.\n");
          return 1;
        }

     WaitForSingleObject(pi.hProcess,INFINITE);
     unsigned long exitCode;
     GetExitCodeProcess(pi.hProcess, &exitCode);
     return exitCode;
#endif
   }

// EOF is not handled correctly here -- EOF is normally set when the child
// process exits
FILE* popenReadFromVector(const vector<string>& argv) {
  assert (!argv.empty());
  int pipeDescriptors[2];

#if !ROSE_MICROSOFT_OS
  int pipeErr = pipe(pipeDescriptors);
  if (pipeErr == -1) {perror("pipe"); abort();}
  pid_t pid = fork();
  if (pid == -1) {perror("fork"); abort();}
  if (pid == 0) { // Child
    vector<const char*> argvC(argv.size() + 1);
    for (size_t i = 0; i < argv.size(); ++i) {
      argvC[i] = strdup(argv[i].c_str());
    }
    argvC.back() = NULL;
    int closeErr = close(pipeDescriptors[0]);
    if (closeErr == -1) {perror("close (in child)"); abort();}
    int dup2Err = dup2(pipeDescriptors[1], 1); // stdout
    if (dup2Err == -1) {perror("dup2"); abort();}
    execvp(argv[0].c_str(), (char* const*)&argvC[0]);
    perror(("execvp in popenReadFromVector: " + argv[0]).c_str());
    exit(1); // Should not get here normally
  } else { // Parent
    int closeErr = close(pipeDescriptors[1]);
    if (closeErr == -1) {perror("close (in parent)"); abort();}
    return fdopen(pipeDescriptors[0], "r");
  }
#else
        // tps: does not work right now. Have not hit this assert yet.
  printf ("Error: no MSVS implementation available popenReadFromVector() (not implemented) \n");
  ROSE_ABORT();
#endif
}

int pcloseFromVector(FILE* f)
   {
  // Assumes there is only one child process

  // DQ (11/28/2009): MSVC warns of status not being used, so initialize it.
     int status = 0;

#if !ROSE_MICROSOFT_OS
  /* pid_t err = */ wait(&status);
#else
     printf ("Error: no MSVS implementation available pcloseFromVector()(not implemented) \n");
     ROSE_ABORT();
#endif

     fclose(f);
     return status;
   }
