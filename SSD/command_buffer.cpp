#include "command_buffer.h"
#include "ssd_command_builder.h"

#include <filesystem>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

CommandBuffer&
CommandBuffer::getInstance(const std::string& dirPath) {
	static CommandBuffer instance(dirPath);
	return instance;
}

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

void
CommandBuffer::addCommand(std::shared_ptr<ICommand> command)
{
	int bufSize = buffer.size();

	if (bufSize == 0) {
		addCommandToBuffer(command);
	}
	else if (bufSize == CommandBuffer::BUFFER_MAX) {
		flushBuffer();
		addCommandToBuffer(command);;
	}
	else {
		addCommandToBuffer(command);
		optimizeBuffer();
	}
}

void
CommandBuffer::clearBuffer()
{
	buffer.clear();
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
	clearBuffer();
}

void
CommandBuffer::addCommandToBuffer(std::shared_ptr<ICommand> command)
{
	buffer.push_back(command);
}

bool
CommandBuffer::optimizeBuffer()
{
    int buf_size = buffer.size();
    CMD_BUF in;
    CMD_BUF out;
    int in_buf_idx = 0;


    for (int buf_idx = 0; buf_idx < buf_size; buf_idx++) {
        auto cmd = buffer.at(buf_idx);
        auto type = cmd->getCmdType();

        std::cout << "optimizeBuffer : buffer_size" << " " << in.lba[buf_idx] << " " << in.data[buf_idx] << "\n";
        if (type == CmdType::WRITE) {
            std::shared_ptr<WriteCommand> wCmdPtr = std::dynamic_pointer_cast<WriteCommand>(cmd);

            in.op[in_buf_idx] = cmd->getCmdType();
            in.lba[in_buf_idx] = cmd->getLBA();
            in.data[in_buf_idx] = wCmdPtr->getValue();
            in.size[in_buf_idx] = 1;
            std::cout << "IN CMD -> WRITE" << " " << in.lba[in_buf_idx] << " " << in.data[in_buf_idx] << "\n";
            in_buf_idx++;
        }
        if (type == CmdType::ERASE) {
            std::shared_ptr<EraseCommand> eCmdPtr = std::dynamic_pointer_cast<EraseCommand>(cmd);
            in.op[in_buf_idx] = cmd->getCmdType();
            in.lba[in_buf_idx] = cmd->getLBA();
            in.size[in_buf_idx] = eCmdPtr->getSize();
            std::cout << "IN CMD -> ERASE" << " " << in.lba[in_buf_idx] << " " << in.size[in_buf_idx] << "\n";
            in_buf_idx++;
        }
    }
    std::cout << "\n";

    int new_buf_size = reduceCMDBuffer(in, out);
    if (new_buf_size < buf_size)
    {
        std::shared_ptr<SSDCommandBuilder> builder;

        if (!builder) {
            builder = std::make_shared<SSDCommandBuilder>();
        }

        buffer.clear();

        for (int buf_idx = 0; buf_idx < new_buf_size; buf_idx++) {
            if (out.op[buf_idx] == CmdType::WRITE) {
                vector<string> commandVector = { "W" , std::to_string(out.lba[buf_idx])  , out.data[buf_idx] };
                auto new_cmd = builder->createCommand(commandVector);
                buffer.push_back(new_cmd);
                std::cout << "OUT CMD -> WRITE" << " " << out.lba[buf_idx] << " " << out.data[buf_idx] << "\n";
            }
            else if (out.op[buf_idx] == CmdType::ERASE) {
                vector<string> commandVector = { "E" , std::to_string(out.lba[buf_idx])  , std::to_string(out.size[buf_idx]) };
                auto new_cmd = builder->createCommand(commandVector);
                buffer.push_back(new_cmd);
                std::cout << "OUT CMD -> ERASE" << " " << out.lba[buf_idx] << " " << out.size[buf_idx] << "\n";
            }
        }
    }
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
	// delete all the files
	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		if (fs::is_regular_file(file.path())) {
			fs::remove(file.path());
		}
	}

	if (buffer.size() > BUFFER_MAX) {
		std::cout << "vector size error" << std::endl;
	}

	// create cmd file
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
						filePath += std::to_string(eCmdPtr->getSize());
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

int
CommandBuffer::reduceCMDBuffer(CMD_BUF in, CMD_BUF& out) 
{
    int virtual_op[100];	// 9 == NULL, 7 = E, 0-5 = W 

    const int OP_NULL = 9;
    const int OP_E = 7;
    const int OP_W_MAX = 5;
    int cmdCount = 5;

    CMD_BUF temp;
    // 1. Replcae w iba "0x00000000" >  E iba
    // 2. make virtual data 
    // 3. Make CMD
        //3-1. Make new CMD E range check and W
        // display

#ifdef PRINT_DEBUG
    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        std::cout << static_cast<int>(in.op[idx_cb]) << " " << in.lba[idx_cb] << " " << in.data[idx_cb] << " " << in.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif

    // step - 2
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        virtual_op[idx_iba] = OP_NULL;
    }

    for (int idx_cb = 0; idx_cb < cmdCount; idx_cb++) {
        if (in.op[idx_cb] == CmdType::WRITE) {
            virtual_op[in.lba[idx_cb]] = idx_cb;
        }
        else if (in.op[idx_cb] == CmdType::ERASE) {
            for (int idx_size = 0; idx_size < in.size[idx_cb]; idx_size++)
                virtual_op[in.lba[idx_cb] + idx_size] = OP_E;
        }
    }
    // display
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        if (virtual_op[idx_iba] == OP_NULL)
            std::cout << "N" << " ";
        else if (virtual_op[idx_iba] == OP_E)
            std::cout << "E" << " ";
        else
            std::cout << "W" << " ";


        if (idx_iba % 10 == 9)
            std::cout << "\n";
    }

    // step - 3
    int continue_E_CMD = 0;
    int newCMDCount = 0;

    // step - 3-1
    for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
        // Check E and W
        if (virtual_op[idx_iba] != OP_NULL) {
            if (continue_E_CMD == 0) {
                if (virtual_op[idx_iba] == OP_E)
                    temp.op[newCMDCount] = CmdType::ERASE;
                else {
                    temp.op[newCMDCount] = CmdType::WRITE;
                    temp.data[newCMDCount] = in.data[virtual_op[idx_iba]];
                }
                temp.lba[newCMDCount] = idx_iba;
                temp.size[newCMDCount] = 1;
                //temp.data[newCMDCount] = in.data[virtual_op[idx_iba]];
                continue_E_CMD = 1;
            }
            else {
                temp.size[newCMDCount]++;
                continue_E_CMD++;
                if (continue_E_CMD == 10) {
                    continue_E_CMD = 0;
                    newCMDCount++;
                }
            }
            continue;
        }
        else if (continue_E_CMD > 0) {
            continue_E_CMD = 0;
            newCMDCount++;
        }
    }
#ifdef  PRINT_DEBUG
    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << static_cast<int>(temp.op[idx_cb]) << " " << temp.lba[idx_cb] << " " << temp.data[idx_cb] << " " << temp.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif

    int hit = 0;
    for (int idx_cb_in = 0; idx_cb_in < cmdCount; idx_cb_in++) {
        if (in.op[idx_cb_in] == CmdType::WRITE && in.data[idx_cb_in] != "0x00000000") {
            hit = 0;
            for (int idx_cb_out = 0; idx_cb_out < newCMDCount; idx_cb_out++) {
                if (temp.lba[idx_cb_out] == in.lba[idx_cb_in]) {
                    if (temp.size[idx_cb_out] == 1) {
                        hit = 1;
                    }
                }
            }
            if (!hit) {
                temp.op[newCMDCount] = CmdType::WRITE;
                temp.lba[newCMDCount] = in.lba[idx_cb_in];
                temp.size[newCMDCount] = 1;
                temp.data[newCMDCount] = in.data[idx_cb_in];
                newCMDCount++;
            }
        }
    }
#ifdef  PRINT_DEBUG
    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << static_cast<int>(temp.op[idx_cb]) << " " << temp.lba[idx_cb] << " " << temp.data[idx_cb] << " " << temp.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif
    int idx = 0;
    for (int idx_iba = 0; idx_iba < newCMDCount; idx_iba++) {
        if (temp.op[idx_iba] == CmdType::ERASE)
        {
            out.op[idx] = temp.op[idx_iba];
            out.lba[idx] = temp.lba[idx_iba];
            out.size[idx] = temp.size[idx_iba];
            out.data[idx] = temp.data[idx_iba];
            idx++;
        }
    }
    for (int idx_iba = 0; idx_iba < newCMDCount; idx_iba++) {
        if (temp.op[idx_iba] == CmdType::WRITE)
        {
            out.op[idx] = temp.op[idx_iba];
            out.lba[idx] = temp.lba[idx_iba];
            out.size[idx] = temp.size[idx_iba];
            out.data[idx] = temp.data[idx_iba];
            idx++;
        }
    }
#ifdef  PRINT_DEBUG
    if (idx != newCMDCount)
        std::cout << "Error idx " << idx << " " << newCMDCount << "\n";

    // display
    for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
        std::cout << static_cast<int>(out.op[idx_cb]) << " " << out.lba[idx_cb] << " " << out.data[idx_cb] << " " << out.size[idx_cb] << "\n";
    }
    std::cout << "\n";
#endif
    return newCMDCount;
}
