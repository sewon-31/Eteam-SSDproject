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
int SSD::reduceCMDBuffer(ReduceCmd in, ReduceCmd& out)
{
	const int BUF_MAX = 100;
	const int OP_NULL = 9;
	const int OP_E = 7;
	const int OP_W_MAX = 5;
	int cmdCount = in.op.size();

	// Step 1: Replace WRITE with 0x00000000 by ERASE
	for (int i = 0; i < cmdCount; i++) {
		if (in.op[i] == CmdType::WRITE && in.data[i] == "0x00000000") {
			in.op[i] = CmdType::ERASE;
			in.size[i] = 1;
			in.data[i] = "";
		}
	}

	// Step 2: Build virtualMap
	std::vector<int> virtualMap(BUF_MAX, OP_NULL);

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
	for (int i = 0; i < BUF_MAX; ++i) {
		if (virtualMap[i] == OP_NULL) std::cout << ". ";
		else if (virtualMap[i] == OP_E) std::cout << "E ";
		else std::cout << "W ";
		if (i % 10 == 9) std::cout << "\n";
	}
#endif

	// Step 3: Construct ERS_CMD and WR_CMD
	ReduceCmd ERS_CMD, WR_CMD;
	int eraseCmdSequence = 0;
	bool isEraseRange = false;

	for (int idx_lba = 0; idx_lba < BUF_MAX; idx_lba++) {
		int vmp = virtualMap[idx_lba];

		if (vmp != OP_NULL) {
			if (eraseCmdSequence == 0) {  // First erase range
				if (vmp == OP_E) {  // First erase range and Construct new ERS commands 
					ERS_CMD.op.push_back(CmdType::ERASE);
					ERS_CMD.lba.push_back(idx_lba);
					ERS_CMD.size.push_back(1);
					ERS_CMD.data.push_back("");
					eraseCmdSequence = 1;
				}
				else {
					WR_CMD.op.push_back(CmdType::WRITE);
					WR_CMD.lba.push_back(idx_lba);
					WR_CMD.size.push_back(1);
					WR_CMD.data.push_back(in.data[vmp]);
				}
			}
			else {         // Erase range loop
				ERS_CMD.size.back() += 1;
				eraseCmdSequence++;
				if (vmp <= OP_W_MAX) {
					WR_CMD.op.push_back(CmdType::WRITE);
					WR_CMD.lba.push_back(idx_lba);
					WR_CMD.size.push_back(1);
					WR_CMD.data.push_back(in.data[vmp]);
				}

				// Check if erase range ends
				bool erase_end = false;
				if (eraseCmdSequence == 10 || idx_lba == BUF_MAX - 1 || virtualMap[idx_lba + 1] == OP_NULL)
					erase_end = true;

				if (erase_end) {
					eraseCmdSequence = 0;
				}
			}
		}
		else if (eraseCmdSequence > 0) {
			eraseCmdSequence = 0;
		}
	}

	// Step 4: Combine ERS_CMD and WR_CMD into out

	// Append erase commands
	out.op.insert(out.op.end(), ERS_CMD.op.begin(), ERS_CMD.op.end());
	out.lba.insert(out.lba.end(), ERS_CMD.lba.begin(), ERS_CMD.lba.end());
	out.size.insert(out.size.end(), ERS_CMD.size.begin(), ERS_CMD.size.end());
	out.data.insert(out.data.end(), ERS_CMD.data.begin(), ERS_CMD.data.end());

	// Append write commands
	out.op.insert(out.op.end(), WR_CMD.op.begin(), WR_CMD.op.end());
	out.lba.insert(out.lba.end(), WR_CMD.lba.begin(), WR_CMD.lba.end());
	out.size.insert(out.size.end(), WR_CMD.size.begin(), WR_CMD.size.end());
	out.data.insert(out.data.end(), WR_CMD.data.begin(), WR_CMD.data.end());

	return static_cast<int>(out.op.size());
}

int
SSD::reduceCMDBufferTest(TEST_CMD in, TEST_CMD& out) {
	ReduceCmd conv_in, conv_out;

	// Convert ReduceCmd format
	for (int i = 0; i < 6; ++i) {
		if (in.op[i] == "E")
			conv_in.op.push_back(CmdType::ERASE);
		else if (in.op[i] == "W")
			conv_in.op.push_back(CmdType::WRITE);
		else
			continue;

		conv_in.lba.push_back(in.lba[i]);
		conv_in.size.push_back(in.size[i]);
		conv_in.data.push_back(in.data[i]);
	}

#ifdef PRINT_DEBUG_SSD_CMDB
	std::cout << "=== Input Commands ===\n";
	for (int i = 0; i < conv_in.op.size(); ++i) {
		if (in.op[i] != "W" && in.op[i] != "E") break;
		std::cout << "[" << i << "] "
			<< "OP: " << in.op[i]
			<< ", LBA: " << in.lba[i]
			<< ", DATA: " << (in.op[i] == "W" ? in.data[i] : "-")
			<< ", SIZE: " << in.size[i]
			<< "\n";
	}
	std::cout << "=======================\n\n";
#endif

	// Run reduceCMDBuffer
	int newCMDCount = reduceCMDBuffer(conv_in, conv_out);

	// Convert TEST_CMD format
	for (int i = 0; i < newCMDCount; ++i) {
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
	for (int i = 0; i < newCMDCount; ++i) {
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