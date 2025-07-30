#include "ssd.h"

void 
SSD::setParser(SSDCommandParser* parser)
{
	this->parser = parser;
}

void
SSD::run(const string& commandStr)
{
	// parse command
	parser->setCommand(commandStr);
	if (!parser->isValidCommand()) {
		// print ERROR to ssd_output.txt
		return;
	}

	parsedCommand = parser->getCommandVector();
}