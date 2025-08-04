#pragma once
#include <iostream>

class ICommandExecutor {
public:
    ICommandExecutor() = default;
    virtual ~ICommandExecutor() = default;
    virtual void execute() = 0;
};