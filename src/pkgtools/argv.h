//!
/// brief: define script command args
///

#if !defined(pkgtools_argv_h_)
#define pkgtools_argv_h_

#include <vector>
#include <string>
#include "strutil.h"

const int kInstall   = 1;
const int kUninstall = 2;
const int kContinue  = 4;

const int kDefult = kInstall;

const std::string kInstallStr   = "-i";
const std::string kUninstallStr = "-u";
const std::string kContinueStr  = "-c";

//! 
/// brief: base args: -i -u -c
///        details: 
///             -i install
///             -u uninstall
///             -c cmd run failed, ignore? default is not ignore.
/// example: file: [-i -u -c] ["filename"]
struct BaseArgv {
public:
    //!
    /// brief: get base args by argv likely [-i -u -c] form, and transfort to base flags.
    BaseArgv(std::string const& argv)
        : flags_(kDefault)
    {
        std::vector<std::string> out;
        cclib::split(argv, " ", &out, true);
        std::vector<std::string>::iterator it;
        for (it = out.begin(); it != out.end(); ++it) {
            if (*it == kInstallStr  ) flags_ |= kInstall;
            else
            if (*it == kUninstallStr) flags_ |= kUnstall;
            else
            if (*it == kContinueStr ) flags_ |= kContinue;
        }
    }
    virtual ~BaseArgv() {}
    int Flags() const { return flags_; }
private:
    int flags_;  
};

struct FileArgv : public BaseArgv
{
public:
    FileArgv(std::vector<std::string> const& argv)
        : BaseArgv(argv[0]) 
    {
        
    }

    // valid file: [-i -u -c] [C:\\Program Files\\xxx\\a.txt]
    static bool valid(std::vector<std::string> const& argv)
    {
        // it must has base args section [-i -u -c]
        // and must has file section [C:\\Program Files\\xxx\\a.txt]
        //
        // SO its size must large or equal 2.
        return argv.size() >= 2;
    }
};

#endif // pkgtools_argv_h_
