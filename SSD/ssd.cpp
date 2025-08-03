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

	// Step 3: Construct ersCmd and wrCmd
	ReduceCmd ersCmd, wrCmd;
	int eraseCmdSequence = 0;
	bool isEraseRange = false;

	for (int idxLba = 0; idxLba < BUF_MAX; idxLba++) {
		int vmp = virtualMap[idxLba];

		if (vmp != OP_NULL) {
			if (eraseCmdSequence == 0) {	// First erase range
				if (vmp == OP_E) {			// First erase range and Construct new ERS commands 
					ersCmd.op.push_back(CmdType::ERASE);
					ersCmd.lba.push_back(idxLba);
					ersCmd.size.push_back(1);
					ersCmd.data.push_back("");
					eraseCmdSequence = 1;
				}
				else {
					wrCmd.op.push_back(CmdType::WRITE);
					wrCmd.lba.push_back(idxLba);
					wrCmd.size.push_back(1);
					wrCmd.data.push_back(in.data[vmp]);
				}
			}
			else {							// Erase range loop
				ersCmd.size.back() += 1;
				eraseCmdSequence++;
				if (vmp <= OP_W_MAX) {
					wrCmd.op.push_back(CmdType::WRITE);
					wrCmd.lba.push_back(idxLba);
					wrCmd.size.push_back(1);
					wrCmd.data.push_back(in.data[vmp]);
				}

				// Check if erase range ends
				bool eraseEnd = false;
				if (eraseCmdSequence == 10 || idxLba == BUF_MAX - 1)
					eraseEnd = true;

				if (idxLba + 1 < BUF_MAX)
					if (virtualMap[idxLba + 1] == OP_NULL)
						eraseEnd = true;

				if (eraseEnd) {
					eraseCmdSequence = 0;
				}
			}
		}
		else if (eraseCmdSequence > 0) {
			eraseCmdSequence = 0;
		}
	}

	// Step 4: Combine ersCmd and wrCmd into out
	// Append erase commands
	out.op.insert(out.op.end(), ersCmd.op.begin(), ersCmd.op.end());
	out.lba.insert(out.lba.end(), ersCmd.lba.begin(), ersCmd.lba.end());
	out.size.insert(out.size.end(), ersCmd.size.begin(), ersCmd.size.end());
	out.data.insert(out.data.end(), ersCmd.data.begin(), ersCmd.data.end());

	// Append write commands
	out.op.insert(out.op.end(), wrCmd.op.begin(), wrCmd.op.end());
	out.lba.insert(out.lba.end(), wrCmd.lba.begin(), wrCmd.lba.end());
	out.size.insert(out.size.end(), wrCmd.size.begin(), wrCmd.size.end());
	out.data.insert(out.data.end(), wrCmd.data.begin(), wrCmd.data.end());

	return static_cast<int>(out.op.size());
}

int
SSD::reduceCMDBufferTest(TestCmdFormat in, TestCmdFormat& out) {
	ReduceCmd testCmdIn, testCmdOut;

	// Convert ReduceCmd format
	for (int i = 0; i < 6; ++i) {
		if (in.op[i] == "E")
			testCmdIn.op.push_back(CmdType::ERASE);
		else if (in.op[i] == "W")
			testCmdIn.op.push_back(CmdType::WRITE);
		else
			continue;

		testCmdIn.lba.push_back(in.lba[i]);
		testCmdIn.size.push_back(in.size[i]);
		testCmdIn.data.push_back(in.data[i]);
	}

#ifdef PRINT_DEBUG_SSD_CMDB
	std::cout << "=== Input Commands ===\n";
	for (int i = 0; i < testCmdIn.op.size(); ++i) {
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
	int newCMDCount = reduceCMDBuffer(testCmdIn, testCmdOut);

	// Convert TestCmdFormat format
	for (int i = 0; i < newCMDCount; ++i) {
		if (testCmdOut.op[i] == CmdType::ERASE)
			out.op[i] = "E";
		else if (testCmdOut.op[i] == CmdType::WRITE)
			out.op[i] = "W";
		else
			out.op[i] = "N";

		out.lba[i] = testCmdOut.lba[i];
		out.size[i] = testCmdOut.size[i];
		out.data[i] = testCmdOut.data[i];
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