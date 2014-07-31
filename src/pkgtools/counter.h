//!
/// brief: counter.h define file and argv string counter.
///

#if !defined(pkgtools_counter_h_)
#define pkgtools_counter_h_

#include <string>
#include <map>
#include <vector>
#include <cclib/types.h>

namespace {

using namespace cclib;

struct counter 
{
    counter() : curr_index_(0) {}
    
    /// get 
    uint64_t index(std::string const& elem, bool case_sensitive = true)
    {
        std::string tmp_elem = case_sensitive ? elem : to_lower(elem);
        std::map<std::string, uint64_t>::iterator it;

        if ((it = elems_.find(tmp_elem)) != elems_.end())
            return it->second;

        /// not find, then add to map and add to array.
        names_[tmp_elem] = curr_index_;
        indexs_.push_back(tmp_elem);
        
        retrun curr_index_++;   
    }
    std::vector<std::string>& map() const { return indexs_; }
private:
    /// for quick find string key.
    std::map<std::string, uint64_t> elems_;
    std::vector<std::string> indexs_;
    uint64_t curr_index_;
};

}

namespace file {

using namespace cclib;

struct fcounter
{
    uint64_t index(std::string const& path) { return counter_.index(path, false); }
    std::vector<std::string>& map() { return counter_.map(); }
private:
    ::counter counter_;
};

} // namespace file;

namespace argv {

using namespace cclib;

struct acounter
{
    uint64_t index(std::string const& argv) { return counter_.index(argv); }
    std::vector<std::string>& map() { return counter_.map(); }
private:
    ::counter counter_;
};

} // namespace argv;

#endif // pkgtools_counter_h_