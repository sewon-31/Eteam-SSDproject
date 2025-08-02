#include "command_buffer.h"
#include "ssd_command_builder.h"
#include "file_interface.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

CommandBuffer&
CommandBuffer::getInstance(const std::string& dirPath) {
	static CommandBuffer instance(dirPath);
	return instance;
}

CommandBuffer::CommandBuffer(const string& dirPath)
	: bufferDirPath(dirPath)
{
}

void
CommandBuffer::Init()
{
	initDirectory();
	updateFromDirectory();
}

int
CommandBuffer::getBufferSize() const
{
	return buffer.size();
}

const std::vector<std::shared_ptr<ICommand>>&
CommandBuffer::getBuffer() const
{
	return buffer;
}

void
CommandBuffer::addCommand(std::shared_ptr<ICommand> command)
{
	int bufSize = buffer.size();

	if (bufSize == 0) {
		addCommandToBuffer(command);
	}
	else if (bufSize == CommandBuffer::BUFFER_MAX) {
		flushBuffer();
		addCommandToBuffer(command);
	}
	else {
		addCommandToBuffer(command);
		optimizeBuffer();
	}
}

void
CommandBuffer::clearBuffer()
{
	buffer.clear();
}

void
CommandBuffer::flushBuffer()
{
	NandData::getInstance().updateFromFile();

	string result;
	for (auto& cmd : buffer) {
		cmd->execute(result);
	}

	NandData::getInstance().updateToFile();
	clearBuffer();
}

void
CommandBuffer::addCommandToBuffer(std::shared_ptr<ICommand> command)
{
	buffer.push_back(command);
}

bool
CommandBuffer::optimizeBuffer()
{
	int bufSize = static_cast<int>(buffer.size());
	CMD_BUF in;
	CMD_BUF out;
	int inBufIdx = 0;

	for (int bufIdx = 0; bufIdx < bufSize; bufIdx++) {
		auto cmd = buffer.at(bufIdx);
		auto type = cmd->getCmdType();
#ifdef PRINT_DEBUG_CMDB
		std::cout << "optimizeBuffer : buffer_size" << " " << in.lba[bufIdx] << " " << in.data[bufIdx] << "\n";
#endif
		if (type == CmdType::WRITE) {
			auto wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);
			in.op[inBufIdx] = type;
			in.lba[inBufIdx] = cmd->getLBA();
			in.data[inBufIdx] = wCmdPtr->getValue();
			in.size[inBufIdx] = 1;
#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> WRITE" << " " << in.lba[inBufIdx] << " " << in.data[inBufIdx] << "\n";
#endif

			inBufIdx++;
		}
		else if (type == CmdType::ERASE) {
			auto eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
			in.op[inBufIdx] = type;
			in.lba[inBufIdx] = cmd->getLBA();
			in.size[inBufIdx] = eCmdPtr->getSize();
#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> ERASE" << " " << in.lba[inBufIdx] << " " << in.size[inBufIdx] << "\n";
#endif

			inBufIdx++;
		}
	}

	int newSize = reduceCMDBuffer(in, out, inBufIdx);
	if (newSize < bufSize) {
		std::shared_ptr<SSDCommandBuilder> builder = std::make_shared<SSDCommandBuilder>();
		buffer.clear();

		for (int bufIdx = 0; bufIdx < newSize; ++bufIdx) {
			vector<string> commandVector;
			if (out.op[bufIdx] == CmdType::WRITE) {
				commandVector = { "W", std::to_string(out.lba[bufIdx]), out.data[bufIdx] };
			}
			else if (out.op[bufIdx] == CmdType::ERASE) {
				commandVector = { "E", std::to_string(out.lba[bufIdx]), std::to_string(out.size[bufIdx]) };
			}
			auto newCmd = builder->createCommand(commandVector);
			buffer.push_back(newCmd);
		}
	}
	return true;
}

void
CommandBuffer::initDirectory()
{
	if (!FileInterface::directoryExists(bufferDirPath)) {
		FileInterface::createDirectory(bufferDirPath);
		for (int i = 1; i <= BUFFER_MAX; ++i) {
			string path = bufferDirPath + "/" + std::to_string(i) + "_empty";
			FileInterface::writeLine(path, "", false);
		}
	}
}

void
CommandBuffer::updateFromDirectory()
{
	vector<string> fileNames;
	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		if (fs::is_regular_file(file.path())) {
			fileNames.push_back(file.path().filename().string());
		}
	}

	std::sort(fileNames.begin(), fileNames.end());
	SSDCommandBuilder builder;

	int fileNum = 1;
	for (const auto& fileName : fileNames) {
		string prefix = std::to_string(fileNum++) + "_";
		if (fileName.rfind(prefix, 0) == 0) {
			string commandStr = fileName.substr(prefix.size());

			if (commandStr == EMPTY) break;

			std::stringstream ss(commandStr);
			vector<string> tokens;
			string token;
			while (std::getline(ss, token, '_')) {
				tokens.push_back(token);
			}
			auto cmd = builder.createCommand(tokens);
			if (cmd) buffer.push_back(cmd);
		}
	}

	// cleanup
	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		fs::remove(file.path());
	}
}

void
CommandBuffer::updateToDirectory()
{
	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		fs::remove(file.path());
	}

	for (int i = 1; i <= BUFFER_MAX; ++i) {
		string path = bufferDirPath + "/" + std::to_string(i) + "_";

		if (i <= buffer.size()) {
			auto cmd = buffer[i - 1];
			auto type = cmd->getCmdType();
			path += (type == CmdType::WRITE ? "W_" : "E_") + std::to_string(cmd->getLBA()) + "_";

			if (type == CmdType::WRITE) {
				auto w = std::dynamic_pointer_cast<WriteCommand>(cmd);
				path += w ? w->getValue() : "";
			}
			else if (type == CmdType::ERASE) {
				auto e = std::dynamic_pointer_cast<EraseCommand>(cmd);
				path += e ? std::to_string(e->getSize()) : "";
			}
		}
		else {
			path += EMPTY;
		}

		FileInterface::writeLine(path, "", false);
	}
}

int
CommandBuffer::reduceCMDBuffer(CMD_BUF in, CMD_BUF& out, int cmdCount)
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
