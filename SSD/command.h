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
	virtual void run(string& result) = 0;
	virtual void execute(string& result) = 0;
	virtual CmdType getCmdType() const = 0;
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

	BaseCommand(NandData& storage, int lba);
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
	WriteCommand(NandData& storage, int lba, const std::string& value);

	void run(string& result) override;
	void execute(string& result) override;
	CmdType getCmdType() const override;

private:
	string value;
};

// EraseCommand
class EraseCommand : public BaseCommand
{
public:
	EraseCommand(NandData& storage, int lba, int size);

	void run(string& result) override;
	void execute(string& result) override;
	CmdType getCmdType() const override;

private:
	int size;   // 0~10
};