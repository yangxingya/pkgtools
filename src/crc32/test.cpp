
#include <iostream>
#include <cclib/algo/crc.h>

int main()
{
    cclib::algo::crc32 crcfile("libglog.dll");

    if (!crcfile.ready()) {
        std::cout << "ready failed, will exit!" << std::endl;
        return -1;
    }

    std::cout << "crc32 valud is: 0x" << std::hex << crcfile.value() << std::endl;
    return 0;
}

