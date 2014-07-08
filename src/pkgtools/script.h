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
/// template: file: [destpath][srcpath][-p [normal/effort [-e break/continue]]] [-i [normal/effort [-e break/continue]]] [-u [normal/effort [-e break/continue]]]
/// template: file: [destpath][srcpath][-p [n/e [-e b/c]]] [-i [n/e [-e b/c]]] [-u [n/e [-e b/c]]]
///                   index:0      index:1         index:2         index:3

/// template: dir:  [dirpath] [-i [normal/effort [-e break/continue]]] [-u [normal/effort [-e break/continue]]]
/// template: dir:  [dirpath] [-i [n/e [-e b/c]]] [-u [n/e [-e b/c]]]
///                  index:0        index:1              index:2         

/// // first: you should package tmp file.
///         file: [filepath] [-p] [] []
///      == file: [filepath] [-p n -e b]
///
/// // second: you should create dir.
///         dir:  [dirpath] [-i effort -e continue] [-u effort -e continue]
///      == dir:  [dirpath] [-i e -e c] [-u e -e c]
///
/// // third: you should package installed file.
///         file: [filepath] [-p effort -e continue] [-i effort -e continue] [-u effort -e continue]
///      == file: [filepath] [-p e -e c] [-i e -e c] [-u e -e c]
/// 
/// // fourth: you should package exec cmdline.
/// exec: [] []
/// exec: [-i -c] [-c 0 takeown "args list"]
/// exec: [-i -c] [-c 0 icacls "args list"]
/// exec: [-i -c] [-c 0 vss-copy "C:\\"]

#if !defined(pkttools_script_h_)
#define pkttools_script_h_

#include <string>
#include <vector>
#include <cclib/smartptr_def.h>

struct EntryBase;
class Script
{
public:
    Script(std::string const& script);
    int Parse();
    int Package();
private:
    std::string script_;
    std::vector<shared_ptr<EntryBase> > entrys_;
};

#endif // pkttools_script_h_