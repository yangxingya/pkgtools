//!
/// brief: lineparser.h define parser for script line.
///


#if !defined(pkgtools_line_parser_h_)
#define pkgtools_line_parser_h_

#include <string>
#include <cclib/smartptr_def.h>

struct EntryBase;
struct LineParser
{
    LineParser(std::string const& line);
    shared_ptr<EntryBase> doParse(); 
    shared_ptr<PkgBase> doPkg();
private:
    std::string line_;
};

#endif // pkgtools_line_parser_h_