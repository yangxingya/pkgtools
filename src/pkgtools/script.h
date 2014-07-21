//!
/// brief: script.h is for script parse
///
/// usage: script form:
///
/// myscript.txt

///   [-p/i/u [n/t [e/i]]]
/// details:
///     -p          package. 
///     -i          install.
///     -u          uninstall.
///     n           if error happened, not try. 
///     t           if error happened, change solution and try again. it is DEFAULT OPTION!
///     e           exit program. 
///     i           ignore error, continue next. it is DEFAULT OPTION!
/// 
/// template: file: [-d destpath][-s srcpath][-p [n/t [e/i]]] [-i [n/t [e/i]]] [-u [n/t [e/i]]]
/// 

/// template: dir:  [-d dirpath] [-i [n/t [e/i]]] [-u [n/t [e/i]]]
///      

/// // first: you should package tmp file.
///         file: [-d tmppath] [-s filepath] [-p] [-i]
///      == file: [-d tmppath] [-s filepath] [-p t i] [-i t i]
///
/// // second: you should create dir.
///         dir:  [-d dirpath] [-i t i] [-u t i]
///
/// // third: you should package installed file.
///         file: [-d filepath] [-s filepath] [-p t i] [-i t i] [-u t i]
/// 
/// // fourth: you should package exec cmdline.
///         exec: [-p n/t b/i] [-i n/t b/i] [-e c 0] [-c args list]
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