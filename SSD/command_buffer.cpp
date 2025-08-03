#include "command_buffer.h"
#include "ssd_command_builder.h"

#include <filesystem>

#include <iostream>
#include <algorithm>
#include <sstream>

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
		addCommandToBuffer(command);;
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
	for (auto cmd : buffer) {
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
	ReduceCmd in;
	ReduceCmd out;

	for (int bufIdx = 0; bufIdx < bufSize; bufIdx++) {
		auto cmd = buffer.at(bufIdx);
		auto type = cmd->getCmdType();

#ifdef PRINT_DEBUG_CMDB
		std::cout << "optimizeBuffer : buffer_size" << " " << in.lba[bufIdx] << " " << in.data[bufIdx] << "\n";
#endif
		if (type == CmdType::WRITE) {
			std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);
			
			in.op.push_back(cmd->getCmdType());
			in.lba.push_back(cmd->getLBA());
			in.size.push_back(1);
			in.data.push_back(wCmdPtr->getValue());
#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> WRITE" << " " << in.lba[inBufIdx] << " " << in.data[inBufIdx] << "\n";
#endif
		}
		if (type == CmdType::ERASE) {
			std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
			in.op.push_back(cmd->getCmdType());
			in.lba.push_back(cmd->getLBA());
			in.size.push_back(1);
			in.data.push_back("");
#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> ERASE" << " " << in.lba[inBufIdx] << " " << in.size[inBufIdx] << "\n";
#endif
		}
	}

	int new_buf_size = reduceCMDBuffer(in, out);
	if (new_buf_size < bufSize)
	{
		std::shared_ptr<SSDCommandBuilder> builder;

		if (!builder) {
			builder = std::make_shared<SSDCommandBuilder>();
		}

		buffer.clear();

		for (int bufIdx = 0; bufIdx < new_buf_size; bufIdx++) {
			if (out.op[bufIdx] == CmdType::WRITE) {
				vector<string> commandVector = { "W" , std::to_string(out.lba[bufIdx])  , out.data[bufIdx] };
				auto newCmd = builder->createCommand(commandVector);
				buffer.push_back(newCmd);
#ifdef PRINT_DEBUG_CMDB
				std::cout << "OUT CMD -> WRITE" << " " << out.lba[bufIdx] << " " << out.data[bufIdx] << "\n";
#endif
			}
			else if (out.op[bufIdx] == CmdType::ERASE) {
				vector<string> commandVector = { "E" , std::to_string(out.lba[bufIdx])  , std::to_string(out.size[bufIdx]) };
				auto newCmd = builder->createCommand(commandVector);
				buffer.push_back(newCmd);
#ifdef PRINT_DEBUG_CMDB
				std::cout << "OUT CMD -> ERASE" << " " << out.lba[bufIdx] << " " << out.size[bufIdx] << "\n";
#endif
			}
		}
	}
	return true;
}

void
CommandBuffer::initDirectory()
{
	// create buffer directory (if needed)
	bool bufferDirExists = fs::exists(bufferDirPath) && fs::is_directory(bufferDirPath);

	if (!bufferDirExists) {
		if (fs::create_directory(bufferDirPath)) {
			for (int i = 1; i <= BUFFER_MAX; ++i) {
				string filePath = bufferDirPath + "/" + std::to_string(i) + "_empty";
				std::ofstream file(filePath);

				if (!file) {
#if _DEBUG
					std::cerr << "Failed to create " << filePath << std::endl;
#endif
				}
			}
		}
	}
}

void
CommandBuffer::updateFromDirectory()
{
	// read file names from directory
	vector<string> fileNames;

	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		if (fs::is_regular_file(file.path())) {
			fileNames.push_back(file.path().filename().string());
		}
	}

#if _DEBUG
	if (fileNames.size() != BUFFER_MAX) {
		std::cout << "Why not 5 files?\n";
	}
#endif

	// sort filenames
	std::sort(fileNames.begin(), fileNames.end());

	SSDCommandBuilder builder;

	auto startsWith = [](const string& str, const string& prefix) -> bool {
		return str.compare(0, prefix.size(), prefix) == 0;
		};

	int fileNum = 1;
	for (const auto& fileName : std::as_const(fileNames)) {
		string prefix = std::to_string(fileNum++) + "_";
		if (startsWith(fileName, prefix)) {
			string noPrefix = fileName.substr(prefix.length());

			if (noPrefix == EMPTY) {
				break;
			}

			vector<string> commandVector;
			std::stringstream ss(noPrefix);

			string token;
			while (std::getline(ss, token, '_')) {
				commandVector.push_back(token);
			}

			auto cmd = builder.createCommand(commandVector);
			if (cmd == nullptr) {
				std::cout << "Commmand not created - " << noPrefix << std::endl;
			}
			buffer.push_back(cmd);

		}
		else {
			std::cout << "Weird file " << fileName << std::endl;
		}
	}

	// delete all the files
	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		if (fs::is_regular_file(file.path())) {
			fs::remove(file.path());
		}
	}
}

void
CommandBuffer::updateToDirectory()
{
	// delete all the files
	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		if (fs::is_regular_file(file.path())) {
			fs::remove(file.path());
		}
	}

	if (buffer.size() > BUFFER_MAX) {
		std::cout << "vector size error" << std::endl;
	}

	// create cmd file
	if (buffer.size() > 0) {
		for (int i = 1; i <= buffer.size(); ++i) {
			string filePath = bufferDirPath + "/" + std::to_string(i) + "_";
			auto cmd = buffer.at(i - 1);
			if (cmd == nullptr) {
				filePath += EMPTY;
			}
			else {
				// concat type, lba, value/size
				auto type = cmd->getCmdType();
				string lbaStr = std::to_string(cmd->getLBA());

				if (type == CmdType::WRITE) {
					filePath += "W_" + lbaStr + "_";

					std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);
					if (wCmdPtr) {
						filePath += wCmdPtr->getValue();
					}
				}
				if (type == CmdType::ERASE) {
					filePath += "E_" + lbaStr + "_";
					std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
					if (eCmdPtr) {
						filePath += std::to_string(eCmdPtr->getSize());
					}
				}
			}

			// create file
			std::ofstream file(filePath);
			if (!file) {
#if _DEBUG
				std::cout << "Failed to create " << filePath << std::endl;
#endif
			}
		}
	}

	for (int i = buffer.size() + 1; i <= BUFFER_MAX; ++i) {
		string filePath = bufferDirPath + "/" + std::to_string(i) + "_" + EMPTY;

		// create file
		std::ofstream file(filePath);
		if (!file) {
#if _DEBUG
			std::cout << "Failed to create " << filePath << std::endl;
#endif
		}
	}
}

int 
CommandBuffer::reduceCMDBuffer(ReduceCmd in, ReduceCmd& out)
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
	printVirtualMap(virtualMap);
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
#ifdef _DEBUG
void CommandBuffer::printVirtualMap(std::vector<int>& virtualMap)
{
	const int BUF_MAX = 100;
	const int OP_NULL = 9;
	const int OP_E = 7;

	int lineStart = 0;
	std::cout << "\nMap: 0 1 2 3 4 5 6 7 8 9\n" << lineStart << "  : ";
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
}
#endif