//!
/// brief: filechecker.h define file package checker .
///

#if !defined(pkgtools_fcounter_h_)
#define pkgtools_fcounter_h_

#include <string>
#include <map>
#include <cclib/types.h>

namespace file {

using namespace cclib;

struct fcounter 
{
    fcounter() : curr_index_(0) {}
    
    /// get 
    uint64_t index(std::string const& name)
    {
        std::string name_lower = to_lower(name);
        std::map<std::string, uint64_t>::iterator it;

        if ((it = names_.find(name_lower)) != names_.end())
            return it->second;

        /// not find, then add to map.
        uint64_t ret = curr_index_;
        names_[name_lower] = curr_index_++;
        retrun curr_index_;   
    }
private:
    std::map<std::string, uint64_t> names_;
    uint64_t curr_index_;
};

} // namespace file;

#endif // pkgtools_fcounter_h_