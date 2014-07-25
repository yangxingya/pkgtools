//!
/// brief: test.
///

#include "scriptcreator.h"

int main()
{
    std::string script = "creator.script";

    script::creator creator(script);

    // add disable fs redirection setting
    std::string setting = script::creator::mksettingline("dfsrd");
    creator.add(setting);


    // add out line.
    std::string out = script::creator::mkoutline("creator.dat");
    creator.add(out);

    // add dir line.
    std::string dir = script::creator::mkoutline("c:\\pkg-install");
    creator.add(dir);

    std::string file = script::creator::mkfileline(
        "c:\\pkg-install\\d3d9.dll", "c:\\windows\\system32\\d3d9.dll");
    creator.add(file);

    creator.close();
    

}