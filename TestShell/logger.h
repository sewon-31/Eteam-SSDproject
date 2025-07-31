#pragma once
class Logger {
public:
    static Logger& getInstance();
    void log(const char* fullFuncSig, const char* fmt, ...);

private:
    Logger() = default;
};