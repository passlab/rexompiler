#ifndef __MLOG_H__
#define __MLOG_H__
#include <stddef.h>
#include <stdlib.h>
#include<string>
#include<iostream>

/** 
 * Mandatory logging are those critial msgs of the runtime including
 * three levels of severity: 
 * MLOG_LEVEL_WARN: warning of likely problem, but the results might be ok
 * MLOG_LEVEL_ERROR: error of runtime execution, program can continue,
 *                  results will be wrong, but still useful
 * MLOG_LEVEL_FATAL: fatal errors of runtime, program cannot continue.
 *
 * Optional logging are those msgs that provide information of program execution. 
 * It is controlled by the mlogLevel varible that control which level of information
 * will be spit out. The levels of details incrementally include:
 * MLOG_LEVEL_KEY
 * MLOG_LEVEL_INFO
 * MLOG_LEVEL_MARCH
 * MLOG_LEVEL_TRACE
 * The default mlogLevel is MLOG_LEVEL_NONE, i.e. no information of these four levels
 * will spit out. 
 *
 * MLOG_LEVEL_DEBUG is a mlogging level that can be controlled by users, it should be 
 * used for debugging purpose only. 
 *
 * There are two sets of APIs to use, C-style (printf) and C++-style (<<), and these APIs are
 * are simply macros that are used as functions. API names carry the level information, 
 * and each mlogging API call should also be proivde a subject to indicate the compoenents or module
 * of this mlogging. MLogging msgs include logging level, subject, file:line, function name, 
 * and user-specified msgs. User msgs are either C-style printf format or C++-style << format 
 * depending on which style to use. C-style APIs and C++-style APIs have _C and _CXX suffix respectively.  
 */

//logging levels
typedef enum MLOG_LEVEL{
	MLOG_LEVEL_FATAL = 0,
	MLOG_LEVEL_ERROR,
	MLOG_LEVEL_WARN,
	MLOG_LEVEL_NONE,
	MLOG_LEVEL_KEY,
	MLOG_LEVEL_INFO,
	MLOG_LEVEL_MARCH,
	MLOG_LEVEL_TRACE,
	MLOG_LEVEL_DEBUG,
	MLOG_LEVEL_UNDEFINED,
} MLOG_LEVEL_t;

//logging subjects, which are one word names
#define MLOG_INIT		"INIT"
#define MLOG_MISC		"MISC"
#define MLOG_DRIVER		"DRIVER"
#define MLOG_EDG			"EDG"
#define MLOG_EDG2SAGE		"EDG2SAGE"
#define MLOG_OFP			"OFP"
#define MLOG_OFP2SAGE		"OFP2SAGE"redirect stderr to cerr c++
#define MLOG_OMPPARSER		"OMPPARSER"
#define MLOG_OMP2SAGE		"OMP2SAGE"
#define MLOG_ROSETTA		"ROSETTA"
#define MLOG_FRONTEND		"FRONTEND"
#define MLOG_MIDEND		"MIDEND"
#define MLOG_BACKEND		"BACKEND"
#define MLOG_UNPARSER		"UNPARSER"
#define MLOG_SAGEINTERFACE	"sageInterface"

/* set the current log level */
#define DEFAULT_MLOG_LEVEL MLOG_LEVEL_NONE
extern int  mlogLevel; //The current mlog level setting to control which level (KEY/INFO/MARCH/TRACE) info is spit out
extern void mlog_C(MLOG_LEVEL_t level, const char * subject, const char * file, int line, const char * funcname, const char * format, ...);
extern void mlogMore_C(const char * format, ...);
extern const char * mlogLevelToString_C[];
extern std::string mlogLevelToString_CXX(MLOG_LEVEL_t level);

#define WHEREARG __FILE__,__LINE__, __func__

//C-style API for mandatory fatal/error/warning/debugging mlogging
#define MLOG_FATAL_C(SUBJECT, ...) mlog_C(MLOG_LEVEL_FATAL, SUBJECT, WHEREARG, __VA_ARGS__)
#define MLOG_ERROR_C(SUBJECT, ...) mlog_C(MLOG_LEVEL_ERROR, SUBJECT, WHEREARG, __VA_ARGS__)
#define MLOG_WARN_C(SUBJECT, ...)  mlog_C(MLOG_LEVEL_WARN,  SUBJECT, WHEREARG, __VA_ARGS__)
#define MLOG_DEBUG_C(SUBJECT,...)  mlog_C(MLOG_LEVEL_DEBUG, SUBJECT, WHEREARG, __VA_ARGS__)
#define MLOG_FATAL_MORE_C(...) mlogMore_C(__VA_ARGS__)
#define MLOG_ERROR_MORE_C(...) mlogMore_C(__VA_ARGS__)
#define MLOG_WARN_MORE_C(...)  mlogMore_C(__VA_ARGS__)
#define MLOG_DEBUG_MORE_C(...)  mlogMore_C(__VA_ARGS__)

#define MLOG_LEVEL_C(level, subject, ...) \
	if (level<=mlogLevel) mlog_C(level, subject, WHEREARG, __VA_ARGS__)
#define MLOG_LEVEL_MORE_C(level, ...) \
	if (level<=mlogLevel) mlogMore_C(__VA_ARGS__)

//C-style API, i.e. printf style for optional key/info/march/trace mlogging controlled by mlogLevel
#define MLOG_KEY_C(SUBJECT,...) 	MLOG_LEVEL_C(MLOG_LEVEL_KEY,   SUBJECT, __VA_ARGS__)
#define MLOG_INFO_C(SUBJECT,...) 	MLOG_LEVEL_C(MLOG_LEVEL_INFO,  SUBJECT, __VA_ARGS__)
#define MLOG_MARCH_C(SUBJECT,...)	MLOG_LEVEL_C(MLOG_LEVEL_MARCH, SUBJECT, __VA_ARGS__)
#define MLOG_TRACE_C(SUBJECT,...)	MLOG_LEVEL_C(MLOG_LEVEL_TRACE, SUBJECT, __VA_ARGS__)
#define MLOG_KEY_MORE_C(...) 		MLOG_LEVEL_MORE_C(MLOG_LEVEL_KEY, __VA_ARGS__)
#define MLOG_INFO_MORE_C(...) 		MLOG_LEVEL_MORE_C(MLOG_LEVEL_INFO, __VA_ARGS__)
#define MLOG_MARCH_MORE_C(...)		MLOG_LEVEL_MORE_C(MLOG_LEVEL_MARCH, __VA_ARGS__)
#define MLOG_TRACE_MORE_C(...)		MLOG_LEVEL_MORE_C(MLOG_LEVEL_TRACE, __VA_ARGS__)

//C-style API to use for fatal/error/warning that is conditioned by the provided expression
#define MLOG_CHECK_WARN_RETURN_C(cond, subject, ... ) \
	if (cond) {mlog_C(MLOG_LEVEL_WARN, subject, WHEREARG, __VA_ARGS__); return; }
#define MLOG_CHECK_ERROR_ABORT_C(cond, subject, ... ) \
	if (cond) {mlog_C(MLOG_LEVEL_ERROR, subject, WHEREARG, __VA_ARGS__); abort(); }
#define MLOG_CHECK_FATAL_ABORT_C(cond, subject, ... ) \
	if(cond) {mlog_C(MLOG_LEVEL_FATAL, subject, WHEREARG, __VA_ARGS__); abort(); }

/**************** for C++-style API, i.e. using cerr <<< ***************************************/
#define MLOG_LEVEL_HEADER_CXX(level, subject) std::cerr << mlogLevelToString_CXX(level) << ": " \
		<< subject << " [" << __FILE__ << ":" << __LINE__<< "," << __func__ << "]\033[0m "
#define MLOG_LEVEL_CXX(level, subject) if (level <= mlogLevel) MLOG_LEVEL_HEADER_CXX(level, subject)

//*__MORE_CXX API is used to add more lines for the mlog msg that is already started by MLOG_*_CXX
#define MLOG_MORE_CXX() std::cerr << "\t"
#define MLOG_LEVEL_MORE_CXX(level) if (level <= mlogLevel) std::cerr << "\t"

//C++-style API for mandatory fatal/error/warning/debugging mlogging
#define MLOG_FATAL_CXX(SUBJECT) MLOG_LEVEL_HEADER_CXX(MLOG_LEVEL_FATAL, SUBJECT)
#define MLOG_ERROR_CXX(SUBJECT) MLOG_LEVEL_HEADER_CXX(MLOG_LEVEL_ERROR, SUBJECT)
#define MLOG_WARN_CXX(SUBJECT)  MLOG_LEVEL_HEADER_CXX(MLOG_LEVEL_WARN,  SUBJECT)
#define MLOG_DEBUG_CXX(SUBJECT) MLOG_LEVEL_HEADER_CXX(MLOG_LEVEL_DEBUG,  SUBJECT)
#define MLOG_FATAL_MORE_CXX()   MLOG_MORE_CXX()
#define MLOG_ERROR_MORE_CXX()   MLOG_MORE_CXX()
#define MLOG_WARN_MORE_CXX()    MLOG_MORE_CXX()
#define MLOG_DEBUG_MORE_CXX()   MLOG_MORE_CXX()

//C++-style API for optional key/info/march/trace mlogging controlled by mlogLevel
#define MLOG_KEY_CXX(SUBJECT)      MLOG_LEVEL_CXX(MLOG_LEVEL_KEY, SUBJECT)
#define MLOG_INFO_CXX(SUBJECT)     MLOG_LEVEL_CXX(MLOG_LEVEL_INFO, SUBJECT)
#define MLOG_MARCH_CXX(SUBJECT)    MLOG_LEVEL_CXX(MLOG_LEVEL_MARCH,  SUBJECT)
#define MLOG_TRACE_CXX(SUBJECT)    MLOG_LEVEL_CXX(MLOG_LEVEL_TRACE,  SUBJECT)
#define MLOG_KEY_MORE_CXX()        MLOG_LEVEL_MORE_CXX(MLOG_LEVEL_KEY)
#define MLOG_INFO_MORE_CXX()       MLOG_LEVEL_MORE_CXX(MLOG_LEVEL_INFO)
#define MLOG_MARCH_MORE_CXX()      MLOG_LEVEL_MORE_CXX(MLOG_LEVEL_MARCH)
#define MLOG_TRACE_MORE_CXX()      MLOG_LEVEL_MORE_CXX(MLOG_LEVEL_TRACE)

#endif /* __MLOGG_H__ */
