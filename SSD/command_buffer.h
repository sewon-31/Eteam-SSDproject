#pragma once
#include "command.h"

#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::string;

class CommandBuffer
{
public:
	CommandBuffer(const string& dirPath = "../buffer/");

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
	void addCommandToBuffer(std::shared_ptr<ICommand> command);

	void initDirectory();
	void updateFromDirectory();

	std::vector<std::shared_ptr<ICommand>> buffer;
	string bufferDirPath;
	//std::shared_ptr<SSD> ssd;
};
