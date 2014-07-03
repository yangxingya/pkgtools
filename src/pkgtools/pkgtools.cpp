//!
/// pkgtools.cpp: package tools main function define.
///
/// date: 2014-07-01 19:55
/// copyright: easytek Comp.
/// author: yangxingya
/// email: xingyayang@163.com

#include <iostream>
#include <sstream>
#include <string>
#include <glog/logging.h>
#include "error.h"

void initializelog(const char *argv0);
void usage();
int install(std::string const& pkgfile);
int uninstall(std::string const& pkgfile);
int package(std::string const& sptfile);
int extract(std::string const& pkgfile, std::string const& outdir);
int formaterror(std::string const& errcode);
int main(int argc, char *argv[])
{
    //std::cout << argv[0] << std::endl;
    initializelog(argv[0]);

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

    if (std::string(argv[1]) == "-i")
        return install(argv[2]);

    if (std::string(argv[1]) == "-u")
        return install(argv[2]);

    if (std::string(argv[1]) == "-p")
        return package(argv[2]);
    
    if (std::string(argv[1]) == "-e") {
        std::string outdir = ".";
        if (argc >= 3)
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
    google::SetLogDestination(google::INFO, log_base_fn.c_str());
    // set error level log file is empty.
    google::SetLogDestination(google::ERROR, "");
    // set warn level log file is empty.
    google::SetLogDestination(google::WARNING, "");

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
        << "pkgtools [-i | -u | -p | -e | -x] [file | error] [outpath] \n\n"
        << spacepfx + alignfill("-i", spacesz)
        << "install package from file:<installed-package>.\n"
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
int install(std::string const& pkgfile)
{
    LOG(INFO) << "Install Start, package name: " << pkgfile;
    
    //TODO:: install implement.

    LOG(INFO) << "Install Over!";
    return -1;
}

//!
/// brief : uninstall a pragram installed by pkgfile.
/// notice: UNINSTALL BY pkgfile, SO YOU MUST WRITE
///         UNINSTALL INFO AT pkgfile, and script MUST
///         INCLUDE unstall section.
int uninstall(std::string const& pkgfile)
{
    LOG(INFO) << "Uninstall Start, package name: " << pkgfile;

    //TODO:: uninstall implement...

    LOG(INFO) << "Uninstall Over!";
    return -1; 
}

//! 
/// brief : make package by sptfile:<script file>.
///
int package(std::string const& sptfile)
{
    LOG(INFO) << "Package Start, script name: " << sptfile;

    //TODO:: packate implement...

    LOG(INFO) << "Package Over!";
    return -1; 
}

//!
/// brief: extract pkgfile to outdir.
int extract(std::string const& pkgfile, std::string const& outdir)
{
    LOG(INFO) << "Extract Start, package name: " << pkgfile << ", to dir: " << outdir;
    
    //TODO:: extract implement...

    LOG(INFO) << "Extract Over!";
    return -1; 
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

