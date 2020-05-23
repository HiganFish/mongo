//
// Created by lsmg on 5/23/20.
//

#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include <cstring>
#include <cstdio>
#include <string>

namespace mongo
{
class LogFile
{
public:
	LogFile(const std::string& prefix_name, const std::string& file_dir);
	~LogFile();

	void Append(const char* str, size_t len);

private:

	const static int BUFFER_SIZE = 64 * 1024;

	char buffer[BUFFER_SIZE];
	size_t write_idx_;

	FILE* out_fp_;

	bool BufferEnough(size_t len);

	void Flush();

	std::string GetFileName();
};
}

#endif //_LOGFILE_H_
