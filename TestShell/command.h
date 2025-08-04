#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ssd_interface.h"

class Command {
public:
    virtual ~Command() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;

    const static int MAX_LBA = 99;
    const static int MIN_LBA = 0;
    const static int NUM_OF_LBA = 100;
};

class IOCommand: public Command {
public:
    IOCommand(SSDInterface* ssd) :ssd(ssd) {}
    virtual ~IOCommand() = default;
protected:
    SSDInterface* ssd;
private:
    IOCommand() = default;
};

// concreate commands
class ReadCommand : public IOCommand {
public:
    ReadCommand(SSDInterface* ssd) : IOCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
protected:
    void ssdReadAndPrint(int addr);
private:
    const string READ_HEADER = "[Read] LBA ";
    const string READ_MIDFIX = " : ";
    const string READ_FOOTER = "\n";

    const int ARG_IDX_LBA = 0;

    void read(int lba);
};

class WriteCommand : public IOCommand {
public:
    WriteCommand(SSDInterface* ssd) : IOCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:
    const int ARG_IDX_LBA = 0;
    const int ARG_IDX_VALUE = 1;

    void write(int lba, std::string value);
};

class FullReadCommand : public ReadCommand {
public:
    FullReadCommand(SSDInterface* ssd) : ReadCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:

    void fullRead();
};

class FullWriteCommand : public IOCommand {
public:
    FullWriteCommand(SSDInterface* ssd) : IOCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:
    const int ARG_IDX_VALUE = 0;

    void fullWrite(std::string value);
};

class ExitCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
};

class HelpCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
};

class EraseCommand : public IOCommand {
public:
    EraseCommand(SSDInterface* ssd) : IOCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;

protected:
    const int ARG_IDX_LBA = 0;
    const int ARG_IDX_SIZE = 1;
    const int MAX_ERASE_SIZE = 10;

    void erase(int lba, int size);
    void parseSizeAndErase(int lba, int size);

};

class EraseRangeCommand : public EraseCommand {
public:
    EraseRangeCommand(SSDInterface* ssd) : EraseCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;

private:
    const int ARG_IDX_START_LBA = 0;
    const int ARG_IDX_END_LBA = 1;

    void eraseRange(int startLba, int endLba);
};

class FlushCommand : public IOCommand {
public:
    FlushCommand(SSDInterface* ssd) : IOCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;

private:
    void flush();
};