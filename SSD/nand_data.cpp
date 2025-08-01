#include "nand_data.h"

NandData& 
NandData::getInstance(const std::string& path) {
	static NandData instance(path);
	return instance;
}

string
NandData::read(int lba) const
{
	if (isInvalidLBA(lba)) return "";

	return data[lba];
}

void
NandData::write(int lba, const string& value)
{
	if (isInvalidLBA(lba)) return;

	data[lba] = value;
}

void
NandData::erase(int startLBA, int endLBA)
{
	if (isInvalidLBA(startLBA)) return;
	if (isInvalidLBA(endLBA)) return;

	std::fill(data + startLBA, data + endLBA + 1, "0x00000000");
}

void
NandData::clear()
{
	std::fill(std::begin(data), std::end(data), "0x00000000");
}

bool
NandData::updateFromFile()
{
	file.fileOpen();

	if (file.checkSize() != 1200) {
		return false;
	}

	bool ret;
	for (int i = 0; i <= NandData::LBA::MAX; i++)
	{
		string data;
		ret = file.fileReadOneline(data);
		write(i, data);

		if (!ret) {
			break;
		}
	}

	file.fileClose();
	return ret;
}

bool
NandData::updateToFile()
{
	file.fileClear();
	file.fileOpen();

	bool ret;
	for (int i = NandData::LBA::MIN; i <= NandData::LBA::MAX; i++) {
		ret = file.fileWriteOneline(read(i));

		if (!ret) {
			break;
		}
	}

	file.fileClose();
	return ret;
}

NandData::NandData(const string& filePath)
	: file(filePath)
{
}

bool
NandData::isInvalidLBA(int lba) const
{
	return lba<MIN || lba>MAX;
}