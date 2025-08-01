#include "command_buffer.h"

CommandBuffer::CommandBuffer(const string& dirPath)
	: file(dirPath)
{
}

const std::vector<std::shared_ptr<ICommand>>&
CommandBuffer::getBuffer() const
{
	return buffer;
}

int
CommandBuffer::addCommand(std::shared_ptr<ICommand> command)
{
	buffer.push_back(command);
	optimizeBuffer();

	return buffer.size();
}

void
CommandBuffer::flushBuffer()
{
	//ssd.getStorage().updateFromFile();

	string result;
	for (auto cmd : buffer) {
		cmd->execute(result);
	}

	//ssd.getStorage().updateToFile();

	buffer.clear();
}

bool
CommandBuffer::optimizeBuffer()
{
	return true;
}