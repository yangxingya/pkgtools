//!
/// brief: define script command args
///

#if !defined(pkgtools_argv_h_)
#define pkgtools_argv_h_

#include <vector>
#include <string>
#include <cclib/strutil.h>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include <glog/logging.h>
#include "except.h"
#include "argvdef.h"
#include "pkgdef.h"
#include "setting.h"

namespace argv {

using namespace cclib;
using namespace pkg;
using namespace err;

//! 
/// brief: base args: -p -i -u
///        details:
///             -p package
///             -i install
///             -u uninstall
/// detail:
///     file:[-d dest] [-s src] [-p/i/u [n/t [b/i]]] 
///     dir :[-d dest] [-p/i/u [n/t [b/i]]] 
/// 
struct ArgvBase {
public:
    //!
    /// brief: get base args by argv likely [-p/i/u n/t b/i] form, and transfort to base flags.
    ArgvBase(std::string const& argv, int type = kUnknown)
        : p_flags_(0), i_flags_(0), u_flags_(0)
        , has_pkgt_(false), has_inst_(false), has_unin_(false)
        , type_(type)
    {
        parse(argv);
    }
    
    ArgvBase(entry_t const& entry)
        : p_flags_(0), i_flags_(toInst(entry.flags)), u_flags_(toUnin(entry.flags))
        , has_pkgt_(false), has_inst_(toInst(entry.flags) != 0), has_unin_(toInst(entry.flags) != 0)
        , type_(toType(entry.type))
    {}

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
                p_flags_ |= kpkgtmask;
                has_pkgt_ = true;
                continue;
            }
            if (cclib::start_with(argvs[i], kinststr)) {
                /// like -i [n/t [b/i]]
                i_flags_ = parse_detail(argvs[i], kinststr);
                i_flags_ |= kinstmask;
                has_inst_ = true;
                continue;
            }
            if (cclib::start_with(argvs[i], kuninstr)) {
                /// like -u [n/t [b/i]]
                u_flags_ = parse_detail(argvs[i], kuninstr);
                u_flags_ |= kuninmask;
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

        /// '-p/i/u' format no [n/t [b/i]] suffix, set default value:
        if (out.size() == 1)
            flags = kdfttry | kdfterr;

        if (out.size() >= 2) {
            if (out[1] != knotrystr && out[1] != ktrystr) {
                std::string error = "Parse [";
                error += argv_pfx;
                error += " [n/t]] failed, argv: ";
                error += argv;
                LOG(ERROR) << error;
                throw script_error(ERROR_ScriptFormatError, error);
            }

            if (out[1] == ktrystr)
                flags = kerrtry;

            /// '-p/i/u n/t' format, no [b/i] then set [b/i] to default: ignored.
            if (out.size() == 2)
                flags = kdfterr;
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

            if (out[2] == kignorestr)
                flags |= kerrignore;
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
const std::string kfilenodelstr = "-x";
const char kspacech = ' ';
//!
/// out argv format: out:[-d c:\xxx.dat]
struct OutArgv : public ArgvBase
{
    OutArgv(std::string const& argv)
        : ArgvBase(argv, kOut)
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
                if (left_argv_[i][2] == kspacech) {
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
/// addf:[-d dst][-s src][-p/i/u [n/t [b/i]]]
/// must have [-p [n/t [b/i]]] section.
struct AddfArgv : public ArgvBase
{
    AddfArgv(std::string const& argv)
        : ArgvBase(argv, kAddf), src_valid_(false)
    {
        if (!hasPkgt()) {
            std::string error = "Parse Args no 'pkgt args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        if (!hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'inst args' or 'unin args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    AddfArgv(entry_t const& entry, std::string const& dst, uint64_t offset)
       : ArgvBase(entry)
       , dst_(dst)
       , offset_(offset)
    {}
    std::string dst() const { return dst_; }
    std::string src() const { return src_; }
    bool srcvalid() const { return src_valid_; }
    void srcvalid(bool v) { src_valid_ = v; }
    uint64_t offset() const { return offset_; }
private:
    std::string dst_;
    std::string src_;
    bool src_valid_;
    uint64_t offset_;

    void parse()
    {
        bool has_dst = false;
        bool has_src = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspacech) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    continue;
                }
            }
            if (cclib::start_with(left_argv_[i], kfilesrcstr)) {
                if (left_argv_[i][2] == kspacech) {
                    has_src = true;
                    src_ = left_argv_[i].substr(3);
                    continue;
                }
            }
        }
        if (!has_dst && !has_src) {
            std::string error = "Parse Addf Args [-d path] [-s path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFilePathFormatError, error);
        }
        DLOG(INFO) << "Addf Argv: dst: \"" << dst_ << "\", src: \"" << src_ << "\"";
            
    }
};

//!
/// delf:[-d dst][-i/u [n/t [b/i]]] 
struct DelfArgv : public ArgvBase
{
    DelfArgv(std::string const& argv)
        : ArgvBase(argv, kDelf)
    {
        if (!hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'inst args' or 'unin args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    DelfArgv(entry_t const& entry, std::string const& dst)
       : ArgvBase(entry)
       , dst_(dst)
    {}
    std::string dst() const { return dst_; }
private:
    std::string dst_;

    void parse()
    {
        bool has_dst = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspacech) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    break;
                }
            }
        }
        if (!has_dst) {
            std::string error = "Parse Delf Args [-d path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFilePathFormatError, error);
        }
        DLOG(INFO) << "Delf Argv: dst: \"" << dst_ << "\"";       
    }
};


//!
/// mkdir:[-d path] [-i/u [n/t [b/i]]]
struct MkdirArgv : public ArgvBase
{
public:
    MkdirArgv(std::string const& argv)
        : ArgvBase(argv, kMkdir)
    {
        if (!hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'inst args' or 'unin args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    MkdirArgv(entry_t const& entry, std::string const& dst)
        : ArgvBase(entry)
        , dst_(dst)
    { }
    std::string dst() const { return dst_; }
private:
    std::string dst_;
    void parse()
    {
        bool has_dst = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspacech) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    break;
                }
            }
        }
        if (!has_dst) {
            std::string error = "Parse Mkdir Args [-d path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptDirPathFormatError, error);
        }
        DLOG(INFO) << "Mkdir Argv: dst: \"" << dst_ << "\"";
    }
};

//!
/// rmdir:[-d path] [-i/u [n/t [b/i]]]
struct RmdirArgv : public ArgvBase
{
public:
    RmdirArgv(std::string const& argv)
        : ArgvBase(argv, kRmdir)
    {
        if (!hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'inst args' or 'unin args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    RmdirArgv(entry_t const& entry, std::string const& dst)
        : ArgvBase(entry)
        , dst_(dst)
    { }
    std::string dst() const { return dst_; }
private:
    std::string dst_;
    void parse()
    {
        bool has_dst = false;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kfiledststr)) {
                if (left_argv_[i][2] == kspacech) {
                    has_dst = true;
                    dst_ = left_argv_[i].substr(3);
                    break;
                }
            }
        }
        if (!has_dst) {
            std::string error = "Parse Rmdir Args [-d path] failed!";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptDirPathFormatError, error);
        }
        DLOG(INFO) << "Rmdir Argv: dst: \"" << dst_ << "\"";
    }
};

const std::string kcmdlinestr = "-c";
const std::string kexecutestr = "-e";

//!
/// exec:[-p/i/u [n/t [b/i]]] [-e c 0] [-c args list]
struct ExecArgv : public ArgvBase
{
public:
    ExecArgv(std::string const& argv)
        : ArgvBase(argv, kExec)
    {
        if (!hasPkgt() && !hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'pkgt args' or 'inst args' or 'unin args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }
        parse();
    }
    ExecArgv(entry_t const& entry, std::string const& cmdline)
        : ArgvBase(entry)
        , ck_flags_(toCheckRet(entry.flags))
        , cmd_(cmdline)
        , ck_ret_(entry.args)
    { }
    uint8_t ckFlags()  const { return ck_flags_; }
    uint32_t ckReturn()const { return ck_ret_;   }
    std::string cmd()  const { return cmd_;      }
private:
    uint8_t ck_flags_;
    uint32_t ck_ret_;
    std::string cmd_;
    void parse()
    {
        bool has_cmd = false;
        bool has_ck  = false;

        ck_ret_ = 0;

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], kcmdlinestr)) {
                if (left_argv_[i][2] == kspacech) {
                    has_cmd = true;
                    cmd_ = left_argv_[i].substr(3);   
                }
                continue;
            }
            if (cclib::start_with(left_argv_[i], kexecutestr)) {
                if (left_argv_[i][2] == kspacech) {
                    std::vector<std::string> out;
                    cclib::split(left_argv_[i], kseparate, &out, true);
                    if (out.size() == 2) {
                        uint32_t ret = cclib::string_cast<uint32_t>(out[1]);
                        ck_ret_ = ret;
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
            << (int32_t)ck_ret_;
    }
};

const std::string ksettingstr = "-s";

//!
/// setting:[-p/i/u [n/t [b/i]]] [-s setting]
struct SettingArgv : public ArgvBase
{
    SettingArgv(std::string const& argv)
        : ArgvBase(argv, kSetting)
    {
        if (!hasPkgt() && !hasInst() && !hasUnin()) {
            std::string error = "Parse Args no 'pkgt args' or 'inst args' or 'unin args': \"";
            error += argv;
            error += "\".";
            LOG(ERROR) << error;
            throw script_error(ERROR_ScriptFormatError, error); 
        }

        parse();
        /// check argv supported ???
        if (Setting::valid(flags_) == kinvalidargs) {
            /// error, not supported args.
            std::string error = "Setting parse args: \"";
            error += flags_;
            error += "\" not supported";

            LOG(ERROR) << error;
            throw setting_error(ERROR_SettingArgsNotSupported, error);
        }
    }

    SettingArgv(entry_t entry, std::string const& flags)
        : ArgvBase(entry)
        , flags_(flags)
    { }
    std::string flags() const { return flags_; }
private:
    std::string flags_;

    void parse()
    {
        /// left format: one set one line.
        /// "-s <d/e>fsrd" / "-s xxxx"

        for (size_t i = 0; i < left_argv_.size(); ++i) {
            if (cclib::start_with(left_argv_[i], ksettingstr)) {
                if (left_argv_[i][2] == kspacech) {
                    flags_ = left_argv_[i].substr(3);
                    return;
                }
            }
        }
        /// can't find "-s xxx" format string.
        std::string error = "Parse Setting Args [-s path] failed!";
        LOG(ERROR) << error;
        throw script_error(ERROR_ScriptSettingFormatError, error);
        
        DLOG(INFO) << "Setting Argv: flags: \"" << flags_ << "\"";
    }
};

namespace helper {

inline entry_t transfer(argv::AddfArgv *argv)
{
    entry_t entry = {0};

    entry.type = kentryaddf;
    entry.flags = argv->instFlags() | (argv->uninFlags() << kflagsuninshift);

    return entry;
}

inline entry_t transfer(argv::DelfArgv *argv)
{
    entry_t entry = {0};

    entry.type = kentrydelf;
    entry.flags = argv->instFlags() | (argv->uninFlags() << kflagsuninshift);

    return entry;
}

inline entry_t transfer(argv::MkdirArgv *argv)
{
    entry_t entry = {0};

    entry.type = kentrymkdir;
    entry.flags = argv->instFlags() | (argv->uninFlags() << kflagsuninshift);
    entry.dtaindex = kinvalid; 

    return entry;
}

inline entry_t transfer(argv::RmdirArgv *argv)
{
    entry_t entry = {0};

    entry.type = kentryrmdir;
    entry.flags = argv->instFlags() | (argv->uninFlags() << kflagsuninshift);
    entry.dtaindex = kinvalid; 

    return entry;
}

inline entry_t transfer(argv::ExecArgv *argv)
{
    entry_t entry = {0};

    entry.type = kentryexec;
    entry.flags = 
        argv->instFlags() | 
        (argv->uninFlags() << kflagsuninshift) | 
        (argv->ckFlags() << kflagsckshift);
    entry.args = argv->ckFlags() ? argv->ckReturn() : 0;
    entry.dtaindex = kinvalid; 

    return entry;
}

inline entry_t transfer(argv::SettingArgv *argv)
{
    entry_t entry = {0};

    entry.type = kentrysetting;
    entry.flags = 
        argv->instFlags() | 
        (argv->uninFlags() << kflagsuninshift);
    entry.dtaindex = kinvalid; 

    return entry;
}

inline entry_t extract(argv::AutoArgv const& argv)
{
    entry_t entry;
    entry.type = kentryunknown;

    switch (argv->type()) {
    case kAddf:  return transfer((argv::AddfArgv*)(argv.get()));
    case kDelf:  return transfer((argv::DelfArgv*)(argv.get()));
    case kMkdir: return transfer((argv::MkdirArgv*)(argv.get()));
    case kRmdir: return transfer((argv::RmdirArgv*)(argv.get()));
    case kExec:  return transfer((argv::ExecArgv*)(argv.get()));
    case kSetting: return transfer((argv::SettingArgv*)(argv.get()));
    default: return entry;
    }
}

} // namespace helper

} // namespace argv

#endif // pkgtools_argv_h_
