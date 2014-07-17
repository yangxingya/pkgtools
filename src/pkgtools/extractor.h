//!
/// brief: pkgextractor.h define package extractor.
///

#if !defined(pkgtools_pkg_extractor_h_)
#define pkgtools_pkg_extractor_h_

#include <string>
#include <cclib/types.h>
#include <cclib/win32/pathutil.h>
#include <glog/logging.h>
#include "pkgfile.h"
#include "pkgdef.h"
#include "unpacker.h"
#include "argvtrans.h"

namespace pkg {

using namespace cclib;

struct Extractor
{
    Extractor(std::string const& pkgfile, std::string const& todir)
        : unpacker_(pkgfile), todir_(todir) 
    {
    }

    int extract()
    {
        ex_data();
        //argv::restorer restor(unpacker_.arglist(), out_args_, 
        return ERROR_Success;
    }

private:
    unpacker unpacker_;
    std::string todir_;

#pragma region extract
    
    bool ex_data()
    {
        std::vector<entry_t> const& entrys = unpacker_.entrys();
        std::string to;
        for (size_t i = 0L; i < entrys.size(); ++i) {
            switch (entrys[i].type) {
            case kentryfile:
                to = ex_path(entrys[i].strindex);
                unpacker_.tofile(to, entrys[i].dtaindex);
                break;
            case kentrydir:
                to = ex_path(entrys[i].strindex);
                win32::mkdirtree(to);
                break;
            case kentryexec:
                break;
            }
        }
        return true;
    }

    std::string ex_path(uint64_t aindex)
    { 
        std::vector<std::string> const& arglist = unpacker_.arglist();
        std::string path = arglist[(uint32_t)aindex];

        if (path[0] != '$' && path[1] == ':')
            path[1] = '$';

        std::string todir = todir_;

        win32::add_sep(todir);

        std::string newpath = todir + path;
        
        return newpath;
    }
#pragma endregion extract all parts.
};

} // namespace pkg

#endif // pkgtools_pkg_extractor_h_