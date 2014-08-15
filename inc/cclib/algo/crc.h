//!
/// brief: define crc algoth.
///

#if !defined(cclib_algo_crc_h_)
#define cclib_algo_crc_h_

#include <stdio.h>
#include <vector>
#include "../types.h"
#include "../scopeguard.h"

CCLIB_NAMESPACE_BEGIN
ALGO_NAMESPACE_BEGIN

const uint32_t kcrc32magic = 0xedb88320;
const uint32_t kcrc32init  = 0xffffffff;
const uint32_t kmaxrdlen   = k1mega * 10;

struct crc32
{
    crc32(std::string const &file, uint32_t skip = 0)
        : value_(kcrc32init)
    {
        FILE *fp = fopen(file.c_str(), "rb");
        if (fp == NULL) {
            ready_ = false;
            return;
        }

        SCOPEEXIT(fclose, fp);

        if (fseek(fp, skip, SEEK_SET) != 0) {
            ready_ = false;
            return;
        }

        std::vector<uint8_t> buf;
        buf.resize(kmaxrdlen);
        size_t rdlen;
        while ((rdlen = fread(&buf[0], 1, buf.size(), fp)) != 0) {
            add(&buf[0], rdlen);
            if (rdlen < buf.size())
                break;
        }
        ready_ = true;
    }

    bool ready() const { return ready_; }

    crc32() : value_(kcrc32init) { }

    uint32_t value() const { return ~value_; }

    void add(void const *buf, size_t len)
    {
        uint8_t *tmp = (uint8_t *)buf;
        std::vector<uint32_t> const &tab = table();
        for (size_t i = 0; i < len; ++i) {
            value_ = tab[ tmp[i] ^ (value_ & 0xff)] ^ (value_ >> 8);
        }
    }

    void add(std::vector<uint8_t> const& buf)
    {
        add(&buf[0], buf.size());
    }

    static std::vector<uint32_t> const& table()
    {
        static std::vector<uint32_t> tab = make_table();
        return tab;
    }
private:
    uint32_t value_;
    bool ready_;

    static std::vector<uint32_t> make_table()
    {
        std::vector<uint32_t> tab;
        uint32_t c;
        for (uint32_t i = 0; i < 256; ++i) {
            c = i;
            for (uint32_t j = 0; j < 8; ++j) {
                if (c & 1)
                    c = kcrc32magic ^ (c >> 1);
                else
                    c = c >> 1;
            }
            tab.push_back(c);
        }
        return tab;
    }
};

ALGO_NAMESPACE_END
CCLIB_NAMESPACE_END

#endif // cclib_algo_crc_h_
