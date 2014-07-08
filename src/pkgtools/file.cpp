//
//

#include "entry.h"
#include "except.h"
#include <glog/logging.h>

/// package as parsing script used.
FileEntry::FileEntry(std::string const& sriptline, bool& valid)
{
    LOG(INFO) << "File Entry: " << sriptline;
}

/// for install or uninstall.
FileEntry::FileEntry(pkg::entry_t const& entry)
{

}
///
int FileEntry::Install()
{
    //TODO:: not implement.
    throw no_impl();
}

int FileEntry::Uninst()
{
    //TODO:: not implement.
    throw no_impl();
}

int FileEntry::Package()
{
    //TODO:: not implement.
    throw no_impl();
}

int FileEntry::getEntry(pkg::entry_t const* entry)
{
    //TODO:: not implement.
    throw no_impl();
}