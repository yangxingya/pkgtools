//!
/// brief: script.h is for script parse
///
/// usage: script form:
///
/// myscript.txt

///   [-p/i/u normal/effort[b/e] -e break/continue[b/c]]
/// details:
///     -p          package. 
///     -i          install.
///     -u          uninstall.
///     normal/n    if error happened, not try. it is DEFAULT OPTION!
///     effort/e    if error happened, change solution and try again.
///     -e          error happened, if effort it indicate try max effort but error also.
///     break/b     exit program. it is DEFAULT OPTION!
///     continue/c  ignore error, continue next.
/// 
/// template: file: [-d destpath][-s srcpath][-p [normal/effort [-e break/continue]]] [-i [normal/effort [-e break/continue]]] [-u [normal/effort [-e break/continue]]]
/// template: file: [-d destpath][-s srcpath][-p [n/e [-e b/c]]] [-i [n/e [-e b/c]]] [-u [n/e [-e b/c]]]
///                   index:0      index:1         index:2         index:3

/// template: dir:  [-d dirpath] [-i [normal/effort [-e break/continue]]] [-u [normal/effort [-e break/continue]]]
/// template: dir:  [-d dirpath] [-i [n/e [-e b/c]]] [-u [n/e [-e b/c]]]
///                  index:0        index:1              index:2         

/// // first: you should package tmp file.
///         file: [-d filepath] [-s filepath] [-p] [-t]
///      == file: [-d filepath] [-s filepath] [-p n -e b] [-t n -e b]
///
/// // second: you should create dir.
///         dir:  [-d dirpath] [-i effort -e continue] [-u effort -e continue]
///      == dir:  [-d dirpath] [-i e -e c] [-u e -e c]
///
/// // third: you should package installed file.
///         file: [-d filepath] [-s filepath] [-p effort -e continue] [-i effort -e continue] [-u effort -e continue]
///      == file: [-d filepath] [-s filepath] [-p e -e c] [-i e -e c] [-u e -e c]
/// 
/// // fourth: you should package exec cmdline.
/// exec: [-p n/t b/i] [-i n/t b/i] [-e c 0] [-c args list]
///
/// 

#if !defined(pkttools_script_h_)
#define pkttools_script_h_

#include <string>
#include "argvdef.h"

class Script
{
public:
    Script(std::string const& script);
    int Parse();
    argv::AutoArgvList ArgList() const { return args_; }
private:
    std::string script_;
    argv::AutoArgvList args_;
};

#endif // pkttools_script_h_