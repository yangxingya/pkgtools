//!
/// brief: test.
///

#include "scriptcreator.h"

int main()
{
    std::string script = "creator.script";

    script::creator creator(script);

    // add disable fs redirection setting
    std::string setting = script::creator::settingline("dfsrd", true, true, true);
    creator.add(setting);


    // add out line.
    std::string out = script::creator::outline("creator.dat");
    creator.add(out);

    // add dir line.
    std::string dir = script::creator::mkdirline("c:\\pkg-install");
    script::creator::pkgtattr(dir, true, true);
    creator.add(dir);

    std::string file = script::creator::addfline(
        "c:\\pkg-install\\d3d9.dll", "c:\\windows\\system32\\d3d9.dll");
    script::creator::pkgtattr(file, true, true);
    script::creator::instattr(file, true, true);
    creator.add(file);

    std::string delfile = script::creator::delfline("c:\\pkg-install\\d3d9.dll", false, true);
    script::creator::uninattr(delfile, true, true);
    creator.add(delfile);

    std::string rmd = script::creator::rmdirline("c:\\pkg-install", false, true);
    creator.add(rmd);

    creator.close();
}