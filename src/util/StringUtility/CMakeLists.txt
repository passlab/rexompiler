add_library(util_StringUtility OBJECT
  Convert.C Escape.C FileNameClassifier.C FileUtility.C NumberToString.C Predicate.C Replace.C SplitJoin.C StringToNumber.C)

install(
  FILES Convert.h Escape.h FileUtility.h NumberToString.h IntegerOps.h Predicate.h Replace.h SplitJoin.h StringToNumber.h BitOps.h Constants.h StringUtility.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/util/StringUtility)