//!
/// brief: script creator is for create pkgtools package script.
///

#if !defined(script_creator_h_)
#define script_creator_h_

#include <stdio.h>
#include <string>
#include <sstream>

namespace script {

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
    bool addout(std::string const& dst)
    {
        std::string line = mkoutline(dst); 
        return add(line);
    }

    //!
    /// brief: add file script line.
    bool addfile(std::string const& dst, std::string const& src, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = mkfileline(dst, src, pkged, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: add dir script line.
    bool adddir(std::string const& dst, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = mkdirline(dst, pkged, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: add exec script line.
    bool addexec(std::string const& cmd, 
        bool checkret = false, int ret = 0,
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = mkexecline(cmd, checkret, ret, pkged, insted, uninsted);
        return add(line);
    }

    //!
    /// brief: add setting script line.
    bool addsetting(std::string const& setting, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = mksettingline(setting, pkged, insted, uninsted); 
        return add(line);
    }

    static std::string mkoutline(std::string const& dst)
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
    static std::string mkfileline(std::string const& dst, std::string const& src, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = "file:";
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// [-s src]
        line += "[-s ";
        line += src;
        line += "]";

        /// common constructor.
        comm_ctor(line, pkged, insted, uninsted);
        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make a dir line.
    static std::string mkdirline(std::string const& dst, 
        bool pkged = true, bool insted = true, bool uninsted = false)
    {
        std::string line = "dir:";
        /// [-d dst]
        line += "[-d ";
        line += dst;
        line += "]";

        /// common constructor.
        comm_ctor(line, pkged, insted, uninsted);

        /// new line
        line += "\n";

        return line;
    }

    //!
    /// brief: make an exec line.
    static std::string mkexecline(std::string const& cmd, 
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
    static std::string mksettingline(std::string const& setting, 
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
};

#pragma warning(pop)

} // namespace script

#endif // script_creator_h_