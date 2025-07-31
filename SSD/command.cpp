#include "command.h"

// BaseCommand
BaseCommand::BaseCommand(NandData& storage, int lba)
    : storage(storage), lba(lba) 
{
}

// ReadCommand
void
ReadCommand::execute()
{
    storage.clear();

    // readNandFile()
    FileInterface nandFile = { "../ssd_nand.txt" };
    nandFile.fileOpen();

    bool ret;
    if (nandFile.checkSize() == 1200) {
        for (int i = 0; i <= NandData::LBA::MAX; i++)
        {
            string data;
            ret = nandFile.fileReadOneline(data);
            storage.write(i, data);

            if (!ret)  break;
        }
    }
    nandFile.fileClose();
    
    string result = storage.read(lba);
    
    // writeOutputFile(result);
    FileInterface outputFile = { "../ssd_output.txt" };

    outputFile.fileClear();
    outputFile.fileOpen();
    ret = outputFile.fileWriteOneline(result);
    outputFile.fileClose();
}

// WriteCommand
WriteCommand::WriteCommand(NandData& storage, int lba, const std::string& value)
    : BaseCommand(storage, lba), value(value) 
{
}

void
WriteCommand::execute()
{
    storage.clear();

    // readNandFile()
    FileInterface nandFile = { "../ssd_nand.txt" };
    nandFile.fileOpen();

    bool ret;
    if (nandFile.checkSize() == 1200) {
        for (int i = NandData::LBA::MIN; i <= NandData::LBA::MAX; i++)
        {
            string data;
            ret = nandFile.fileReadOneline(data);
            storage.write(i, data);

            if (!ret)  break;
        }
    }
    nandFile.fileClose();

    storage.write(lba, value);

    // writeNandFile();
    nandFile.fileClear();
    nandFile.fileOpen();
    for (int i = NandData::LBA::MIN; i <= NandData::LBA::MAX; i++) {
        if (!nandFile.fileWriteOneline(storage.read(i))) {
            break;
        }
    }
    nandFile.fileClose();
}

// EraseCommand
EraseCommand::EraseCommand(NandData& storage, int lba, int size)
    : BaseCommand(storage, lba), size(size) 
{
}

void
EraseCommand::execute()
{
    // storage.erase();
}