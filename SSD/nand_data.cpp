#include "nand_data.h"
#include <vector>

NandData&
NandData::getInstance(const std::string& path) {
	static NandData instance(path);
	return instance;
}

NandData::NandData(const string& filePath)
	: filePath(filePath) { }

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
	std::vector<std::string> lines;
	if (!FileInterface::fileExists(filePath) || FileInterface::getFileSize(filePath) == 0) {
		clear();
		return false;
	}

	if (!FileInterface::readAllLines(filePath, lines) || lines.size() != LBA::SIZE) {
		clear();
		return false;
	}

	for (int i = LBA::MIN; i <= LBA::MAX; ++i) {
		data[i] = lines[i];
	}

	return true;
}

bool
NandData::updateToFile()
{
	std::vector<std::string> lines(data, data + LBA::SIZE);
	return FileInterface::writeAllLines(filePath, lines, /*append=*/false);
}

bool
NandData::isInvalidLBA(int lba) const
{
	return lba < LBA::MIN || lba > LBA::MAX;
}
