//
//

#include "lineparser.h"
#include <glog/logging.h>
#include <cclib/strutil.h>
#include "except.h"
#include "argvdef.h"
#include "argv.h"

using namespace err;

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
    int entrytype = argv::type(type);
    if (entrytype == argv::kUnknown) {
        std::string what = "entry type: ";
        what += type;
        what += " can't supported!";
        throw pkg_error(ERROR_EntryTypeCantSupported, what);
    }

    argv::AutoArgv argvbase;

    switch (entrytype) {
    case argv::kOut:
        argvbase.reset(new argv::OutArgv(line_.substr(pos + 1)));
        break;
    case argv::kAddf:
        argvbase.reset(new argv::AddfArgv(line_.substr(pos + 1)));
        break;
    case argv::kDelf:
        argvbase.reset(new argv::DelfArgv(line_.substr(pos + 1)));
        break;
    case argv::kMkdir:
        argvbase.reset(new argv::MkdirArgv(line_.substr(pos + 1)));
        break;
    case argv::kRmdir:
        argvbase.reset(new argv::RmdirArgv(line_.substr(pos + 1)));
        break;
    case argv::kExec:
        argvbase.reset(new argv::ExecArgv(line_.substr(pos + 1)));
        break;
    case argv::kSetting:
        argvbase.reset(new argv::SettingArgv(line_.substr(pos + 1)));
    } 
    return argvbase;
}