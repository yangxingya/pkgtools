//
//

#include "lineparser.h"
#include <glog/logging.h>
#include <cclib/strutil.h>
#include "except.h"
#include "error.h"
#include "entrydef.h"
#include "argv.h"

LineParser::LineParser(std::string const& line)
    : line_(line)
{
    LOG(INFO) << "Line Parser line: " << line_;
}

//!
/// define support format:
///  out: xxx.dat
///  file: [dstpath][srcpath][-p n/t c/e][-i n/t c/e][-u n/t c/e]
///  dir: [dirpath][-p n/t c/e][-i n/t -e c/e][-u n/t -e c/e]
///  exec:
argv::AutoArgv LineParser::doParse()
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
        throw pkg_error(ERROR_EntryTypeCantSupported, what);
    }

    argv::AutoArgv argvbase;

    switch (entrytype) {
    case entry::kOut:
        argvbase.reset(new argv::OutArgv(line_.substr(pos + 1)));
        break;
    case entry::kFile:
        argvbase.reset(new argv::FileArgv(line_.substr(pos + 1)));
        break;
    case entry::kDir:
        argvbase.reset(new argv::DirArgv(line_.substr(pos + 1)));
        break;
    case entry::kExec:
        argvbase.reset(new argv::ExecArgv(line_.substr(pos + 1)));
        break;
    case entry::kSetting:
        argvbase.reset(new argv::SettingArgv(line_.substr(pos + 1)));
    } 
    return argvbase;
}