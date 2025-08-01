#include "command_buffer.h"
#include "ssd_command_builder.h"

#include <filesystem>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

CommandBuffer::CommandBuffer(const string& dirPath)
    : bufferDirPath(dirPath)
{
}

void
CommandBuffer::Init()
{
    initDirectory();
    updateFromDirectory();
}

int
CommandBuffer::getBufferSize() const
{
    return buffer.size();
}

const std::vector<std::shared_ptr<ICommand>>&
CommandBuffer::getBuffer() const
{
    return buffer;
}

int
CommandBuffer::addCommand(std::shared_ptr<ICommand> command)
{
    buffer.push_back(command);
    optimizeBuffer();

    return buffer.size();
}

void
CommandBuffer::flushBuffer()
{
    NandData::getInstance().updateFromFile();

    string result;
    for (auto cmd : buffer) {
        cmd->execute(result);
    }

    NandData::getInstance().updateToFile();
    buffer.clear();
}

bool
CommandBuffer::optimizeBuffer()
{
    //std::cout << "optimizeBuffer\n";
#if 0
    int buf_size = buffer.size();
    auto cmd = buffer.at(0);
    auto type = cmd->getCmdType();
    string lbaStr = std::to_string(cmd->getLBA());

    std::cout << "optimizeBuffer :" << buf_size << "\n";

    std::cout << "buf_size :" << buf_size << "\n";


    if (type == CmdType::WRITE) {
        std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);

    }
    if (type == CmdType::ERASE) {
        std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);

    }
    //cmd->getLBA
        //cmd->getLBA
#endif
    return true;
}

void
CommandBuffer::initDirectory()
{
    // create buffer directory (if needed)
    bool bufferDirExists = fs::exists(bufferDirPath) && fs::is_directory(bufferDirPath);

    if (!bufferDirExists) {
        if (fs::create_directory(bufferDirPath)) {
            for (int i = 1; i <= BUFFER_MAX; ++i) {
                string filePath = bufferDirPath + "/" + std::to_string(i) + "_empty";
                std::ofstream file(filePath);

                if (!file) {
#if _DEBUG
                    std::cerr << "Failed to create " << filePath << std::endl;
#endif
                }
            }
        }
    }
}

void
CommandBuffer::updateFromDirectory()
{
    // read file names from directory
    vector<string> fileNames;

    for (const auto& file : fs::directory_iterator(bufferDirPath)) {
        if (fs::is_regular_file(file.path())) {
            fileNames.push_back(file.path().filename().string());
        }
    }

#if _DEBUG
    if (fileNames.size() != BUFFER_MAX) {
        std::cout << "Why not 5 files?\n";
    }
#endif

    // sort filenames
    std::sort(fileNames.begin(), fileNames.end());

    SSDCommandBuilder builder;

    auto startsWith = [](const string& str, const string& prefix) -> bool {
        return str.compare(0, prefix.size(), prefix) == 0;
        };

    int fileNum = 1;
    for (const auto& fileName : std::as_const(fileNames)) {
        string prefix = std::to_string(fileNum++) + "_";
        if (startsWith(fileName, prefix)) {
            string noPrefix = fileName.substr(prefix.length());

            if (noPrefix == EMPTY) {
                break;
            }

            vector<string> commandVector;
            std::stringstream ss(noPrefix);

            string token;
            while (std::getline(ss, token, '_')) {
                commandVector.push_back(token);
            }

            auto cmd = builder.createCommand(commandVector);
            if (cmd == nullptr) {
                std::cout << "Commmand not created - " << noPrefix << std::endl;
            }
            buffer.push_back(cmd);

        }
        else {
            std::cout << "Weird file " << fileName << std::endl;
        }
    }

    // delete all the files
    for (const auto& file : fs::directory_iterator(bufferDirPath)) {
        if (fs::is_regular_file(file.path())) {
            fs::remove(file.path());
        }
    }
}

void
CommandBuffer::updateToDirectory()
{
    if (buffer.size() > BUFFER_MAX) {
        std::cout << "vector size error" << std::endl;
    }
    if (buffer.size() > 0) {
        for (int i = 1; i <= buffer.size(); ++i) {
            string filePath = bufferDirPath + "/" + std::to_string(i) + "_";
            auto cmd = buffer.at(i - 1);
            if (cmd == nullptr) {
                filePath += EMPTY;
            }
            else {
                // concat type, lba, value/size
                auto type = cmd->getCmdType();
                string lbaStr = std::to_string(cmd->getLBA());

                if (type == CmdType::WRITE) {
                    filePath += "W_" + lbaStr + "_";

                    std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);
                    if (wCmdPtr) {
                        filePath += wCmdPtr->getValue();
                    }
                }
                if (type == CmdType::ERASE) {
                    filePath += "E_" + lbaStr + "_";
                    std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
                    if (eCmdPtr) {
                        filePath += eCmdPtr->getSize();
                    }
                }
            }

            // create file
            std::ofstream file(filePath);
            if (!file) {
#if _DEBUG
                std::cout << "Failed to create " << filePath << std::endl;
#endif
            }
        }
    }
    for (int i = buffer.size() + 1; i <= BUFFER_MAX; ++i) {
        string filePath = bufferDirPath + "/" + std::to_string(i) + "_" + EMPTY;

        // create file
        std::ofstream file(filePath);
        if (!file) {
#if _DEBUG
            std::cout << "Failed to create " << filePath << std::endl;
#endif
        }
    }
}