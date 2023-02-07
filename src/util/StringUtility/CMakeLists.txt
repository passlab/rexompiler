add_library(roseStringUtility OBJECT
  Convert.C Escape.C FileNameClassifier.C FileUtility.C NumberToString.C Predicate.C SplitJoin.C StringToNumber.C)

install(
  FILES Convert.h Escape.h FileUtility.h NumberToString.h Predicate.h SplitJoin.h StringToNumber.h BItOps.h Constants.h StringUtility.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/util/StringUtility)

add_dependencies(roseStringUtility rosetta_generated)
