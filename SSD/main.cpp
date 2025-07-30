#include "gmock/gmock.h"
#include "ssd.h"

#include <string>
#include <sstream>
#include <iostream>

using std::string;

int main(int argc, char* argv[])
{
#if _DEBUG
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
#else
    std::ostringstream oss;

    for (int i = 1; i < argc; ++i) {
        oss << argv[i];
        if (i != argc - 1) oss << " ";
    }

    SSD app;
    app.run(oss.str());
#endif
}