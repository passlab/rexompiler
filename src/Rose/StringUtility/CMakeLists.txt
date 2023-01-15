add_library(roseStringUtility OBJECT
  Convert.C Escape.C NumberToString.C Predicate.C SplitJoin.C StringToNumber.C)

install(
  FILES Convert.h Escape.h NumberToString.h Predicate.h SplitJoin.h StringToNumber.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/StringUtility)

add_dependencies(roseStringUtility rosetta_generated)
