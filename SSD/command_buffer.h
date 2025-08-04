#pragma once
#include "command_interface.h"

#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::string;

struct MergeCmd {
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
	int mergeCmdBuffer(const MergeCmd in, MergeCmd& out);

	static const int BUFFER_MAX = 5;
	static constexpr const char* EMPTY = "empty";

private:
	CommandBuffer(const string& dirPath);
	CommandBuffer(const CommandBuffer&) = delete;

	void addCommandToBuffer(std::shared_ptr<ICommand> command);

	void initDirectory();
	void updateFromDirectory();

	void updateMergeCmd(int new_buf_size, MergeCmd& out);
	void getMergeCmd(MergeCmd& in);

	void replaceZeroWriteCmdToEraseCmd(MergeCmd& in);
	void buildVirtualMap(const MergeCmd& in, std::vector<int>& virtualMap);
	void buildMergedCmd(const std::vector<int>& virtualMap, MergeCmd& ersCmd, MergeCmd& wrCmd, const MergeCmd& in);
	void updateCommandBuffer(MergeCmd& out, const  MergeCmd& ersCmd, const  MergeCmd& wrCmd);

	std::vector<std::shared_ptr<ICommand>> buffer;
	string bufferDirPath;
	void printVirtualMap(const std::vector<int>& virtualMap);

	const int BUF_MAX = 100;
	const int OP_NULL = 9;
	const int OP_E = 7;
	const int OP_W_MAX = 5;
};
