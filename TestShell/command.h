#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ssd_interface.h"

class Command {
public:
    virtual 
    virtual ~Command() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;
};

// concreate commands
class ReadCommand : public Command {
public:
    ReadCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:
    SSDInterface* ssd;
    const string READ_HEADER = "[Read] LBA ";
    const string READ_MIDFIX = " : ";
    const string READ_FOOTER = "\n";

    void read(int lba);
    void ssdReadAndPrint(int addr);
};
class WriteCommand : public Command {
public:
    WriteCommand(SSDInterface* ssd) : ssd(ssd) {}
    bool execute(const std::vector<std::string>& args) override;
private:
    SSDInterface* ssd;

    void write(int lba, std::string value);
};
class FullReadCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
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
class TestScript1Command : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
};
class TestScript2Command : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
};
class TestScript3Command : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
};

// command Factory

