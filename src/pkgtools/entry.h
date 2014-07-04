//!
/// brief: entry.h define every script line execute cmd
///

#if !defined(pkgtools_entry_h_)
#define pkgtools_entry_h_

struct EntryBase 
{
    virtual int Execute(BaseArgs const* argv) = 0;
};

class FileEntry : public EntryBase
{
    
};

#endif // pkgtools_entry_h_