//!
/// brief: packer.h define pack installed package api.
///

#if !defined(pkgtools_packer_h_)
#define pkgtools_packer_h_

#include <string>


namespace pkg {


struct packer 
{
    packer(std::string const& file, AutoArgvList const& arglist)
        : writer_(file), arglist_(arglist)
    {}
    
    
};

} // namespace pkg

#endif // pkgtools_packer_h_