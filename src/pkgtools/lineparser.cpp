//
//

#include "lineparser.h"
#include <glog/logging.h>
#include "except.h"
#include "error.h"
#include "entrydef.h"
#include "entry.h"

LineParser::LineParser(std::string const& line)
    : line_(line)
{
    LOG(INFO) << "Line Parser line: " << line_;
}

//!
/// define support format:
///  out: xxx.dat
///  file: [dstpath][srcpath][-p n/e -e c/e][-i n/e -e c/e][-u n/e -e c/e]
///  dir: [dirpath][-p n/e -e c/e][-i n/e -e c/e][-u n/e -e c/e]
///  exec:
shared_ptr<EntryBase> LineParser::doParse()
{
    std::string::size_type pos = line_.find_first_of(":");
    if (pos == std::string::npos)
        throw pkg_error(ERROR_ScriptLineNoEntrySplitChar, "no entry split char(:) at script line");
    
    std::string type = line_.substr(0, pos);
    int entrytype = entry::type(type);
    if (entrytype == entry::kUnknown) {
        std::string what = "entry type: ";
        what += type;
        what += " can't supported!";
        throw pkg_error(ERROR_EntryTypeCantSupported, what.c_str());
    }

    shared_ptr<EntryBase> entrybase; 
    bool valid;
    switch (entrytype) {
    case entry::kOut:
        //TODO:: not implement.
        throw no_impl();
        break;
    case entry::kFile:
        entrybase.reset(new FileEntry(line_.substr(pos + 1), valid));
        break;
    case entry::kDir:
        //TODO:: not implement.
        throw no_impl();
        break;
    case entry::kExec:
        //TODO:: not implement.
        throw no_impl();
        break;
    }
    
    return entrybase;
}