#include "ssd_command_builder.h"
#include "file_interface.h"
#include "nand_data.h"
#include "command.h"

#include <memory>

using std::string;

class SSD
{
public:
	explicit SSD(
		const string& nandPath = "../ssd_nand.txt",
		const string& outputPath = "../ssd_output.txt");

	void run(vector<string> commandVector);
	bool updateOutputFile(const string& result);

	// for unit test
	string getData(int lba) const;
	void writeData(int lba, const string& value);
	void clearData();
	void setBuilder(std::shared_ptr<SSDCommandBuilder> builder);
	NandData& getStorage();

private:
	NandData& storage;
	FileInterface outputFile;
	std::shared_ptr<SSDCommandBuilder> builder;

	static const int nandFileSize = 1200;
};
