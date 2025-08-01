#pragma once
#include "command.h"

// command Factory
class CommandFactory {
public:
    static std::unique_ptr<Command> createCommand(const std::vector<std::string>& commandVector, SSDInterface* ssd);
    static std::unique_ptr<Command> createScriptCommand(const std::vector<std::string>& commandVector, SSDInterface* ssd);
};