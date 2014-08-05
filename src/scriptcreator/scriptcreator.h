//!
/// brief: script creator is for create pkgtools package script.
///

#if !defined(script_creator_h_)
#define script_creator_h_

#include <stdio.h>
#include <string>
#include <sstream>
#include <cclib/strutil.h>

namespace script {

const std::string k1stsep = "[";
const std::string k2ndsep = "]";

const std::string kpkgtpfx = "-p";
const std::string kinstpfx = "-i";
const std::string kuninpfx = "-u";

const std::string knotrystr  = "n";
const std::string ktrystr    = "t";
const std::string kbreakstr  = "b";
const std::string kignorestr = "i";

const std::string kspace = " ";

#pragma warning(push)
#pragma warning(disable: 4996) // for disable fopen function warning

struct creator {
    creator(std::string const& file)
        : fp_(fopen(file.c_str(), "w+"))
        , ready_(fp_ != NULL)
        , closed_(false)
    {}

    ~creator() { close(); }

    bool ready() const { return ready_; }

    void close() 
    { 
        if (!closed_) { 
            if (fp_) {
                fflush(fp_); 
                fclose(fp_); 
            }
            closed_ = true; 
        } 
    }

    //!
    /// brief: add out script line.
    bool out(std::string const& dst)
    {
        std::string line = outline(dst); 
        return add(line);
    }

    //!
    /// brief: add file script line.
    bool addf(std::string const& dst, std::string const& src, 
        bool insted = true, bool uninsted = false)
    {
        std::string line = addfline(dst, src, insted, uninsted);
        return add(line);
    }

    bool delf(std::string const& dst, bool insted = false, bool uninsted = true)
    {
        std::string line = delfline(dst, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: make dir script line.
    bool mkdir(std::string const& dst, 
        bool insted = true, bool uninsted = false)
    {
        std::string line = mkdirline(dst, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: rm dir script line.
    bool rmdir(std::string const& dst, 
        bool insted = false, bool uninsted = true)
    {
        std::string line = rmdirline(dst, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: add exec script line.
    bool adde(std::string const& cmd, 
        bool checkret = false, int ret = 0,
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = execline(cmd, checkret, ret, pkged, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: add setting script line.
    bool adds(std::string const& setting, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = settingline(setting, pkged, insted, uninsted); 
        return add(line);
    }

    static std::string outline(std::string const& dst)
    {
        std::string line = "out:";
        
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make an file line.
    static std::string addfline(std::string const& dst, std::string const& src, 
        bool insted = true, bool uninsted = false)
    {
        std::string line = "addf:";
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// [-s src]
        line += "[-s ";
        line += src;
        line += "]";

        /// common constructor.
        comm_ctor(line, true, insted, uninsted);
        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make an file line.
    static std::string delfline(std::string const& dst,  
        bool insted = false, bool uninsted = true)
    {
        std::string line = "delf:";
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// common constructor.
        comm_ctor(line, false, insted, uninsted);
        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make a dir line.
    static std::string mkdirline(std::string const& dst, 
        bool insted = true, bool uninsted = false)
    {
        std::string line = "mkdir:";
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// common constructor.
        comm_ctor(line, false, insted, uninsted);

        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make a dir line.
    static std::string rmdirline(std::string const& dst, 
        bool insted = false, bool uninsted = true)
    {
        std::string line = "rmdir:";
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// common constructor.
        comm_ctor(line, false, insted, uninsted);

        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make an exec line.
    static std::string execline(std::string const& cmd, 
        bool checkret = false, int ret = 0,
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = "exec:";
        /// [-e c 0]
        if (checkret) {
            line += "[-e c ";

            /// get ret string.
            std::stringstream ss;
            ss << ret;
            std::string retstr;
            ss >> retstr;

            line += retstr;
            line += "]";
        }

        /// [-c cmd]
        line += "[-c ";
        line += cmd;
        line += "]";

        /// common constructor.
        comm_ctor(line, pkged, insted, uninsted);

        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make a setting line.
    static std::string settingline(std::string const& setting, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = "setting:";
        /// [-s setting]
        line += "[-s ";
        line += setting;
        line += "]";

        /// common constructor.
        comm_ctor(line, pkged, insted, uninsted);
        
        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: add one line to script file
    bool add(std::string const& line)
    {
        if (!ready_) return false;

        return fwrite(line.c_str(), 1, line.length(), fp_) == line.length(); 
    }

    //!
    /// brief: package attribute modify.
    static bool pkgtattr(std::string &line, bool errtry = true, bool ignorerr = true)
    {
        return commattr(line, kpkgtpfx, errtry, ignorerr);
    }

    //!
    /// brief: install attribute modify.
    static bool instattr(std::string &line, bool errtry = true, bool ignorerr = true)
    {
        return commattr(line, kinstpfx, errtry, ignorerr);
    }

    //!
    /// brief: uninstall attribute modify.
    static bool uninattr(std::string &line, bool errtry = true, bool ignorerr = true)
    {
        return commattr(line, kuninpfx, errtry, ignorerr);
    }

private:
    FILE *fp_;
    bool ready_;
    bool closed_;

    static void comm_ctor(std::string &line, bool pkged, bool insted, bool uninsted)
    {
        /// pkged.
        if (pkged) line += "[-p]";

        /// insted.
        if (insted) line += "[-i]";

        /// uninsted.
        if (uninsted) line += "[-u]";
    }

    static bool commattr(std::string &line, std::string const& comm_pfx, bool errtry, bool ignorerr)
    {
        std::vector<std::string> elems;
        cclib::split(line, k1stsep, k2ndsep, &elems, true);
       
        bool find = false;
        size_t i;

        for (i = 0; i < elems.size(); ++i) {
            if (cclib::start_with(elems[i], comm_pfx)) {
                find = true;
                break;
            } 
        }

        if (find) {
            std::string from = k1stsep + elems[i] + k2ndsep;
            std::string to = k1stsep;
            to += comm_pfx;
            to += kspace;
            to += (errtry ? ktrystr : knotrystr);
            to += kspace;
            to += (ignorerr ? kignorestr: kbreakstr);
            to += k2ndsep;

            /// replace [-p/i/u xxx] -> [-p/i/u yyy] format.
            cclib::replace(line, from, to); 
        }

        return find;
    }
};

#pragma warning(pop)

} // namespace script

#endif // script_creator_h_