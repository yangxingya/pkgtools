//!
/// brief: lineparser.h define parser for script line.
///


#if !defined(pkgtools_line_parser_h_)
#define pkgtools_line_parser_h_

#include <string>

struct LineParser
{
    LineParser(std::string const& line);
};

#endif // pkgtools_line_parser_h_