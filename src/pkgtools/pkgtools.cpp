//!
/// pkgtools.cpp: package tools main function define.
///
/// date: 2014-07-01 19:55
/// copyright: easytek Comp.
/// author: yangxingya
/// email: xingyayang@163.com

/// test:
///     project setting: argv = ' -p pkg.script '
///     workdir        : $(OutDir)

#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <glog/logging.h>
#include <cclib/singleton.h>
#include <cclib/win32/disabler.h>
#include "error.h"
#include "script.h"
#include "argvtrans.h"
#include "installer.h"
#include "uninstaller.h"
#include "maker.h"
#include "extractor.h"


void initializelog(const char *argv0);
void usage();
int install(std::string const& pkgfile, bool witharg = false, std::string const& arglist = "");
int uninstall(std::string const& pkgfile);
int package(std::string const& sptfile);
int extract(std::string const& pkgfile, std::string const& outdir);
int formaterror(std::string const& errcode);
int main(int argc, char *argv[])
{
    initializelog(argv[0]);

    //void test();
    //test();

    // log all args.
    std::string log_args = argv[0];
    for (int i = 1; i < argc; ++i) {
        log_args += " ";
        log_args += argv[i];
    }
    LOG(INFO) << "Commandline: \"" << log_args << "\"";

    if (argc < 3) {
        usage();
        return ERROR_UsageInvalid; 
    }

    if (std::string(argv[1]) == "-i") {
        /// install with args.
        if (argc == 4)
            return install(argv[2], true, argv[3]);
        return install(argv[2]);
    }

    if (std::string(argv[1]) == "-u")
        return uninstall(argv[2]);

    if (std::string(argv[1]) == "-p")
        return package(argv[2]);
    
    if (std::string(argv[1]) == "-e") {
        std::string outdir = ".";
        if (argc >= 4)
            outdir = argv[3];
        return extract(argv[2], argv[3]);
    }

    if (std::string(argv[1]) == "-x")
        return formaterror(argv[2]);

    usage();
    return ERROR_UsageInvalid; 
}

//!
/// brief: initialize log system, this project use [google-glog]
///        for log system.
void initializelog(const char *argv0)
{
    google::InitGoogleLogging(argv0);
    //TODO:: need get programm path.
    std::string programm_path = ".";
    std::string programm_short = "pkgtools";
    std::string log_base_fn = programm_path + "/log/" + programm_short;
    // only log large than INFO level log.
    google::SetLogDestination(google::GLOG_INFO, log_base_fn.c_str());
    // set error level log file is empty.
    google::SetLogDestination(google::GLOG_ERROR, "");
    // set warn level log file is empty.
    google::SetLogDestination(google::GLOG_WARNING, "");

    // log buffer time(second) default is 30s. 
    // set 0 will output immediately.
    FLAGS_logbufsecs = 0; 
    // max log size(MB). set to 10MB.
    FLAGS_max_log_size = 100; 
    // if disk full then stop logging. set true.
    FLAGS_stop_logging_if_full_disk = true;
    // console output with color.
    FLAGS_colorlogtostderr = true;

    //google::SetLogFilenameExtension("91_"); //
    //google::InstallFailureSignalHandler();  //捕捉 core dumped (linux)
    //google::InstallFailureWriter(&Log);     //默认捕捉 SIGSEGV 信号信息输出会输出到 stderr (linux)
}

//!
/// brief: alignfill for usage output: "-i" to "-i      ".
///        fill 'str' suffix with space:' ',
///        and the filled 'str' length is 'spacesz'.
/// note : if 'str' length is large than spacesz then return 'str'.
std::string alignfill(std::string const& str, size_t spacesz)
{
    size_t fillsz = spacesz > str.length() ? spacesz - str.length() : 0;
    std::string alignstr(fillsz, ' ');
    alignstr = str + alignstr;
    return alignstr;
}

//!
/// brief: pkgtools usage std output.
void usage()
{
    std::string spacepfx(2, ' ');
    size_t spacesz = 8;
    std::cout
        << "pkgtools [-i | -u | -p | -e | -x] [file | error] [install arglist] [outpath] \n\n"
        << spacepfx + alignfill("-i", spacesz)
        << "install package from file:<installed-package>. and you can install with args \"arglist\" format.\n"
        << spacepfx + alignfill("-u", spacesz)
        << "uninstall package from file:<installed-package>.\n"
        << spacepfx + alignfill("-p", spacesz)
        << "make install-package by file:<package-script>.\n" 
        << spacepfx + alignfill("-e", spacesz)
        << "extract file:<installed-package> to outpath:<special out dir>.\n"
        << spacepfx + alignfill("-x", spacesz)
        << "format pkgtools return code by error:<error code>.\n"
        << spacepfx + alignfill("file", spacesz)
        << "file can be installed-package, package-script.\n"
        << spacepfx + alignfill("error", spacesz)
        << "error is a error-code returned by pkgtools programm.\n"
         << spacepfx + alignfill("outpath", spacesz)
        << "outhpath is a dir that extracted installed-package to.\n"
        << std::endl;
}

//!
/// brief: install a pragram by pkgfile.
int install(std::string const& pkgfile, bool witharg, std::string const& arglist)
{
    LOG(INFO) << "Install Start, package name: " << pkgfile;
    
    int ret;

    shared_ptr<pkg::Installer> installer;
    try {
        installer.reset(new pkg::Installer(pkgfile, witharg, arglist));
        ret = installer->install();
    } catch (except_base &ex) {
        return ex.error();
    }

    LOG(INFO) << "Install Over!";
    return ret;
}

//!
/// brief : uninstall a pragram installed by pkgfile.
/// notice: UNINSTALL BY pkgfile, SO YOU MUST WRITE
///         UNINSTALL INFO AT pkgfile, and script MUST
///         INCLUDE unstall section.
int uninstall(std::string const& pkgfile)
{
    LOG(INFO) << "Uninstall Start, package name: " << pkgfile;

    int ret;

    shared_ptr<pkg::Uninstaller> uninstaller;
    try {
        uninstaller.reset(new pkg::Uninstaller(pkgfile));
        ret = uninstaller->uninstall();
    } catch (except_base &ex) {
        return ex.error();
    }

    LOG(INFO) << "Uninstall Over!";
    return -1; 
}

//! 
/// brief : make package by sptfile:<script file>.
///
int package(std::string const& sptfile)
{
    LOG(INFO) << "Package Start, script name: " << sptfile;

    int ret;

    Script script(sptfile);
    ret = script.Parse();
    if (ret != ERROR_Success)
        return ret;

    argv::AutoArgvList arglist = script.ArgList();
    ///TODO:: find arglist if disable wow64 fs redirection???
    /// 1. find 
    /// 2. disable.

    /// disable wow64 fs redirection.
    /// cclib::win32::Wow64FileSystem wow64fs;
    /// cclib::win32::disabler<cclib::win32::Wow64FileSystem> disable(wow64fs);

    /// file open checker.
    /// if file open check failed, then return failed.
    for (size_t i = 0; i < arglist.size(); ++i) {
        switch (arglist[i]->type()) {
        case entry::kSetting:
            {
                argv::SettingArgv *sargv = (argv::SettingArgv*)arglist[i].get();
                if (sargv->pkgtFlags()) {
                    pkg::Setting setting(sargv->flags());
                    if ((ret = setting.doSet()) != ERROR_Success)
                        return ret;
                }
            }
            break;
        case entry::kFile:
            {
                argv::FileArgv *fargv = (argv::FileArgv*)arglist[i].get();
                if ((ret = fargv->pkgPreCheck()) != ERROR_Success)
                    return ret;
            }
            break;
        default:
            break;
        }
    }

    /// package pre check and remove invalid argv.
    /// main arg is file arg, because file arg package
    /// will be package to output file, if you can't 
    /// open source file, how to copy to package???
    std::string opt_file = pkg::kdefaultoutput;
    argv::AutoArgvList::iterator it;
    for (it = arglist.begin(); it != arglist.end(); ) {
        if ((*it)->type() == entry::kFile) {
            argv::FileArgv *fargv = (argv::FileArgv*)(*it).get();
            if (fargv->get().get() == NULL)
                it = arglist.erase(it);
            else
                it++;
            continue;
        }

        /// extract output file name.
        if ((*it)->type() == entry::kOut) {
            argv::OutArgv *oargv = (argv::OutArgv*)(*it).get();
            opt_file = oargv->dst();
            it = arglist.erase(it);
            continue;
        }
        it++;
    }

    ///
    /// transfer to pkg maker inner deal.
    /// package.
    pkg::Maker maker(arglist, opt_file);
    if ((ret = maker.make()) != ERROR_Success) {
        LOG(ERROR) << "make pkg failure!";
        return ret;
    }

    LOG(INFO) << "Package Over!";
    return ERROR_Success; 
}

//!
/// brief: extract pkgfile to outdir.
int extract(std::string const& pkgfile, std::string const& outdir)
{
    LOG(INFO) << "Extract Start, package name: " << pkgfile << ", to dir: " << outdir;
    
    int ret;
    shared_ptr<pkg::Extractor> extractor;
    try {
        extractor.reset(new pkg::Extractor(pkgfile, outdir));
        ret = extractor->extract();
    } catch (except_base &ex) {
        ret = ex.error();
    }

    LOG(INFO) << "Extract Over!";
    return ret; 
}

//! 
/// brief: format errorcode to string.
int formaterror(std::string const& errcode)
{
    std::istringstream iss(errcode);
    int err = -1;
    iss >> err;
    std::cout << "Format Error Code:\n\t" << getError(err) << std::endl;
    return ERROR_Success;
}

#if 0
#include <windows.h>
#include "osinfo.h"
void test()
{
    SYSTEM_INFO sysinfo;

    GetSystemInfo(&sysinfo);
    std::cout << "Current cpu arch: " << cpuArch(&sysinfo) << std::endl;
    std::cout << "Current cpu type: " << cpuType(&sysinfo) << std::endl;

    GetNativeSystemInfo(&sysinfo);
    std::cout << "Current cpu arch: " << cpuArch(&sysinfo) << std::endl;
    std::cout << "Current cpu type: " << cpuType(&sysinfo) << std::endl;

}

#endif