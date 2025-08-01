#pragma once
#define interface struct

#include "nand_data.h"
#include <string>

using std::string;

enum class CmdType {
	READ = 0,
	WRITE,
	ERASE,
	FLUSH
};

// ICommand (abstract class)
interface ICommand
{
	virtual ~ICommand() = default;
	virtual void run(string& result) = 0;		// run whole process (including file read/write)
	virtual void execute(string& result) = 0;	// execute core action
	virtual CmdType getCmdType() const = 0;
	virtual int getLBA() const = 0;

	const string ERASE_DATA = "0x00000000";
}; 
