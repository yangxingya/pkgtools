//!
/// brief: entry.h define every script line execute cmd
///

#if !defined(pkgtools_entry_h_)
#define pkgtools_entry_h_

#include <string>
#include "pkgdef.h"

struct EntryBase 
{
    virtual int Install() = 0;
    virtual int Uninst () = 0;
    // HACK... remove package function.
    virtual int Package() = 0;
    virtual int getEntry(pkg::entry_t const* entry) = 0;
    virtual ~EntryBase() {}
};

struct FileEntry : public EntryBase
{
    /// package as parsing script used.
    FileEntry(std::string const& sriptline, bool& valid);
    /// for install or uninstall.
    FileEntry(pkg::entry_t const& entry);
    ///
    virtual int Install();
    virtual int Uninst();
    virtual int Package();
    virtual int getEntry(pkg::entry_t const* entry);
    virtual ~FileEntry() {}
};

#endif // pkgtools_entry_h_