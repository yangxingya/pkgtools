//
//

#include "script.h"
#include <cclib/strutil.h>
#include <glog/logging.h>
#include <fstream>
#include "lineparser.h"
#include "except.h"
#include "error.h"


Script::Script(std::string const& script)
    : script_(script)
{
    LOG(INFO) << "Script: " << script_ << ", will be parsed!";
}

int Script::Parse()
{
    std::ifstream ifs;
    ifs.open(script_.c_str());

    std::string line;
    int count = 1;
    LOG(INFO) << "Parse Script Begin: ------>";
    while (getline(ifs, line)) {
        LOG(INFO) << count++ << ": " << line;
        line = cclib::trim(line);
        if (line.empty())
            continue;

        try {
            LineParser parser(line);
            args_.push_back(parser.doParse());
        } catch (except_base &ex) {
            LOG(ERROR) << "Parse Script Error, script line: " << count - 1 <<". errcode: " << ex.error()
                << ". errstring: " << ex.what();
            return ex.error();
        }
    }
    LOG(INFO) << "Parse Script End: <------";

    if (args_.size() == 0) {
        LOG(ERROR) << "Parse Script NO Error, but no ENTRY also!";
        return ERROR_NoContentInScript;
    }
    LOG(INFO) << "Parse Script Successful, entry size: " << args_.size();
    return ERROR_Success;
}