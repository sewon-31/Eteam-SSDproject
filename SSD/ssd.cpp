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
SSD::reduceCMDBuffer(CMD_BUF in, CMD_BUF& out, int cmdCount)
{
	const int BUF_MAX = 100;
	const int OP_NULL = 9;
	const int OP_E = 7;
	const int OP_W_MAX = 5;

	// Step 1: Replace "W" commands with data "0x00000000" by "E" commands size=1
	for (int i = 0; i < cmdCount; i++) {
		if (in.op[i] == CmdType::WRITE && in.data[i] == "0x00000000") {
			in.op[i] = CmdType::ERASE;
			in.size[i] = 1;
			in.data[i] = "";
		}
	}

	// Step 2: Build virtualMap array
	int virtualMap[BUF_MAX];
	for (int i = 0; i < BUF_MAX; i++) {
		virtualMap[i] = OP_NULL;
	}

	for (int i = 0; i < cmdCount; i++) {
		if (in.op[i] == CmdType::WRITE) {
			if (in.lba[i] < BUF_MAX)
				virtualMap[in.lba[i]] = i;
		}
		else if (in.op[i] == CmdType::ERASE) {
			for (int s = 0; s < in.size[i]; s++) {
				int curLba = in.lba[i] + s;
				if (curLba < BUF_MAX)
					virtualMap[curLba] = OP_E;
			}
		}
	}
#ifdef _DEBUG
	for (int idxLba = 0; idxLba < 100; idxLba++) {
		if (virtualMap[idxLba] == OP_NULL)
			std::cout << "." << " ";
		else if (virtualMap[idxLba] == OP_E)
			std::cout << "E" << " ";
		else
			std::cout << "W" << " ";

		if (idxLba % 10 == 9)
			std::cout << "\n";
	}
#endif

	// Step 3: Construct new commands (ERS_CMD and WR_CMD)
	CMD_BUF ERS_CMD, WR_CMD;
	int continueErsCMD = 0;
	int ersCmdConunt = 0;
	int wrCmdConunt = 0;

	for (int idx_lba = 0; idx_lba < 100; idx_lba++) {
		int vmp = virtualMap[idx_lba];

		// Check E and W
		if (vmp != OP_NULL) {
			if (continueErsCMD == 0) {  // First erase range
				if (vmp == OP_E) {  // First erase range + Construct new ERS commands 
					ERS_CMD.op[ersCmdConunt] = CmdType::ERASE;
					ERS_CMD.lba[ersCmdConunt] = idx_lba;
					ERS_CMD.size[ersCmdConunt] = 1;
					continueErsCMD = 1;
				}
				else {              // First erase range and WR command
					WR_CMD.op[wrCmdConunt] = CmdType::WRITE;
					WR_CMD.lba[wrCmdConunt] = idx_lba;
					WR_CMD.size[wrCmdConunt] = 1;
					WR_CMD.data[wrCmdConunt] = in.data[vmp];
					wrCmdConunt++;
				}
			}
			else {      // Erase range loop
				ERS_CMD.size[ersCmdConunt]++;
				continueErsCMD++;
				if (vmp <= OP_W_MAX) {  // Construct new WR commands 
					WR_CMD.op[wrCmdConunt] = CmdType::WRITE;
					WR_CMD.lba[wrCmdConunt] = idx_lba;
					WR_CMD.size[wrCmdConunt] = 1;
					WR_CMD.data[wrCmdConunt] = in.data[vmp];
					wrCmdConunt++;
				}

				// Check if erase range ends
				bool erase_end = false;
				if (continueErsCMD == 10)
					erase_end = true;
				else if (idx_lba == BUF_MAX - 1)
					erase_end = true;
				else if (virtualMap[idx_lba + 1] == OP_NULL)
					erase_end = true;

				if (erase_end) {
					ersCmdConunt++;
					continueErsCMD = 0;
				}
			}
		}
		else if (continueErsCMD > 0) {
			continueErsCMD = 0;
			ersCmdConunt++;
		}
	}

	// Step 3: Combine ERS_CMD and WR_CMD to out
	int outCmdCount = 0;
	for (int idx_lba = 0; idx_lba < ersCmdConunt; idx_lba++) {
		out.op[outCmdCount] = ERS_CMD.op[idx_lba];
		out.lba[outCmdCount] = ERS_CMD.lba[idx_lba];
		out.size[outCmdCount] = ERS_CMD.size[idx_lba];
		out.data[outCmdCount] = ERS_CMD.data[idx_lba];
		outCmdCount++;
	}
	for (int idx_lba = 0; idx_lba < wrCmdConunt; idx_lba++) {
		out.op[outCmdCount] = WR_CMD.op[idx_lba];
		out.lba[outCmdCount] = WR_CMD.lba[idx_lba];
		out.size[outCmdCount] = WR_CMD.size[idx_lba];
		out.data[outCmdCount] = WR_CMD.data[idx_lba];
		outCmdCount++;
	}

	return outCmdCount;
}


int
SSD::reduceCMDBufferTest(TEST_CMD in, TEST_CMD& out) {
	CMD_BUF conv_in, conv_out;

	for (int i = 0; i < 6; i++) {
		if (in.op[i] == "E")
			conv_in.op[i] = CmdType::ERASE;
		else if (in.op[i] == "W")
			conv_in.op[i] = CmdType::WRITE;
		else
			conv_in.op[i] = CmdType::FLUSH;

		conv_in.lba[i] = in.lba[i];
		conv_in.size[i] = in.size[i];
		conv_in.data[i] = in.data[i];
	}
#ifdef PRINT_DEBUG_SSD_CMDB
	std::cout << "=== Input Commands ===\n";
	for (int i = 0; i < 6; i++) {
		if (in.op[i] != "W" && in.op[i] != "E")    break;

		std::cout << "[" << i << "] "
			<< "OP: " << in.op[i]
			<< ", LBA: " << in.lba[i]
			<< ", DATA: " << (in.op[i] == "W" ? in.data[i] : "-")
			<< ", SIZE: " << in.size[i]
			<< "\n";
	}
	std::cout << "=======================\n\n";
#endif

	int newCMDCount = reduceCMDBuffer(conv_in, conv_out, 6);

	for (int i = 0; i < newCMDCount; i++) {
		if (conv_out.op[i] == CmdType::ERASE)
			out.op[i] = "E";
		else if (conv_out.op[i] == CmdType::WRITE)
			out.op[i] = "W";
		else
			out.op[i] = "N";

		out.lba[i] = conv_out.lba[i];
		out.size[i] = conv_out.size[i];
		out.data[i] = conv_out.data[i];
	}
#ifdef PRINT_DEBUG_SSD_CMDB
	std::cout << "=== Optimized Command Output ===\n";
	for (int i = 0; i < newCMDCount; i++) {
		std::cout << "[" << i << "] "
			<< "OP: " << out.op[i]
			<< ", LBA: " << out.lba[i]
			<< ", DATA: " << (out.op[i] == "W" ? out.data[i] : "-")
			<< ", SIZE: " << out.size[i]
			<< "\n";
	}
	std::cout << "================================\n\n";
#endif
	return newCMDCount;
}