#pragma once
#define interface struct

#include "nand_data.h"
#include "file_interface.h"

#include <string>
#include <vector>

using std::string;

enum class CmdType {
	READ = 0,
	WRITE,
	ERASE,
};

// ICommand (abstract class)
interface ICommand
{
	virtual ~ICommand() = default;
	virtual void run(string& result) = 0;		// run whole process (including file read/write)
	virtual void execute(string& result) = 0;	// execute core action
	virtual CmdType getCmdType() const = 0;
	virtual int getLBA() const = 0;
};

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

	std::vector<std::shared_ptr<ICommand>> buffers;
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