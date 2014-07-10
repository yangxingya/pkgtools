//
//

#include "entry.h"
#include "except.h"
#include <assert.h>
#include <glog/logging.h>
#include "argv.h"

/// package as parsing script used.
FileEntry::FileEntry(argv::AutoArgv argv)
{   
    argv::FileArgv *fargv = (argv::FileArgv*)(argv.get());
    assert(fargv != NULL);
    LOG(INFO) << "File Entry: ";
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

int FileEntry::makeEntry(pkg::entry_t const* entry)
{
    //TODO:: not implement.
    throw no_impl();
}