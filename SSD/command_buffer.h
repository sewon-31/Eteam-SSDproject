#pragma once
#include "command.h"
#include "ssd.h"

#include <vector>
#include <memory>
#include <string>

class CommandBuffer
{
public:
	CommandBuffer(const string& dirPath = "");

	void updateFromDirectory();
	void updateToDirectory();

	const std::vector<std::shared_ptr<ICommand>>& getBuffer() const;
	int addCommand(std::shared_ptr<ICommand> command);

	void flushBuffer();

private:
	std::vector<std::shared_ptr<ICommand>> buffer;
	FileInterface file;
	SSD& ssd;

	bool optimizeBuffer();
};