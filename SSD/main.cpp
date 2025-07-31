#include "gmock/gmock.h"
#include "ssd.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

int main(int argc, char* argv[])
{
#if _DEBUG
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
#else
    SSD app;

    vector<string> commandVector(argv + 1, argv + argc);
    app.run(commandVector);
#endif
}