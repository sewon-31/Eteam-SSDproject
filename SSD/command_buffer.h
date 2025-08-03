#pragma once
#include "command_interface.h"

#include <vector>
#include <memory>
#include <string>

#if _DEBUG
#define PRINT_DEBUG_CMDB 1
#endif
using std::vector;
using std::string;

struct ReduceCmd {
	std::vector<CmdType> op;
	std::vector<int> lba;
	std::vector<int> size;
	std::vector<std::string> data;
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

	int reduceCMDBuffer(ReduceCmd in, ReduceCmd& out);
private:
	CommandBuffer(const string& dirPath);
	CommandBuffer(const CommandBuffer&) = delete;

	void addCommandToBuffer(std::shared_ptr<ICommand> command);

	void initDirectory();
	void updateFromDirectory();
	std::vector<std::shared_ptr<ICommand>> buffer;
	string bufferDirPath;
	//std::shared_ptr<SSD> ssd;
	void printVirtualMap(const std::vector<int>& virtualMap);
};
