#include "ssd.h"
#include "file_interface.h"
#include <algorithm>
#include <iostream>

SSD::SSD(const std::string& nandPath, const std::string& outputPath)
    : nandFile(nandPath), outputFile(outputPath) {
}

void
SSD::setParser(SSDCommandParser* parser)
{
    this->parser = parser;
}

void
SSD::run(const string& commandStr)
{
    if (parser == nullptr) {
        SSDCommandParser myParser;
        parser = &myParser;
    }

    // parse command
    parser->setCommand(commandStr);
    if (!parser->isValidCommand()) {
        writeOutputFile("ERROR");
        return;
    }
    parsedCommand = parser->getCommandVector();

    clearData();
    readNandFile();

    // run command
    string operation = parsedCommand.at(SSDCommandParser::Index::OP);
    int lba = std::stoi(parsedCommand.at(SSDCommandParser::Index::LBA));

    if (operation == "R") {
        //std::cout << "Read" << lba << std::endl;
        string result = runReadCommand(lba);
        writeOutputFile(result);
    }
    else if (operation == "W") {
        //std::cout << "Write" << lba << std::endl;
        runWriteCommand(lba, parsedCommand.at(SSDCommandParser::Index::VAL));
        writeNandFile();
    }
}

string
SSD::runReadCommand(int lba)
{
    return data[lba];
}

void
SSD::clearData()
{
    std::fill(std::begin(data), std::end(data), "0x00000000");
}

void
SSD::runWriteCommand(int lba, const string& value)
{
    data[lba] = value;
}

string
SSD::getData(int lba) const
{
    return data[lba];
}

FileInterface&
SSD::getNandFile() {
    return nandFile;
}

FileInterface&
SSD::getOutputFile() {
    return outputFile;
}

bool
SSD::readNandFile() {
    bool ret;

    nandFile.fileOpen();

    if (nandFile.checkSize() != nandFileSize)  return false;

    for (int i = 0; i < maxLbaNum; i++)
    {
        ret = nandFile.fileReadOneline(data[i]);

        if (!ret)  break;
    }
    nandFile.fileClose();
    return ret;
}

bool
SSD::writeNandFile() {
    bool ret;

    nandFile.fileClear();
    nandFile.fileOpen();

    for (int i = 0; i < maxLbaNum; i++)
    {
        ret = nandFile.fileWriteOneline(data[i]);

        if (!ret)
            break;
    }
    nandFile.fileClose();
    return ret;
}

bool
SSD::writeOutputFile(const string& str) {
    bool ret;

    outputFile.fileClear();
    outputFile.fileOpen();
    ret = outputFile.fileWriteOneline(str);
    outputFile.fileClose();
    return ret;
}

int
SSD::reduceCMDBufferDisplay(TEST_CMD in) {
    // display
    for (int idx_cb = 0; idx_cb < 6; idx_cb++) {
        std::cout << in.op[idx_cb] << " " << in.lba[idx_cb] << " " << in.data[idx_cb] << " " << in.size[idx_cb] << "\n";
    }
    return 0;
}

int
SSD::reduceCMDBufferMerge(TEST_CMD in, TEST_CMD& out, int cmdCount) {
    int virtual_op[100];	// 9 == NULL, 7 = E, 0-5 = W 

    const int OP_NULL = 9;
    const int OP_E = 7;
    const int OP_W_MAX = 5;

    TEST_CMD temp;
    // 1. Replcae w iba "0x00000000" >  E iba
    // 2. make virtual data 
    // 3. Make CMD
        //3-1. Make new CMD E range check and W
        // display
    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        std::cout << in.op[idx_cb] << " " << in.lba[idx_cb] << " " << in.data[idx_cb] << " " << in.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#if 0
    // step - 1
    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        if (in.op[idx_cb] == "W" && in.data[idx_cb] == "0x00000000") {
            in.op[idx_cb] = "E";
            in.size[idx_cb] = 1;
        }
    }
#endif
    // step - 2
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        virtual_op[idx_iba] = OP_NULL;
    }

    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        if (in.op[idx_cb] == "W") {
            virtual_op[in.lba[idx_cb]] = idx_cb;
        }
        else if (in.op[idx_cb] == "E") {
            for (int idx_size = 0; idx_size < in.size[idx_cb]; idx_size++)
                virtual_op[in.lba[idx_cb] + idx_size] = OP_E;
        }
    }
    // display
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        if (virtual_op[idx_iba] == OP_NULL)
            std::cout << "N" << " ";
        else if (virtual_op[idx_iba] == OP_E)
            std::cout << "E" << " ";
        else
            std::cout << "W" << " ";


        if (idx_iba % 10 == 9)
            std::cout << "\n";
    }

    // step - 3
    int continue_E_CMD = 0;
    int newCMDCount = 0;

    // step - 3-1
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        // Check E and W
        if (virtual_op[idx_iba] != OP_NULL) {
            if (continue_E_CMD == 0) {
                temp.op[newCMDCount] = "E";
                temp.lba[newCMDCount] = idx_iba;
                temp.size[newCMDCount] = 1;
                //temp.data[newCMDCount] = in.data[virtual_op[idx_iba]];
                continue_E_CMD = 1;
            }
            else {
                temp.size[newCMDCount]++;
                continue_E_CMD++;
                if (continue_E_CMD == 10) {
                    continue_E_CMD = 0;
                    newCMDCount++;
                }
            }
            continue;
        }
        else if (continue_E_CMD > 0) {
            continue_E_CMD = 0;
            newCMDCount++;
        }
    }

    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << temp.op[idx_cb] << " " << temp.lba[idx_cb] << " " << temp.data[idx_cb] << " " << temp.size[idx_cb] << "\n";
    }
    std::cout << "\n";

#if 0
    //Check W
    if (virtual_op[idx_iba] <= OP_W_MAX) {
        out.op[newCMDCount] = "W";
        out.lba[newCMDCount] = idx_iba;
        out.data[newCMDCount] = in.data[virtual_op[idx_iba]];
        out.size[newCMDCount] = 1;
        newCMDCount++;
    }
#endif
    int hit = 0;
    for (int idx_cb_in = 0; idx_cb_in < cmdCount; idx_cb_in++) {
        if (in.op[idx_cb_in] == "W" && in.data[idx_cb_in] != "0x00000000") {
            hit = 0;
            for (int idx_cb_out = 0; idx_cb_out < newCMDCount; idx_cb_out++) {
                if (temp.lba[idx_cb_out] == in.lba[idx_cb_in]) {
                    if (temp.size[idx_cb_out] == 1) {
                        temp.op[idx_cb_out] = "W";
                        temp.lba[idx_cb_out] = in.lba[idx_cb_in];
                        temp.size[idx_cb_out] = 1;
                        temp.data[idx_cb_out] = in.data[idx_cb_in];
                        hit = 1;
                    }
                }
            }
            if (!hit) {
                temp.op[newCMDCount] = "W";
                temp.lba[newCMDCount] = in.lba[idx_cb_in];
                temp.size[newCMDCount] = 1;
                temp.data[newCMDCount] = in.data[idx_cb_in];
                newCMDCount++;
            }
        }
    }

    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << temp.op[idx_cb] << " " << temp.lba[idx_cb] << " " << temp.data[idx_cb] << " " << temp.size[idx_cb] << "\n";
    }
    std::cout << "\n";

    int idx = 0;
    for (int idx_iba = 0; idx_iba < newCMDCount; idx_iba++) {
        if (temp.op[idx_iba] == "E")
        {
            out.op[idx] = temp.op[idx_iba];
            out.lba[idx] = temp.lba[idx_iba];
            out.size[idx] = temp.size[idx_iba];
            out.data[idx] = temp.data[idx_iba];
            idx++;
        }
    }
    for (int idx_iba = 0; idx_iba < newCMDCount; idx_iba++) {
        if (temp.op[idx_iba] == "W")
        {
            out.op[idx] = temp.op[idx_iba];
            out.lba[idx] = temp.lba[idx_iba];
            out.size[idx] = temp.size[idx_iba];
            out.data[idx] = temp.data[idx_iba];
            idx++;
        }
    }

    if (idx != newCMDCount)
        std::cout << "Error idx " << idx << " " << newCMDCount << "\n";

    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << out.op[idx_cb] << " " << out.lba[idx_cb] << " " << out.data[idx_cb] << " " << out.size[idx_cb] << "\n";
    }
    std::cout << "\n";

    return newCMDCount;
}

int
SSD::reduceCMDBuffer(TEST_CMD in, TEST_CMD& out) {
    int newCMDCount = reduceCMDBufferMerge(in, out, 6);

    return newCMDCount;

}