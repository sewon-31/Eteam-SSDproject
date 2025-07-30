#include <string>
#include <vector>
#include "file_interface.h"

using std::string;
using std::vector;

class SSD
{
public:
    void run(const string& commandStr);
    /*

parser �����ؼ� �� �ؾ� �ϴ� �� �ľ�
nand_txt ������ �� �о -> data ������ ����
read/write ����*/

    void readSSDfile(FileInterface& fileInterface);
    string runReadCommand(FileInterface& fileInterface, int lba);
    void runWriteCommand(FileInterface& fileInterface, int lab, const string& value);

private:
    string data[100];
    vector<string> parsedCommand;

    // 0. ssd
    // 1. R/W
    // 2. lba
    // 3. value
};