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
	MergeCmd in;
	MergeCmd out;

	for (int bufIdx = 0; bufIdx < bufSize; bufIdx++) {
		auto cmd = buffer.at(bufIdx);
		auto type = cmd->getCmdType();

		if (type == CmdType::WRITE) {
			std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);
			
			in.op.push_back(cmd->getCmdType());
			in.lba.push_back(cmd->getLBA());
			in.size.push_back(1);
			in.data.push_back(wCmdPtr->getValue());

#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> WRITE" << " " << in.lba[bufIdx] << " " << in.data[bufIdx] << "\n";
#endif
		}
		if (type == CmdType::ERASE) {
			std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
			in.op.push_back(cmd->getCmdType());
			in.lba.push_back(cmd->getLBA());
			in.size.push_back(eCmdPtr->getSize());
			in.data.push_back("");

#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> ERASE" << " " << in.lba[bufIdx] << " " << in.size[bufIdx] << "\n";
#endif
		}
	}

	int newSize = mergeCmdBuffer(in, out);
	if (newSize < bufSize)
	{
		std::shared_ptr<SSDCommandBuilder> builder;

		if (!builder) {
			builder = std::make_shared<SSDCommandBuilder>();
		}
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
			string path = bufferDirPath + "/" + std::to_string(i) + "_" + EMPTY;
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
CommandBuffer::mergeCmdBuffer(MergeCmd in, MergeCmd& out)
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

#ifdef PRINT_DEBUG_CMDB
	printVirtualMap(virtualMap);
#endif

	// Step 3: Construct ersCmd and wrCmd
	MergeCmd ersCmd, wrCmd;
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

void CommandBuffer::printVirtualMap(const std::vector<int>& virtualMap)
{
	const int BUF_MAX = 100;
	const int OP_NULL = 9;
	const int OP_E = 7;

	int lineStart = 0;
	std::cout << "Map: 0 1 2 3 4 5 6 7 8 9\n" << lineStart << "  : ";
	lineStart += 10;
	for (int i = 0; i < BUF_MAX; ++i) {
		if (virtualMap[i] == OP_NULL) std::cout << ". ";
		else if (virtualMap[i] == OP_E) std::cout << "E ";
		else std::cout << "W ";
		if (i % 10 == 9 && i != BUF_MAX - 1) {
			std::cout << "\n" << lineStart << " : ";
			lineStart += 10;
		}
	}
	std::cout << "\n";
}
