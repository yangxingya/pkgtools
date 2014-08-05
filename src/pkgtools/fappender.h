//!
/// brief: fappender.h define pkg data(files) appender
/// principle: 
///         multi-fholder as all files holder is as input.
///         one-fwrapper is as output.
///         copy fholder content to fwrapper while you can
///         use compress algo or not.
///
/// detail:
///         fholder1   fholder2   fholder3 ...
///             \          |          /
///              \         |         /
///               \        |        /
///                \       |       /
///                  single-fwrapper

#if !defined(pkgtools_fappender_h_)
#define pkgtools_fappender_h_

#include <vector>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>
#include "fchecker.h"
#include "pkgfile.h"
#include "pkgdef.h"

namespace file {

using namespace cclib;
using ::pkg::Writer;

struct fappender
{
    fappender(Writer *writer, std::vector<uint64_t> &complenlist, uint8_t compress = pkg::kcompno) 
        : writer_(writer), comp_len_list_(complenlist), compress_(compress) 
    {
        buf_.resize(k1mega * 10);
    }

    void operator()(shared_ptr<fholder> holder)
    {
        uint64_t len = holder->length();
        uint64_t writed = 0;
        uint64_t readed = 0;
        uint32_t readonce;
        
        while (readed < len) {
            
            readonce = holder->read(&buf_[0], buf_.size());
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

private: 
    Writer *writer_;
    uint8_t compress_;
    std::vector<uint8_t> buf_;

    std::vector<uint64_t> orig_len_list_;
    std::vector<uint64_t> &comp_len_list_;

    uint32_t compress(void *dst, uint32_t dlen, void const* src, uint32_t slen)
    {
        ///TODO:: not implement.
        return 0;
    }
};

} // namespace file

#endif // pkgtools_fappender_h_