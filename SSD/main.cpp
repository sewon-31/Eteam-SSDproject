#include "gmock/gmock.h"
#include "ssd.h"

#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::vector;

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
    vector<string> commandVector(argv + 1, argv + argc);

    SSD app;
    app.run(commandVector); // run을 vector<string> 받도록 바꿔야 함
#endif
}