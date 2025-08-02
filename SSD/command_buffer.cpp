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
			std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);

			in.op[inBufIdx] = cmd->getCmdType();
			in.lba[inBufIdx] = cmd->getLBA();
			in.data[inBufIdx] = wCmdPtr->getValue();
			in.size[inBufIdx] = 1;
#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> WRITE" << " " << in.lba[inBufIdx] << " " << in.data[inBufIdx] << "\n";
#endif
			inBufIdx++;
		}
		if (type == CmdType::ERASE) {
			std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
			in.op[inBufIdx] = cmd->getCmdType();
			in.lba[inBufIdx] = cmd->getLBA();
			in.size[inBufIdx] = eCmdPtr->getSize();
#ifdef PRINT_DEBUG_CMDB
			std::cout << "IN CMD -> ERASE" << " " << in.lba[inBufIdx] << " " << in.size[inBufIdx] << "\n";
#endif
			inBufIdx++;
		}
	}

	int new_buf_size = reduceCMDBuffer(in, out, inBufIdx);
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
