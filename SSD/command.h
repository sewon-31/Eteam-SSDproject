#pragma once
#define interface struct

#include "command_buffer.h"

#include <vector>

using std::string;

// BaseCommand (abstract class) for common data members
class BaseCommand : public ICommand
{
protected:
	NandData& storage;
	int lba;

public:
	enum OP {
		READ = 0,
		WRITE = 1,
		ERASE = 2
	};

	BaseCommand(int lba);
	int getLBA() const;
};

// ReadCommand
class ReadCommand : public BaseCommand
{
public:
	using BaseCommand::BaseCommand;

	void run(string& result) override;
	void execute(string& result) override;
	CmdType getCmdType() const override;

private:
	string updateNandDataFromBuffer();
};

// WriteCommand
class WriteCommand : public BaseCommand
{
public:
	WriteCommand(int lba, const std::string& value);

	void run(string& result) override;
	void execute(string& result) override;
	CmdType getCmdType() const override;

	string getValue() const;

private:
	string value;
};

// EraseCommand
class EraseCommand : public BaseCommand
{
public:
    EraseCommand(int lba, int size);

	void run(string& result) override;
	void execute(string& result) override;
	CmdType getCmdType() const override;

	int getSize() const;

private:
    int size;
};