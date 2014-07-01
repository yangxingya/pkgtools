//!
/// pkgtools.cpp: package tools main function define.
///
/// date: 2014-07-01 19:55
/// copyright: easytek Comp.
/// author: yangxingya
/// email: xingyayang@163.com

#include <iostream>
#include "error.h"

void usage();

int main(int argc, char *argv[])
{
    if (argc < 3) {
        usage();
        return ERROR_UsageInvalid; 
    }

    //if (argv[1]

    return ERROR_Success;
}


void usage()
{
    std::cout << "Usage: \n"
        << "  pkgtools -i pkgpath                   \n"
        << "    install package from pkgpath.       \n"
        << "  pkgtools -p pkgscript                 \n"
        << "    make install-package by pkgscript.  \n" 
        << "  pkgtools -e pkgpath outpath           \n"
        << "    extra install-package to outpath.   \n"
        << "  pkgtools -x ERRORCODE                 \n"
        << "    format pkgtools return code.        \n"
        << std::endl;
}

