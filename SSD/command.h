#pragma once
#define interface struct

#include "nand_data.h"
#include "file_interface.h"

#include <string>

using std::string;

// ICommand (abstract class)
interface ICommand
{
	virtual ~ICommand() = default;
	virtual void execute() = 0;
};

// BaseCommand (abstract class) for common data members
class BaseCommand : public ICommand 
{
protected:
    NandData& storage;
	int lba;

public:
    BaseCommand(NandData& storage, int lba);
};

// ReadCommand
class ReadCommand : public BaseCommand 
{
public:
    using BaseCommand::BaseCommand;

    void execute() override;
};

// WriteCommand
class WriteCommand : public BaseCommand 
{
public:
    WriteCommand(NandData& storage, int lba, const std::string& value);

    void execute() override;

private:
    string value;
};

// EraseCommand
class EraseCommand : public BaseCommand 
{
public:
    EraseCommand(NandData& storage, int lba, int size);

    void execute() override;

private:
    int size;   // 0~10
};