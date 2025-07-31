#include "nand_data.h"

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
NandData::isInvalidLBA(int lba) const
{
	return lba<MIN || lba>MAX;
}