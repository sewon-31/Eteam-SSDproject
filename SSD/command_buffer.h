#pragma once
#include "command.h"

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
	CommandBuffer(const string& dirPath = "../buffer/");

	void Init();
	void updateToDirectory();

	int getBufferSize() const;
	const vector<std::shared_ptr<ICommand>>& getBuffer() const;
	int addCommand(std::shared_ptr<ICommand> command);

	void flushBuffer();
	bool optimizeBuffer();

	static const int BUFFER_MAX = 5;
	static constexpr const char* EMPTY = "empty";

private:
	void initDirectory();
	void updateFromDirectory();
	int reduceCMDBuffer(CMD_BUF in, CMD_BUF& out);
	std::vector<std::shared_ptr<ICommand>> buffer;
	string bufferDirPath;
	//std::shared_ptr<SSD> ssd;
};
