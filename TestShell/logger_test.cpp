#include "gmock/gmock.h"
#include "logger.h"
#include <stdarg.h>
#include <regex>

class LoggerTestFixture :public testing::Test {
protected:
	Logger& logger = Logger::getInstance();
	bool checkFormat(const std::string& output, const std::string& msg) {
		std::regex logFormatRegex(
			R"(\[\d{2}\.\d{2}\.\d{2} \d{2}:\d{2}\] )"  // [��¥ �ð�]
			R"(.{30})"                                 // ��Ȯ�� 30�� �Լ��� ����
			R"( : )" + msg                             // �ݷ� �� �޽���
		);
		std::cout << output << std::endl;
		return std::regex_search(output, logFormatRegex);
	}
	void SetUp() override {
		logger.setConsoleOutput(true);
	}
	void TearDown() override {
		logger.setConsoleOutput(false);
	}
};

TEST_F(LoggerTestFixture, LogInteger) {
	testing::internal::CaptureStdout();
	logger.log("LoggerTest.TestLogInteger()", "Integer: %d", 42);
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(checkFormat(output, "Integer: 42"));
}

TEST_F(LoggerTestFixture, LogString) {
	testing::internal::CaptureStdout();
	logger.log("LoggerTest.PrintString()", "String: hello");
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(checkFormat(output, "String: hello"));
}