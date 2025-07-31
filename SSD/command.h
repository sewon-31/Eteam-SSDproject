#pragma once
#define interface struct

#include <string>

interface ICommand
{
	virtual ~ICommand() = default;
	virtual std::string execute() = 0;

private:
	int lba;
};

