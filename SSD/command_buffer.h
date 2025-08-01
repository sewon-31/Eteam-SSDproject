#pragma once
#include "command.h"
#include "ssd.h"

#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::string;

class CommandBuffer
{
public:
	CommandBuffer(const string& dirPath = "");

	void updateFromDirectory();
	void updateToDirectory();

	const vector<std::shared_ptr<ICommand>>& getBuffer() const;
	int addCommand(std::shared_ptr<ICommand> command);

	void flushBuffer();

	static const int BUFFER_MAX = 5;
	static constexpr const char* EMPTY = "empty";

private:
	std::vector<std::shared_ptr<ICommand>> buffer;
	string bufferDirPath;
	//std::shared_ptr<SSD> ssd;

	bool optimizeBuffer();
};
