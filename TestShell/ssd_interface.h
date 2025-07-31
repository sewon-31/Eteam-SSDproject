#pragma once
#include <iostream>
#include <fstream>
#include "gmock/gmock.h"
#include <windows.h>
#include <string>

using std::string;

class SSDInterface {
public:
    virtual void write(int lba, string value) = 0;
    virtual string read(int lba) = 0;
    virtual void erase(int lba, int size) = 0;
    virtual void flush() = 0;
};

class SSDDriver : public SSDInterface {
public:
    virtual bool runExe(const string& command);
    void write(int lba, string value) override;
    string read(int lba) override;
    void erase(int lba, int size) override;
    void flush() override;

private:
    const string SSD_READ_RESULT = "../ssd_output.txt";
};

class SSDExecutionException : public std::exception {
public:
    explicit SSDExecutionException(const std::string& msg)
        : message_(msg) {
    }

    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};
