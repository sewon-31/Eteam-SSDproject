#pragma once

#include<sstream>
#include<string>
#include<iostream>

#include "ssd_interface.h"

using namespace std;

class testScript {
public:
	virtual bool run(SSDInterface& ssd) = 0;
	virtual std::string getRandomIntToString() = 0;
	virtual bool readCompare(int address, std::string num, SSDInterface &ssd) = 0;
};

class testScript2 : public testScript {
public:
	bool run(SSDInterface& ssd) override;
	std::string getRandomIntToString() override;
	bool readCompare(int address, std::string num, SSDInterface &ssd) override;
};