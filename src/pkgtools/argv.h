//!
/// brief: define script command args
///

#if !defined(pkgtools_argv_h_)
#define pkgtools_argv_h_

#include <vector>
#include <string>
#include <cclib/strutil.h>
#include <cclib/types.h>
#include <glog/logging.h>
#include "error.h"
#include "except.h"
#include "entrydef.h"

namespace argv {

using namespace cclib;

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
const std::string kseparate = " ";
const std::string kpkgtstr = "-p";
const std::string kinststr = "-i";
const std::string kuninstr = "-u";
/// no error try prefix, error try value follow '-p/-i/-u'.
/// detail: -p n/t, -i n/t, -u n/t
/// no error exit prefix likely '-e b/i', is only b/i. so:
/// detail: -p n/t b/i, -i n/t b/i, -u n/t b/i

const std::string knotrystr  = "n";
const std::string ktrystr    = "t";
const std::string kbreakstr  = "b";
const std::string kignorestr = "i";

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
const uint8_t kenabled = 1;

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
struct ArgvBase {
public:
    //!
    /// brief: get base args by argv likely [-p/i/u n/t b/i] form, and transfort to base flags.
    ArgvBase(std::string const& argv, int type = entry::kUnknown)
        : p_flags_(0), i_flags_(0), u_flags_(0)
        , has_pkgt_(false), has_inst_(false), has_unin_(false)
        , type_(type)
    {
        parse(argv);
    }
    virtual ~ArgvBase() {}
    uint8_t pkgtFlags() const { return p_flags_; }
    uint8_t instFlags() const { return i_flags_; }
    uint8_t uninFlags() const { return u_flags_; }
    int type() const { return type_; }
private:
    uint8_t p_flags_;
    uint8_t i_flags_;  
    uint8_t u_flags_;
    bool has_pkgt_;
    bool has_inst_;
    bool has_unin_;
    int type_;
    void parse(std::string const& argv)
    {
        std::vector<std::string> argvs;
        cclib::split(argv, kargvbegin, kargvend, &argvs, true);
        for (size_t i = 0; i < argvs.size(); ++i) {
            if (cclib::start_with(argvs[i], kpkgtstr)) {
                /// like -p [n/t [b/i]]
                p_flags_ = parse_detail(argvs[i], kpkgtstr);
                has_pkgt_ = true;
                continue;
            }
            if (cclib::start_with(argvs[i], kinststr)) {
                /// like -i [n/t [b/i]]
                i_flags_ = parse_detail(argvs[i], kinststr);
                has_inst_ = true;
                continue;
            }
            if (cclib::start_with(argvs[i], kuninstr)) {
                /// like -u [n/t [b/i]]
                u_flags_ = parse_detail(argvs[i], kuninstr);
                has_unin_ = true;
                continue;
            }
            left_argv_.push_back(argvs[i]);
        }
    }  
    uint8_t parse_detail(std::string const& argv, std::string const& argv_pfx)
    {
        uint8_t flags = 0;
        /// like -?<argv> [n/t [b/i]]
        std::vector<std::string> out;
        cclib::split(argv, kseparate, &out, true);
        /// check split out argv number and out
        if (out.size() == 0 || out.size() > 3 || out[0] != argv_pfx) {
            std::string error = "Parse [";
            error += argv_pfx;
            error += " [n/t [b/i]]] failed, argv: ";
            error += argv;
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error);
        }

        /// pkgt flags enabled.
        flags |= 1;

        if (out.size() >= 2) {
            if (out[1] != knotrystr && out[1] != ktrystr) {
                std::string error = "Parse [";
                error += argv_pfx;
                error += " [n/t]] failed, argv: ";
                error += argv;
                LOG(ERROR) << error;
                throw script_error(ERROR_ScriptFormatError, error);
            }
            //TODO::...
            if (out[1] == ktrystr)
                flags |= kerrtry;
            /// notry str not set.
        }
        if (out.size() == 3) {
            if (out[2] != kbreakstr && out[2] != kignorestr) {
                std::string error = "Parse [";
                error += argv_pfx;
                error += " [n/t [b/i]]] failed, argv: ";
                error += argv;
                LOG(ERROR) << error;
                throw script_error(ERROR_ScriptFormatError, error);
            }
            //TODO::...
            if (out[2] == kignorestr)
                flags |= kerrignore;
            /// notry str not set.
        }
        DLOG(INFO) << "Parse detail, prefix: " << argv <<", flag(1<p/i/u>, 2<try>, 4<ignore>): " << (int)flags;
        return flags;
    }
protected:
    std::vector<std::string> left_argv_;
    bool hasPkgt() const { return has_pkgt_; }
    bool hasInst() const { return has_inst_; }
    bool hasUnin() const { return has_unin_; }
    
};

const std::string kfiledststr = "-d";
const std::string kfilesrcstr = "-s";
const char kspace = ' ';
//!
/// out argv format: out:[-d c:\xxx.dat]
struct OutArgv : public ArgvBase
{
    OutArgv(std::string const& argv)
        : ArgvBase(argv, entry::kOut)
    {
        parse();
    }
    std::string dst() const { return dst_; }
private:
    void parse()
    {
        bool has_dst = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspace) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    break;
                }
            }
        }
        if (!has_dst) {
            std::string error = "Parse Out Args [-d path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptOutPathFormatError, error);
        }
        DLOG(INFO) << "Out Argv: dst: \"" << dst_ << "\"";
    }
    std::string dst_;
};

//!
/// file:[-d dest] [-s src] [-p/i/u n/t b/i] 
struct FileArgv : public ArgvBase
{
public:
    FileArgv(std::string const& argv)
        : ArgvBase(argv, entry::kFile)
    {
        if (!hasPkgt() || !hasInst()) {
            std::string error = "Parse Args no 'pkg args' or 'inst args'";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    std::string dst() const { return dst_; }
    std::string src() const { return src_; }
private:
    std::string dst_;
    std::string src_;
    void parse()
    {
        bool has_dst = false;
        bool has_src = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspace) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    continue;
                }
            }
            if (cclib::start_with(left_argv_[i], kfilesrcstr)) {
                if (left_argv_[i][2] == kspace) {
                    has_src = true;
                    src_ = left_argv_[i].substr(3);
                    continue;
                }
            }
        }
        if (!has_dst || !has_src) {
            std::string error = "Parse File Args [-d path] [-s path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFilePathFormatError, error);
        }
        DLOG(INFO) << "File Argv: dst: \"" << dst_ << "\", src: \"" << src_ << "\"";
    }
};

//!
/// dir:[-d path] [-p n/t b/i] [-i n/t b/i] [-u n/t b/i]
struct DirArgv : public ArgvBase
{
public:
    DirArgv(std::string const& argv)
        : ArgvBase(argv, entry::kDir)
    {
        if (!hasPkgt() || !hasInst()) {
            std::string error = "Parse Args no 'pkg args' or 'inst args'";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    std::string dst() const { return dst_; }
private:
    std::string dst_;
    void parse()
    {
        bool has_dst = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspace) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    break;
                }
            }
        }
        if (!has_dst) {
            std::string error = "Parse Dir Args [-d path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptDirPathFormatError, error);
        }
        DLOG(INFO) << "Dir Argv: dst: \"" << dst_ << "\"";
    }
};

const std::string kcmdlinestr = "-c";
const std::string kexecutestr = "-v";

//!
/// exec:[-i n/t b/i] [-e c 0] [-c args list]
struct ExecArgv : public ArgvBase
{
public:
    ExecArgv(std::string const& argv)
        : ArgvBase(argv, entry::kExec)
    {
        if (!hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'inst args' or 'unin args'";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    uint8_t ckFlags() const { return ck_flags_; }
    uint8_t ckReturn()const { return ck_ret_;  }
    std::string cmd() const { return cmd_;     }
private:
    uint8_t ck_flags_;
    uint8_t ck_ret_;
    std::string cmd_;
    void parse()
    {
        bool has_cmd = false;
        bool has_ck  = false;

        ck_ret_ = 0;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kcmdlinestr)) {
                if (left_argv_[i][2] == kspace) {
                    has_cmd = true;
                    cmd_ = left_argv_[i].substr(3);   
                }
                continue;
            }
            if (cclib::start_with(left_argv_[i], kexecutestr)) {
                if (left_argv_[i][2] == kspace) {
                    std::vector<std::string> out;
                    cclib::split(left_argv_[i], " ", &out, true);
                    if (out.size() == 2) {
                        uint32_t ret = cclib::string_cast<uint32_t>(out[1]);
                        ck_ret_ = (uint8_t)ret;
                        has_ck = true;           
                    }
                }
            }
        }
        if (!has_cmd) {
            std::string error = "Parse Cmd Args [-c argvs] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptExecCmdFormatError, error);
        }
        ck_flags_ = has_ck;
        DLOG(INFO) << "Exec Argv: cmd: \"" << cmd_ << "\""
            << (has_ck ? ", check and value: " : ", no check: ")
            << (int32_t)(int8_t)ck_ret_;
    }
};

} // namespace argv

#endif // pkgtools_argv_h_
