#pragma once
#include "command_interface.h"

#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::string;

struct CMD_BUF {
public:
	CmdType op[6];
	int lba[6];
	int size[6];
	string data[6];
};

class CommandBuffer
{
public:
	static CommandBuffer& getInstance(const string& dirPath = "../buffer");

	void Init();
	void updateToDirectory();

	int getBufferSize() const;
	const vector<std::shared_ptr<ICommand>>& getBuffer() const;
	
	void addCommand(std::shared_ptr<ICommand> command);

	void clearBuffer();
	void flushBuffer();
	bool optimizeBuffer();

	static const int BUFFER_MAX = 5;
	static constexpr const char* EMPTY = "empty";

private:
	CommandBuffer(const string& dirPath);
	CommandBuffer(const CommandBuffer&) = delete;

	void addCommandToBuffer(std::shared_ptr<ICommand> command);

	void initDirectory();
	void updateFromDirectory();
	int reduceCMDBuffer(CMD_BUF in, CMD_BUF& out, int cmdCount);
	std::vector<std::shared_ptr<ICommand>> buffer;
	string bufferDirPath;
	//std::shared_ptr<SSD> ssd;
};
