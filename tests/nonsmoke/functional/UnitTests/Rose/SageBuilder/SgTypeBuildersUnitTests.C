#include <rose/tests/unitTests/common.h>
using namespace Rose::Tests;

namespace Rose {
namespace Tests {
namespace UnitTests {
namespace SageBuilder {

/** Unit Tests for the SgType builder API functions.
 *
 *  Builders for simple and complex SgType nodes, such as integer type, function type, array type, struct type, etc.
 *  This library defines a collection of logic assertion macros (mostly) beginning with "ASSERT_". Many of the macros come in
 *
 *  See the SageBuilder namespace: http://rosecompiler.org/ROSE_HTML_Reference/namespaceSageBuilder.html
 **/
namespace SgTypeBuilders {

/** Common unit testing convenience functions for SgTypeBuilders.
 */
namespace Common {

/** Asserts common conditions for a newly built SgType.
 */
template<typename T>
static void HandlesDefaultParameter(T* type) {
  check(isNull(type) == false);
  check(is<T>(type));
}
}//::Rose::Tests::UnitTests::SageBuilder::SgTypeBuilders::Common

//-----------------------------------------------------------------------------
namespace BuildBoolTypeTests {
//-----------------------------------------------------------------------------
static void HandlesDefaultParameter() {
  SgTypeBool* t = ::SageBuilder::buildBoolType();
  SgTypeBuilders::Common::HandlesDefaultParameter(t);
}

static void RunUnitTests() {
  MLOG_TRACE_CXX("UnitTest") << "Starting BuildBoolTypeTests" << endl;
  //---------------------------------------------------------------------------
  // Tests

  HandlesDefaultParameter();

  //---------------------------------------------------------------------------
  MLOG_TRACE_CXX("UnitTest") << "Done" << endl;
}
}

//-----------------------------------------------------------------------------
namespace BuildNullptrTypeTests {
//-----------------------------------------------------------------------------
static void HandlesDefaultParameter() {
  SgTypeNullptr* t = ::SageBuilder::buildNullptrType();
  SgTypeBuilders::Common::HandlesDefaultParameter(t);
}

static void RunUnitTests() {
  MLOG_TRACE_CXX("UnitTest") << "Starting BuildBoolTypeTests" <<endl;
  //---------------------------------------------------------------------------
  // Tests

  HandlesDefaultParameter();

  //---------------------------------------------------------------------------
  MLOG_TRACE_CXX("UnitTest") << "Done" <<endl;
}
}

//-----------------------------------------------------------------------------
namespace BuildCharTypeTests {
//-----------------------------------------------------------------------------
static void HandlesDefaultParameter() {
  SgTypeChar* t = ::SageBuilder::buildCharType();
  SgTypeBuilders::Common::HandlesDefaultParameter(t);
}

static void RunUnitTests() {
  MLOG_TRACE_CXX("UnitTest") << "Starting BuildCharTypeTests" <<endl;
  //---------------------------------------------------------------------------
  // Tests

  HandlesDefaultParameter();

  //---------------------------------------------------------------------------
  MLOG_TRACE_CXX("UnitTest") << "Done" <<endl;
}
}

//-----------------------------------------------------------------------------
namespace BuildDoubleTypeTests {
//-----------------------------------------------------------------------------
static void HandlesDefaultParameter() {
  SgTypeDouble* t = ::SageBuilder::buildDoubleType();
  SgTypeBuilders::Common::HandlesDefaultParameter(t);
}

static void RunUnitTests() {
  MLOG_TRACE_CXX("UnitTest") << "Starting BuildDoubleTypeTests" <<endl;
  //---------------------------------------------------------------------------
  // Tests

  HandlesDefaultParameter();

  //---------------------------------------------------------------------------
  MLOG_TRACE_CXX("UnitTest") << "Done" <<endl;
}
}

//-----------------------------------------------------------------------------
static void RunUnitTests() {
//-----------------------------------------------------------------------------
  MLOG_TRACE_CXX("UnitTest") << "Starting SgTypeBuilders tests" <<endl;

  //---------------------------------------------------------------------------
  // Tests

  BuildBoolTypeTests::RunUnitTests();
  BuildNullptrTypeTests::RunUnitTests();
  BuildCharTypeTests::RunUnitTests();
  BuildDoubleTypeTests::RunUnitTests();

  //---------------------------------------------------------------------------

  MLOG_TRACE_CXX("UnitTest") << "Done" <<endl;
}//::Rose::Tests::UnitTests::SageBuilder::SgTypeBuilders::RunUnitTests
}//::Rose::Tests::UnitTests::SageBuilder::SgTypeBuilders
}//::Rose::Tests::UnitTests::SageBuilder
}//::Rose::Tests::UnitTests
}//::Rose::Tests
}//::Rose

//-----------------------------------------------------------------------------
int main() {
//-----------------------------------------------------------------------------
  UnitTests::Diagnostics::initialize();
  UnitTests::SageBuilder::SgTypeBuilders::RunUnitTests();

  return 0;
}//::main

