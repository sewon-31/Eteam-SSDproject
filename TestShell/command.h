#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ssd_interface.h"

class Command {
public:
    virtual 
    virtual ~Command() = default;
    // 반환값은 TestShell이 계속 실행될지(true) 종료될지(false)를 나타냅니다.
    virtual bool execute(const std::vector<std::string>& args) = 0;
};

// concreate commands
class ReadCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
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
    bool execute(const std::vector<std::string>& args) override;
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

