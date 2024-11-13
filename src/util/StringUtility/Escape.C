#include <Escape.h>
#include <StringToNumber.h>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <Replace.h>
#include <cstring>

namespace Rose {
namespace StringUtility {

std::string
htmlEscape(const std::string& s) {
    std::string s2;
    for (size_t i = 0; i < s.size(); ++i) {
        switch (s[i]) {
            case '<': s2 += "&lt;"; break;
            case '>': s2 += "&gt;"; break;
            case '&': s2 += "&amp;"; break;
            default: s2 += s[i]; break;
        }
    }
    return s2;
}

std::string
cEscape(char ch, char context) {
    std::string result;
    switch (ch) {
        case '\a':
            result += "\\a";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\t':
            result += "\\t";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\v':
            result += "\\v";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\"':
            if ('"' == context) {
                result += "\\\"";
            } else {
                result += ch;
            }
            break;
        case '\'':
            if ('\'' == context) {
                result += "\\'";
            } else {
                result += ch;
            }
            break;
        case '\\':
            result += "\\\\";
            break;
        default:
            if (isprint(ch)) {
                result += ch;
            } else {
                char buf[8];
                sprintf(buf, "\\%03o", (unsigned)(unsigned char)ch);
                result += buf;
            }
            break;
    }
    return result;
}

std::string
cEscape(const std::string &s, char context) {
    std::string result;
    for (char ch: s)
        result += cEscape(ch, context);
    return result;
}

std::string
cUnescape(const std::string &s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if ('\\' == s[i] && i+1 < s.size()) {
            ++i;
            switch (s[i]) {
                case '?':
                    result += '?';
                    break;
                case '"':
                    result += '"';
                    break;
                case '\'':
                    result += '\'';
                    break;
                case 'a':
                    result += '\a';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'u':
                    result += "\\u";                    // Unicode is not supported; leave it escaped
                    break;
                case 'U':
                    result += "\\U";                    // Unicode is not supported; leave it escaped
                    break;
                case 'v':
                    result += '\v';
                    break;
                case 'x': {
                    unsigned byte = 0;
                    while (i+1 < s.size() && isxdigit(s[i+1]))
                        byte = ((16*byte) + hexadecimalToInt(s[++i])) & 0xff;
                    result += (char)byte;
                    break;
                }
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7': {
                    unsigned byte = 0, nchars = 0;
                    --i;
                    while (i+1 < s.size() && nchars++ < 3 && strchr("01234567", s[i+1]))
                        byte = 8*byte + s[++i] - '0';
                    result += (char)byte;
                    break;
                }
                default:
                    result += std::string("\\") + s[i];
            }
        } else {
            result += s[i];
        }
    }
    return result;
}

// Escaping special characters in shells is difficult. There are many classes of characters:
//
//   1. Characters that can appear unescaped and outside quotes (bare). They can also appear unescaped inside single or
//      double quotes.  Most characters fall into this category.
//
//   2. Characters that must be escaped when outside quotes or (not escaped) inside double or single quotes.  These are
//      characters like "*", "?", "=", "{", "}", "[", "]", etc. and consist of most printable non-alphanumeric characters.
//
//   3. Characters that cannot be escaped by some shells. These are things like non-printable control characters
//      and characters above 0177. The Bash shell has a special syntax for escaping these in a C-like manner.
std::string
bourneEscape(const std::string &s) {
    // If the string is empty it needs to be quoted.
    if (s.empty())
        return "''";

    // The presence of non-printing characters or single quotes trumps all others and requires C-style quoting
    for (char ch: s) {
        if (!::isprint(ch) || '\'' == ch)
            return "$'" + cEscape(s, '\'') + "'";
    }

    // If the string contains any shell meta characters or white space that must be quoted then single-quote the entire string
    // and escape backslashes.
    for (char ch: s) {
        if (!::isalnum(ch) && !strchr("_-+./", ch))
            return "'" + Rose::StringUtility::replaceAllCopy(s, "\\", "\\\\") + "'";
    }

    // No quoting or escaping necessary
    return s;
}

std::string
yamlEscape(const std::string &s) {
    const std::string escaped = cEscape(s);
    if (s.empty() || s != escaped) {
        return "\"" + escaped + "\"";
    } else if (s.find(':') != std::string::npos) {
        return "\"" + s + "\"";
    } else {
        std::string lc = s;
        for (char&c: lc) c = std::tolower(c); //convert to lower case
        if ("yes" == lc || "true" == lc || "no" == lc || "false" == lc) {
            return"\"" + s + "\"";
        } else {
            return s;
        }
    }
}

std::string
csvEscape(const std::string &s) {
    const std::string quote = s.find_first_of(",\r\n\"") == std::string::npos ? "" : "\"";
    return quote + Rose::StringUtility::replaceAllCopy(s, "\"", "\"\"") + quote;
}

// DQ (12/8/2016): This version fixed most of the issues that Robb raised with the escapeNewLineCharaters() function.
// Unaddressed is: point #4. It escapes newlines using "l" rather than the more customary "n".
// I have implemented this to support the DOT graphics where it is used.  This can be a subject
// for discussion at some point a bit later (it is late evening on the west coast presently).
std::string
escapeNewlineAndDoubleQuoteCharacters ( const std::string & X )
   {
     std::string returnString;
     int stringLength = X.length();

     for (int i=0; i < stringLength; i++)
        {
          if ( X[i] == '\n' )
             {
               returnString += "\\l";
             }
          else
             {
               if ( X[i] == '\"' )
                  {
                    returnString += "\\\"";
                  }
               else
                  {
                    returnString += X[i];
                  }
             }
        }

     return returnString;
   }



} // namespace
} // namespace

std::string escapeString(const std::string & s) {

// DQ (2/4/2014): Note that this function can not be used on the asm strings
// for the unparser.  When it can frequently work, the present of "\n" and "\t"
// substrings causes the transformation to "\\n" and \\t" which is a bug when
// used with the asmembler.  So a special version of this function that does
// not process "\n" to "\\n" is defined in the unparser.  Note that we still
// need to process '\n' to "\n" and this behavior is still preserved.
// Test codes demonstrating this are in test2014_83-87.c.

  std::string result;
  for (size_t i = 0; i < s.length(); ++i)
  {
    switch (s[i]) {
      case '\\':
#if 0
        printf ("In escapeString(): processing \\\\ character \n");
#endif
        result += "\\\\";
        break;
      case '"':
#if 0
        printf ("In escapeString(): processing \\\" character \n");
#endif
        result += "\\\"";
        break;
      case '\a':
#if 0
        printf ("In escapeString(): processing \\a character \n");
#endif
        result += "\\a";
        break;
      case '\f':
#if 0
        printf ("In escapeString(): processing \\f character \n");
#endif
        result += "\\f";
        break;
      case '\n':
#if 0
        printf ("In escapeString(): processing \\n character \n");
#endif
        result += "\\n";
        break;
      case '\r':
#if 0
        printf ("In escapeString(): processing \\r character \n");
#endif
        result += "\\r";
        break;
      case '\t':
#if 0
        printf ("In escapeString(): processing \\t character \n");
#endif
        result += "\\t";
        break;
      case '\v':
#if 0
        printf ("In escapeString(): processing \\v character \n");
#endif
        result += "\\v";
        break;
      default:
#if 0
        printf ("In escapeString(): processing default case character \n");
#endif
        if (isprint(s[i])) {
          result.push_back(s[i]);
        } else {
          std::ostringstream stream;
          stream << '\\';
          stream << std::setw(3) << std::setfill('0') <<std::oct << (unsigned)(unsigned char)(s[i]);
          result += stream.str();
        }
        break;
    }
  }
  return result;
}

std::string unescapeString(const std::string& s) {
  std::string result;
  for (size_t i = 0; i < s.length(); ++i) {
    // Body of this loop can change i
    if (s[i] == '\\') {
      assert (i + 1 < s.length());
      switch (s[i + 1]) {
        case 'a': result += '\a'; break;
        case 'e': result += '\033'; break;
        case 'f': result += '\f'; break;
        case 'n': result += '\n'; break;
        case 'r': result += '\r'; break;
        case 't': result += '\t'; break;
        case 'v': result += '\v'; break;
        case '0':
        case '1':
        case '2':
        case '3': {
          ++i;
          if (s[i] == '0' && i + 1 == s.length()) {
            result += '\0';
            break;
          }
          assert (i + 2 < s.length());
          unsigned char c = (s[i] - '0') << 6;
          ++i;
          assert (s[i] >= '0' && s[i] <= '7');
          c |= (s[i] - '0') << 3;
          ++i;
          assert (s[i] >= '0' && s[i] <= '7');
          c |= (s[i] - '0');
          break;
        }
        default: ++i; result += s[i]; break;
      }
    } else if (s[i] == '"') {
      return result;
    } else {
      result += s[i];
    }
  }
  return result;
}

