#pragma once
#include <vector>
#include <string>
#include <memory>

class Command {
public:
    virtual ~Command() = default;
    // ��ȯ���� TestShell�� ��� �������(true) �������(false)�� ��Ÿ���ϴ�.
    virtual bool execute(const std::vector<std::string>& args) = 0;
};


// concreate commands
class ReadCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
};
class WriteCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override;
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

