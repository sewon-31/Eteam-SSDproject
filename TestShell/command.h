#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ssd_interface.h"

class Command {
public:
    virtual ~Command() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;

    const static int MAX_LBA = 100;
};

// concreate commands
class ReadCommand : public Command {
public:
    ReadCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
protected:
    void ssdReadAndPrint(int addr);
private:
    SSDInterface* ssd;
    const string READ_HEADER = "[Read] LBA ";
    const string READ_MIDFIX = " : ";
    const string READ_FOOTER = "\n";

    void read(int lba);
};

class WriteCommand : public Command {
public:
    WriteCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:
    SSDInterface* ssd;

    void write(int lba, std::string value);
};

class FullReadCommand : public ReadCommand {
public:
    FullReadCommand(SSDInterface* ssd) : ReadCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:

    void fullRead();
};

class FullWriteCommand : public Command {
public:
    FullWriteCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:
    SSDInterface* ssd;

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

class EraseCommand : public Command {
public:
    EraseCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;

protected:
    void erase(int lba, int size);

private:
    SSDInterface* ssd;

    void parseSizeAndErase(int size, int lba);
};

class EraseRangeCommand : public EraseCommand {
public:
    EraseRangeCommand(SSDInterface* ssd) : EraseCommand(ssd) {}
    bool execute(const std::vector<std::string>& args) override;

private:
    void eraseRange(int startLba, int endLba);
};

class FlushCommand : public Command {
public:
    FlushCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;

private:
    SSDInterface* ssd;

    void flush();

};