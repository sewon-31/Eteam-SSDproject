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
#if _DEBUG
#define PRINT_DEBUG_SSD_CMDB 1
#endif
int
SSD::reduceCMDBufferMerge(TEST_CMD in, TEST_CMD& out, int cmdCount) {
    int virtual_op[100];	// 9 == NULL, 7 = E, 0-5 = W 

    const int OP_NULL = 9;
    const int OP_E = 7;
    const int OP_W_MAX = 5;

    TEST_CMD ERS_CMD;
    TEST_CMD WR_CMD;
    // 1. Replcae w iba "0x00000000" >  E iba
    // 2. make virtual data 
    // 3. Make CMD
        //3-1. Make new CMD E range check and W

#ifdef PRINT_DEBUG_SSD_CMDB
    std::cout << "Input\n";
    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        std::cout << in.op[idx_cb] << " " << in.lba[idx_cb] << " " << in.data[idx_cb] << " " << in.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif
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
            for (int idx_size = 0; idx_size < in.size[idx_cb]; idx_size++) {
                int current_lba = in.lba[idx_cb] + idx_size;
                virtual_op[current_lba] = OP_E;
#if 0
                if (virtual_op[current_lba] <= OP_W_MAX) {
                    virtual_op[current_lba] = OP_M;

                    for (int idx_cb_in = 0; idx_cb_in < cmdCount; idx_cb_in++)
                    {
                        if(current_lba == in.lba[idx_cb_in] && in.op[idx_cb_in] == "E")
                            //in.op[idx_cb_in] = "E";
                            virtual_op[current_lba] = OP_M;
                    }

                }
                else {
                    virtual_op[current_lba] = OP_E;
                }
#endif
            }
        }
    }
#ifdef PRINT_DEBUG_SSD_CMDB
    // display
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        if (virtual_op[idx_iba] == OP_NULL)
            std::cout << "." << " ";
        else if (virtual_op[idx_iba] == OP_E)
            std::cout << "E" << " ";
        else
            std::cout << "W" << " ";

        if (idx_iba % 10 == 9)
            std::cout << "\n";
    }
#endif
    // step - 3
    int continue_E_CMD = 0;
    int ersCmdConunt = 0;
    int wrCmdConunt = 0;

    // step - 3-1
    for (int idx_lba = 0; idx_lba < 100; idx_lba++) {
        // Check E and W
        if (virtual_op[idx_lba] != OP_NULL) {
            if (continue_E_CMD == 0) {
                if (virtual_op[idx_lba] == OP_E) {
                    ERS_CMD.op[ersCmdConunt] = "E";
                    ERS_CMD.lba[ersCmdConunt] = idx_lba;
                    ERS_CMD.size[ersCmdConunt] = 1;
                    continue_E_CMD = 1;
                }
                else {   //virtual_op[idx_lba] => OP_W
                    WR_CMD.op[wrCmdConunt] = "W";
                    WR_CMD.lba[wrCmdConunt] = idx_lba;
                    WR_CMD.size[wrCmdConunt] = 1;
                    WR_CMD.data[wrCmdConunt] = in.data[virtual_op[idx_lba]];
                    wrCmdConunt++;
                }
            }
            else {
                ERS_CMD.size[ersCmdConunt]++;
                continue_E_CMD++;
                if (virtual_op[idx_lba] <= OP_W_MAX) {
                    WR_CMD.op[wrCmdConunt] = "W";
                    WR_CMD.lba[wrCmdConunt] = idx_lba;
                    WR_CMD.size[wrCmdConunt] = 1;
                    WR_CMD.data[wrCmdConunt] = in.data[virtual_op[idx_lba]];
                    wrCmdConunt++;
                }

                if (continue_E_CMD == 10) {
                    continue_E_CMD = 0;
                    ersCmdConunt++;
                }
                else if (virtual_op[idx_lba + 1] == OP_NULL) {
                    continue_E_CMD = 0;
                    ersCmdConunt++;
                }
                else if (idx_lba == 99) {
                    continue_E_CMD = 0;
                    ersCmdConunt++;
                }
            }
        }
        else if (continue_E_CMD > 0) {
            continue_E_CMD = 0;
            ersCmdConunt++;
        }
    }
#if 0
    // display
    for (int idx_cb = 0; idx_cb < ersCmdConunt; idx_cb++) {
        std::cout << ERS_CMD.op[idx_cb] << " " << ERS_CMD.lba[idx_cb] << " " << ERS_CMD.data[idx_cb] << " " << ERS_CMD.size[idx_cb] << "\n";
    }
    std::cout << "\n";

    //Check W
    if (virtual_op[idx_lba] <= OP_W_MAX) {
        out.op[newCMDCount] = "W";
        out.lba[newCMDCount] = idx_lba;
        out.data[newCMDCount] = in.data[virtual_op[idx_lba]];
        out.size[newCMDCount] = 1;
        newCMDCount++;
    }

    int hit = 0;
    for (int idx_cb_in = 0; idx_cb_in < cmdCount; idx_cb_in++) {
        if (in.op[idx_cb_in] == "W" && in.data[idx_cb_in] != "0x00000000") {
            hit = 0;
            for (int idx_cb_out = 0; idx_cb_out < ersCmdConunt; idx_cb_out++) {
                if (ERS_CMD.lba[idx_cb_out] == in.lba[idx_cb_in]) {
                    if (ERS_CMD.size[idx_cb_out] != 1 && ERS_CMD.op[idx_cb_out] == "E") {
                        hit = 1;
                    }
                }
            }
            if (hit) {
                ERS_CMD.op[ersCmdConunt] = "W";
                ERS_CMD.lba[ersCmdConunt] = in.lba[idx_cb_in];
                ERS_CMD.size[ersCmdConunt] = 1;
                ERS_CMD.data[ersCmdConunt] = in.data[idx_cb_in];
                ersCmdConunt++;
            }
        }
    }
#endif
#if 0
    std::cout << "Step 3-1\n";
    // display
    for (int idx_cb = 0; idx_cb < ersCmdConunt; idx_cb++) {
        std::cout << ERS_CMD.op[idx_cb] << " " << ERS_CMD.lba[idx_cb] << " " << ERS_CMD.data[idx_cb] << " " << ERS_CMD.size[idx_cb] << "\n";
    }
    for (int idx_cb = 0; idx_cb < wrCmdConunt; idx_cb++) {
        std::cout << WR_CMD.op[idx_cb] << " " << WR_CMD.lba[idx_cb] << " " << WR_CMD.data[idx_cb] << " " << WR_CMD.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif
    int idx = 0;
    for (int idx_lba = 0; idx_lba < ersCmdConunt; idx_lba++) {
        out.op[idx] = ERS_CMD.op[idx_lba];
        out.lba[idx] = ERS_CMD.lba[idx_lba];
        out.size[idx] = ERS_CMD.size[idx_lba];
        out.data[idx] = ERS_CMD.data[idx_lba];
        idx++;
    }
    for (int idx_lba = 0; idx_lba < wrCmdConunt; idx_lba++) {
        out.op[idx] = WR_CMD.op[idx_lba];
        out.lba[idx] = WR_CMD.lba[idx_lba];
        out.size[idx] = WR_CMD.size[idx_lba];
        out.data[idx] = WR_CMD.data[idx_lba];
        idx++;
    }

    int outCmd_Count = ersCmdConunt + wrCmdConunt;
#ifdef PRINT_DEBUG_SSD_CMDB
    if (idx != outCmd_Count)
        std::cout << "Error idx " << idx << " " << ersCmdConunt << "\n";

    // display
    std::cout << "Output\n";
    for (int idx_cb = 0; idx_cb < outCmd_Count; idx_cb++) {
        std::cout << out.op[idx_cb] << " " << out.lba[idx_cb] << " " << out.data[idx_cb] << " " << out.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif
    return outCmd_Count;
}

int
SSD::reduceCMDBuffer(TEST_CMD in, TEST_CMD& out) {
    int newCMDCount = reduceCMDBufferMerge(in, out, 6);

    return newCMDCount;
}