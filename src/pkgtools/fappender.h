//!
/// brief: fappender.h define pkg data(files) appender
/// principle: 
///         multi-fopener as all files holder is as input.
///         one-fwriter is as output.
///         copy fopener content to fwriter while you can
///         use compress algo or not.
///
/// detail:
///         fopener1   fopener2   fopener3 ...
///             \          |          /
///              \         |         /
///               \        |        /
///                \       |       /
///                  single-fwriter

#if !defined(pkgtools_fappender_h_)
#define pkgtools_fappender_h_

#include <vector>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include "fopener.h"
#include "pkgfile.h"
#include "pkgdef.h"

namespace file {

using namespace cclib;
using ::pkg::Writer;

const uint32_t kmaxbufsize = (1 << 20) * 10; /// 10M.

struct fappender
{
    fappender(Writer *writer, uint8_t compress = pkg::kcompno) 
        : writer_(writer), compress_(compress) 
    {
        buf_.resize(kmaxbufsize);
    }

    void operator()(shared_ptr<fopener> opener)
    {
        uint64_t len = opener->length();
        uint64_t writed = 0;
        uint64_t readed = 0;
        uint32_t readonce;
        
        while (readed < len) {
            
            readonce = opener->read(&buf_[0], buf_.size());
            readed += readonce;
            ///TODO:: compress..
            if (compress_ != pkg::kcompno) {
                ///TODO:: ...
                //writed += compress();;;

                continue;
            }
            /// no compress...
            writer_->write(&buf_[0], readonce);
            writed += readonce;
        }
        comp_len_list_.push_back(writed);
    }
    std::vector<uint64_t> complenlist() const { return comp_len_list_; }

private: 
    Writer *writer_;
    uint8_t compress_;
    std::vector<uint8_t> buf_;

    std::vector<uint64_t> orig_len_list_;
    std::vector<uint64_t> comp_len_list_;

    uint32_t compress(void *dst, uint32_t dlen, void const* src, uint32_t slen)
    {
        ///TODO:: not implement.
        return 0;
    }
};

} // namespace file

#endif // pkgtools_fappender_h_