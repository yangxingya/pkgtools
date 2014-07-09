//!
/// brief: define script command args
///

#if !defined(pkgtools_argv_h_)
#define pkgtools_argv_h_

#include <vector>
#include <string>
#include <cclib/strutil.h>
#include <cclib/types.h>

//!
/// detail:
///   flags:
///     7 6 5 4 3 2 1 0
///                   ^
///                  p/i/u
///                 ^
///                 t(error try?)
///               ^
///               e(error exit?)

const uint8_t kpkgtmask = 1;    /// 0x01
const uint8_t kinstmask = 1;    /// 0x01
const uint8_t kuninmask = 1;    /// 0x01  

/// error try mask
const uint8_t kerrtmask = 2;    /// 0x02
/// error exit mask
const uint8_t kerrxmask = 4;    /// 0x04

const std::string kargvbegin = "[";
const std::string kargvend = "]";
const std::string kpkgtstr = "-p";
const std::string kinststr = "-i";
const std::string kuninstr = "-u";
/// no error try prefix, error try value follow '-p/-i/-u'.
/// detail: -p n/t, -i n/t, -u n/t
/// no error exit prefix likely '-e b/i', is only b/i. so:
/// detail: -p n/t b/i, -i n/t b/i, -u n/t b/i

/// error try value.
const uint8_t kerrtry = 2;
/// error no try value.
const uint8_t kerrnotry = 0;
/// error ignore value.
const uint8_t kerrignore = 4;
/// error break value.
const uint8_t kerrbreak = 0;
///!
/// the following is default value.
const uint8_t kdfttry = kerrtry; 
const uint8_t kdfterr = kerrignore;

const uint8_t kdftpkgt = 1;
const uint8_t kdftinst = 1;
const uint8_t kdftunin = 0;

/// the following is all combined default flags.
const uint8_t kdftpcomb = kdftpkgt | kdfttry | kdfterr;
const uint8_t kdfticomb = kdftinst | kdfttry | kdfterr;
const uint8_t kdftucomb = kdftunin | kdfttry | kdfterr;


//! 
/// brief: base args: -p -i -u
///        details:
///             -p package
///             -i install
///             -u uninstall
/// detail:
///     file:[-d dest] [-s src] [-p/i/u n/t b/i] 
///     dir :[-d dest] [-p/i/u n/t b/i] 
/// 
struct BaseArgv {
public:
    //!
    /// brief: get base args by argv likely [-p/i/u n/t b/i] form, and transfort to base flags.
    BaseArgv(std::string const& argv)
        : p_flags_(kdftpcomb)
        , i_flags_(kdfticomb)
        , u_flags_(kdftucomb)
    {
        std::vector<std::string> argvs;
        cclib::split(argv, kargvbegin, kargvend, &argvs, true);
        for (size_t i = 0; i < argvs.size(); ++i) {
            if (cclib::start_with(argvs[i], kpkgtstr)) {

                continue;
            }
        }
    }
    virtual ~BaseArgv() {}
    uint8_t pkgtFlags() const { return p_flags_; }
    uint8_t instFlags() const { return i_flags_; }
    uint8_t uninFlags() const { return u_flags_; }
private:
    uint8_t p_flags_;
    uint8_t i_flags_;  
    uint8_t u_flags_; 
protected:
    std::string argv_;
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
