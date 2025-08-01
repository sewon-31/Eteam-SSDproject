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
SSD::reduceCMDBufferFirstMerge(TEST_CMD in, TEST_CMD& out, int cmdCount) {
    int virtual_op[100];	// 9 == NULL, 7 = E, 0-5 = W 

    const int OP_NULL = 9;
    const int OP_E = 7;
    const int OP_W_MAX = 5;

    // 1. Replcae w iba "0x00000000" >  E iba
    // 2. make virtual data 
    // 3. Make CMD
        //3-1. Make new CMD E range check and W

    // step - 1
    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        if (in.op[idx_cb] == "W" && in.data[idx_cb] == "0x00000000") {
            in.op[idx_cb] = "E";
            in.size[idx_cb] = 1;
        }
    }

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
        // Check E
        if (virtual_op[idx_iba] == OP_E) {
            if (continue_E_CMD == 0) {
                out.op[newCMDCount] = "E";
                out.lba[newCMDCount] = idx_iba;
                out.size[newCMDCount] = 1;
                continue_E_CMD = 1;
            }
            else {
                out.size[newCMDCount]++;
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

        //Check W
        if (virtual_op[idx_iba] <= OP_W_MAX) {
            out.op[newCMDCount] = "W";
            out.lba[newCMDCount] = idx_iba;
            out.data[newCMDCount] = in.data[virtual_op[idx_iba]];
            out.size[newCMDCount] = 1;
            newCMDCount++;
        }
    }
    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << out.op[idx_cb] << " " << out.lba[idx_cb] << " " << out.data[idx_cb] << " " << out.size[idx_cb] << "\n";
    }
    std::cout << "\n";

    return newCMDCount;
}

int
SSD::reduceCMDBufferNonSeqCmdMerge(TEST_CMD in, TEST_CMD& out, int cmdCount) {
    // step - 3-2 Merge
    int idx_cb = 0;

    for (idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        if (idx_cb < cmdCount - 1) {
            if (in.op[idx_cb] == "E" && in.op[idx_cb + 1] == "E") {
                if (in.lba[idx_cb] + in.size[idx_cb] >= in.lba[idx_cb + 1]) {
                    if (in.lba[idx_cb + 1] >= in.lba[idx_cb + 2] - 1) {
                        //Fill left CMD (E(6)-E(5) -> E(10)-E(1))
                        if (in.lba[idx_cb] + in.size[idx_cb] >= in.lba[idx_cb + 1]) {
                            int sum_size = in.size[idx_cb] + in.size[idx_cb + 1];

                            for (int idx = idx_cb; idx < cmdCount; idx++) {
                                out.op[idx] = in.op[idx];
                                out.lba[idx] = in.lba[idx];
                                out.size[idx] = in.size[idx];
                                out.data[idx] = in.data[idx];
                            }
                            in.size[idx_cb] = std::min(sum_size, 10);
                            in.size[idx_cb + 1] = sum_size - in.size[idx_cb];

                            return cmdCount;
                        }
                    }
                }
            }
        }
        if (idx_cb < cmdCount - 2) {
            if (in.op[idx_cb] == "E" && in.op[idx_cb + 1] == "W" && in.op[idx_cb + 2] == "E") {
                if (in.lba[idx_cb] + in.size[idx_cb] >= in.lba[idx_cb + 1]) {
                    if (in.lba[idx_cb + 1] = in.lba[idx_cb + 2] - 1) {
                        int sum_size = in.size[idx_cb] + in.size[idx_cb + 2] + 1;
                        //Merge check (E-W-E -> E-W)
                        if (in.size[idx_cb] + in.size[idx_cb + 2] + 1 <= 10) {
                            out.op[idx_cb] = in.op[idx_cb];
                            out.lba[idx_cb] = in.lba[idx_cb];
                            out.size[idx_cb] = sum_size;
                            out.data[idx_cb] = in.data[idx_cb];
                            out.op[idx_cb + 1] = in.op[idx_cb + 1];
                            out.lba[idx_cb + 1] = in.lba[idx_cb + 1];
                            out.size[idx_cb + 1] = in.size[idx_cb + 1];
                            out.data[idx_cb + 1] = in.data[idx_cb + 1];

                            int newCMDCount = cmdCount - 1;
                            for (int idx = idx_cb + 2; idx < newCMDCount; idx++) {
                                out.op[idx] = in.op[idx + 1];
                                out.lba[idx] = in.lba[idx + 1];
                                out.size[idx] = in.size[idx + 1];
                                out.data[idx] = in.data[idx + 1];
                            }
                            return newCMDCount;
                        }
                        //Fill left CMD (E(5)-W(1)-E(5) -> E(10)-W(1)-E(5))
                        for (int idx = idx_cb; idx < cmdCount; idx++) {
                            out.op[idx] = in.op[idx];
                            out.lba[idx] = in.lba[idx];
                            out.size[idx] = in.size[idx];
                            out.data[idx] = in.data[idx];
                        }

                        out.size[idx_cb] = std::min(sum_size, 10);
                        out.size[idx_cb + 2] = sum_size - out.size[idx_cb];
                        out.lba[idx_cb + 2] = out.lba[idx_cb] + out.size[idx_cb];
                        return cmdCount;
                    }
                }
            }
        }
        int hit_EWNE = 0;
        //Merge check (E-WxN-E -> E-WxN)
        for (int pos = 3; pos < (cmdCount - idx_cb); pos++) {
            if (in.size[idx_cb] + pos > 10) break;

            if (in.op[idx_cb + pos] == "E") {
                if (in.size[idx_cb] + pos + in.size[idx_cb + pos] <= 10) {

                }
            }
        }

        out.op[idx_cb] = in.op[idx_cb];
        out.lba[idx_cb] = in.lba[idx_cb];
        out.size[idx_cb] = in.size[idx_cb];
        out.data[idx_cb] = in.data[idx_cb];
    }

    // display
    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        std::cout << out.op[idx_cb] << " " << out.lba[idx_cb] << " " << out.data[idx_cb] << " " << out.size[idx_cb] << "\n";
    }

    return cmdCount;
}

int
SSD::reduceCMDBuffer(TEST_CMD in, TEST_CMD& out) {
#if 0
    return reduceCMDBufferSeqCMD(in, out);
#else
    TEST_CMD step3_1;

    int CMDCount = reduceCMDBufferFirstMerge(in, step3_1, 6);
    int newCMDCount = reduceCMDBufferNonSeqCmdMerge(step3_1, out, CMDCount);

    while (newCMDCount != CMDCount) {
        CMDCount = newCMDCount;
        newCMDCount = reduceCMDBufferNonSeqCmdMerge(step3_1, out, CMDCount);
    }
    
    return newCMDCount;
#endif
   
}