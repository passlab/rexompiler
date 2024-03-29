// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, const SgName& n) {
   return os << "\"" << n.str() << "\"";
}
std::ostream& operator<<(std::ostream& os, const SgAsmStmt::AsmRegisterNameList & bv) 
   {
      for (unsigned int i=0; i < bv.size(); i++) {
        if (i != 0) os << ", ";
        os << ((long)(bv[i]));
      }
     return os;
   }
std::ostream& operator<<(std::ostream& os, const SgDataStatementObjectPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, const SgDataStatementValuePtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, const SgCommonBlockObjectPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, const SgDimensionObjectPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, const SgLabelSymbolPtrList& bv) 
   {
      return os;
   }
std::ostream& operator<<(std::ostream& os, const SgFormatItemPtrList& bv) 
   {
      return os;
   }

void doRTI(const char* fieldNameBase, void* fieldPtr, size_t fieldSize, void* thisPtr, const char* className, const char* typeString, const char* fieldName, const std::string& fieldContents, RTIMemberData& memberData) {
#if ROSE_USE_VALGRIND
  doUninitializedFieldCheck(fieldNameBase, fieldPtr, fieldSize, thisPtr, className);
#endif
  memberData = RTIMemberData(typeString, fieldName, fieldContents);
}

