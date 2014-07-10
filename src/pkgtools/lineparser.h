//!
/// brief: lineparser.h define parser for script line.
///


#if !defined(pkgtools_line_parser_h_)
#define pkgtools_line_parser_h_

#include <string>
#include "argvdef.h"

struct LineParser
{
    LineParser(std::string const& line);
    argv::AutoArgv doParse(); 
private:
    std::string line_;
};

#endif // pkgtools_line_parser_h_