#include "mlog.h"

int main() {
   int i;

   for (i=0; i<MLOG_LEVEL_UNDEFINED; i++) {
	   mlogLevel = i;
	   std::cout << "Current log level: " << std::string(mlogLevelToString_C[mlogLevel]) << std::endl;
	   std::cout << "=======================================================================" << std::endl;

	   const char * subject_c = "C-API";
	   const std::string subject_cxx = "C++-API";

	   MLOG_FATAL_C(subject_c, "This is a FATAL event: %d = %s\n", 1, "one");
	   MLOG_FATAL_MORE_C("Additional lines for FATAL event information %s is %d\n", "two", 2);
	   MLOG_FATAL_MORE_C("More lines for FATAL event information %s is %d\n", "three", 3);

	   MLOG_ERROR_C(subject_c, "This is an ERROR event: %d = %s\n", 1, "one");
	   MLOG_ERROR_MORE_C("Additional lines for ERROR event information %s is %d\n", "two", 2);
	   MLOG_ERROR_MORE_C("More lines for ERROR event information %s is %d\n", "three", 3);

	   MLOG_WARN_C(subject_c, "This is an WARN event: %d = %s\n", 1, "one");
	   MLOG_WARN_MORE_C("Additional lines for WARN event information %s is %d\n", "two", 2);
	   MLOG_WARN_MORE_C("More lines for WARN event information %s is %d\n", "three", 3);

	   MLOG_KEY_C(subject_c, "This is a KEY event: %d = %s\n", 1, "one");
	   MLOG_KEY_MORE_C("Additional lines for KEY event information %s is %d\n", "two", 2);
	   MLOG_KEY_MORE_C("More lines for KEY event information %s is %d\n", "three", 3);

	   MLOG_INFO_C(subject_c, "This is an INFO event: %d = %s\n", 1, "one");
	   MLOG_INFO_MORE_C("Additional lines for INFO event information %s is %d\n", "two", 2);
	   MLOG_INFO_MORE_C("More lines for INFO event information %s is %d\n", "three", 3);

	   MLOG_MARCH_C(subject_c, "This is a MARCH event: %d = %s\n", 1, "one");
	   MLOG_MARCH_MORE_C("Additional lines for MARCH event information %s is %d\n", "two", 2);
	   MLOG_MARCH_MORE_C("More lines for MARCH event information %s is %d\n", "three", 3);

	   MLOG_TRACE_C(subject_c, "This is a TRACE event: %d = %s\n", 1, "one");
	   MLOG_TRACE_MORE_C("Additional lines for TRACE event information %s is %d\n", "two", 2);
	   MLOG_TRACE_MORE_C("More lines for TRACE event information %s is %d\n", "three", 3);

	   MLOG_DEBUG_C(subject_c, "This is a DEBUG event: %d = %s\n", 1, "one");
	   MLOG_DEBUG_MORE_C("Additional lines for DEBUG event information %s is %d\n", "two", 2);
	   MLOG_DEBUG_MORE_C("More lines for DEBUG event information %s is %d\n", "three", 3);
	   std::cout << "---------------------------------------------------------------------------" << std::endl;

	   MLOG_FATAL_CXX(subject_cxx) << "This is a FATAL event: " << 1 << " = " << "one" << std::endl;
	   MLOG_FATAL_MORE_CXX() << "Additional lines for FATAL event information " << 2 << " = " << "two" << std::endl;
	   MLOG_FATAL_MORE_CXX() << "More lines for FATAL event information " << 3 << " = " << "three" << std::endl;

	   MLOG_ERROR_CXX(subject_cxx) << "This is a ERROR event: " << 1 << " = " << "one" << std::endl;
	   MLOG_ERROR_MORE_CXX() << "Additional lines for ERROR event information " << 2 << " = " << "two" << std::endl;
	   MLOG_ERROR_MORE_CXX() << "More lines for ERROR event information " << 3 << " = " << "three" << std::endl;
	   
	   MLOG_WARN_CXX(subject_cxx) << "This is a WARN event: " << 1 << " = " << "one" << std::endl;
	   MLOG_WARN_MORE_CXX() << "Additional lines for WARN event information " << 2 << " = " << "two" << std::endl;
	   MLOG_WARN_MORE_CXX() << "More lines for WARN event information " << 3 << " = " << "three" << std::endl;
	   
	   MLOG_KEY_CXX(subject_cxx) << "This is a KEY event: " << 1 << " = " << "one" << std::endl;
	   MLOG_KEY_MORE_CXX() << "Additional lines for KEY event information " << 2 << " = " << "two" << std::endl;
	   MLOG_KEY_MORE_CXX() << "More lines for KEY event information " << 3 << " = " << "three" << std::endl;
	  
	   MLOG_INFO_CXX(subject_cxx) << "This is a INFO event: " << 1 << " = " << "one" << std::endl;
	   MLOG_INFO_MORE_CXX() << "Additional lines for INFO event information " << 2 << " = " << "two" << std::endl;
	   MLOG_INFO_MORE_CXX() << "More lines for INFO event information " << 3 << " = " << "three" << std::endl;
	   
	   MLOG_MARCH_CXX(subject_cxx) << "This is a MARCH event: " << 1 << " = " << "one" << std::endl;
	   MLOG_MARCH_MORE_CXX() << "Additional lines for MARCH event information " << 2 << " = " << "two" << std::endl;
	   MLOG_MARCH_MORE_CXX() << "More lines for MARCH event information " << 3 << " = " << "three" << std::endl;

	   MLOG_TRACE_CXX(subject_cxx) << "This is a TRACE event: " << 1 << " = " << "one" << std::endl;
	   MLOG_TRACE_MORE_CXX() << "Additional lines for TRACE event information " << 2 << " = " << "two" << std::endl;
	   MLOG_TRACE_MORE_CXX() << "More lines for TRACE event information " << 3 << " = " << "three" << std::endl;
	   
	   MLOG_DEBUG_CXX(subject_cxx) << "This is a DEBUG event: " << 1 << " = " << "one" << std::endl;
	   MLOG_DEBUG_MORE_CXX() << "Additional lines for DEBUG event information " << 2 << " = " << "two" << std::endl;
	   MLOG_DEBUG_MORE_CXX() << "More lines for DEBUG event information " << 3 << " = " << "three" << std::endl;
	   std::cout << "---------------------------------------------------------------------------" << std::endl;

	   //Testing lower-level API for which level can be directly provided as argument
	   const char * low_subject_c = "LOW-LEVEL C-API";
	   const std::string low_subject_cxx = "LOW-LEVEL C++-API";
	   MLOG_LEVEL_C(MLOG_LEVEL_FATAL, low_subject_c, "hello world, FATAL\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_ERROR, low_subject_c, "hello world, ERROR\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_WARN, low_subject_c, "hello world, WARN\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_KEY, low_subject_c, "hello world, KEY\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_INFO, low_subject_c, "hello world, INFO\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_MARCH, low_subject_c, "hello world, MARCH\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_TRACE, low_subject_c, "hello world, TRACE\n");
	   MLOG_LEVEL_C(MLOG_LEVEL_DEBUG, low_subject_c, "hello world, DEBUG\n");
	   std::cout << "---------------------------------------------------------------------------" << std::endl;

	   MLOG_LEVEL_CXX(MLOG_LEVEL_FATAL, low_subject_cxx) << "hello world, FATAL" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_ERROR, low_subject_cxx) << "hello world, ERROR" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_WARN, low_subject_cxx) << "hello world, WARN" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_KEY, low_subject_cxx) << "hello world, KEY" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_INFO, low_subject_cxx) << "hello world, INFO" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_MARCH, low_subject_cxx) << "hello world, MARCH" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_TRACE, low_subject_cxx) << "hello world, TRACE" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_DEBUG, low_subject_cxx) << "hello world, DEBUG" <<std::endl;
	   MLOG_LEVEL_CXX(MLOG_LEVEL_DEBUG, MLOG_INIT) << "hello world, DEBUG" <<std::endl;
	   std::cout << "=======================================================================" << std::endl;

   }
   //check and abort, only supported by C-style API
   //MLOG_CHECK_ERROR_ABORT_C(1==1, MLOG_EDG2SAGE, "error %d = %s and abort\n", 1, "one");

   return 0;
}
