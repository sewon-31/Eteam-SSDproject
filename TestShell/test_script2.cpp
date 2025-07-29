#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>

#include "test_script2.h"
#include "ssd_interface.h"

bool testScript2::run(SSDInterface &ssd)
{
    for (int i = 0; i < 30; i++) {
#if _DEBUG
        std::string num = "0x0000000A";
#else
        std::string num = getRandomIntToString();
#endif
        ssd.write(4, num);
        ssd.write(0, num);
        ssd.write(3, num);
        ssd.write(1, num);
        ssd.write(2, num);

        for (int j = 0; j < 4; j++) {
            if (readCompare(j, num, ssd) == false) { 
                std::cout << "FAIL";
                return false; 
            }
        }
    }

    std::cout << "PASS";
    return true;
}

std::string testScript2::getRandomIntToString()
{
    char buffer[11];
    std::srand(std::time(nullptr));

    std::snprintf(buffer, sizeof(buffer), "0x%08X", (std::rand() % INT_MAX + 1));
    return std::string(buffer);
}

bool testScript2::readCompare(int address, std::string num, SSDInterface& ssd)
{
    if (ssd.read(address) == num) return true;
    return false;
}
