#include "ssd.h"
#include "file_interface.h"
#include "command_buffer.h"

#include <algorithm>
#include <iostream>

SSD::SSD(const std::string& nandPath, const std::string& outputPath)
    : outputFile(outputPath),
    storage(NandData::getInstance()),
    cmdBuf(CommandBuffer::getInstance())
{
}

void
SSD::run(vector<string> commandVector)
{
    if (!builder) {
        builder = std::make_shared<SSDCommandBuilder>();
    }

    string result("");

    // create command (validity check included)
    auto cmd = builder->createCommand(commandVector);
    if (cmd == nullptr) {
        updateOutputFile("ERROR");
        return;
    }

    cmdBuf.Init();

    auto type = cmd->getCmdType();
    if (type == CmdType::READ || type == CmdType::FLUSH)
    {
        cmd->run(result);
    }
    else if (type == CmdType::WRITE || type == CmdType::ERASE) {
        cmdBuf.addCommand(cmd);
    }

    cmdBuf.updateToDirectory();

    if (!result.empty()) {
        updateOutputFile(result);
    }
}

bool
SSD::updateOutputFile(const string& result)
{
    outputFile.fileClear();
    outputFile.fileOpen();

    bool ret = outputFile.fileWriteOneline(result);

    outputFile.fileClose();
    return ret;
}

string
SSD::getData(int lba) const
{
    return storage.read(lba);
}

void
SSD::writeData(int lba, const string& value)
{
    storage.write(lba, value);
}

void
SSD::clearData()
{
    storage.clear();
}

void
SSD::setBuilder(std::shared_ptr<SSDCommandBuilder> builder)
{
    this->builder = builder;
}

NandData&
SSD::getStorage()
{
    return storage;
}

void
SSD::clearBufferAndDirectory()
{
    cmdBuf.clearBuffer();
    cmdBuf.updateToDirectory();
}

void
SSD::clearBuffer()
{
    cmdBuf.clearBuffer();
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
                if (virtual_op[idx_iba] == OP_E)
                    temp.op[newCMDCount] = "E";
                else {
                    temp.op[newCMDCount] = "W";
                    temp.data[newCMDCount] = in.data[virtual_op[idx_iba]];
                }
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
                        //temp.op[idx_cb_out] = "W";
                        //temp.lba[idx_cb_out] = in.lba[idx_cb_in];
                        //temp.size[idx_cb_out] = 1;
                        //temp.data[idx_cb_out] = in.data[idx_cb_in];
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