//
// Created by lsmg on 5/23/20.
//

#include <mongo/base/Logger.h>

int main()
{
	LOG_INFO << "CONSOLE 1";

	mongo::Logger::SetLogPlace(mongo::Logger::FILE, "log_file", "/home/lsmg");

	LOG_INFO << "FILE 1";
	LOG_INFO << "FILE 2";
	LOG_INFO << "FILE 3";
}