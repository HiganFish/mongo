//
// Created by lsmg on 5/23/20.
//

#include <cstring>
#include "LogFile.h"
#include "Timestamp.h"
#include "unistd.h"
#include "Logger.h"

using namespace mongo;

const int LogFile::LOG_INTERVAL = 5;

LogFile::LogFile(const std::string& prefix_name, const std::string& file_dir) :
out_fp_(nullptr)
{
	std::string file_name = file_dir;

	if (file_name.empty())
	{
		char path[128];
		getcwd(path, sizeof path);
		file_name.append(path);
	}

	file_name += '/';

	if (!prefix_name.empty())
	{
		file_name += prefix_name;
		file_name += '-';
	}

	file_name += GetFileName();
	out_fp_ = fopen(file_name.c_str(), "a");
}
LogFile::~LogFile()
{
	Flush();
}
void LogFile::Append(const char* str, size_t len)
{
	if (out_fp_ == nullptr)
	{
		return;
	}

	TryFlush(len);

	memcpy(&buffer[write_idx_], str, len);
	write_idx_ += len;
}

bool LogFile::BufferEnough(size_t len)
{
	return BUFFER_SIZE - write_idx_ - len > 0;
}

void LogFile::TryFlush(size_t len)
{
	if (TimeUpFlush())
	{
		Flush();
	}
	else if (BufferEnough(len))
	{
		Flush();
	}
}
std::string LogFile::GetFileName()
{
	std::string file_name;

	Timestamp stamp(Timestamp::Now());
	file_name += stamp.ToSecMsec();
	file_name += ".log";

	return file_name;
}
bool LogFile::TimeUpFlush()
{
	Timestamp stamp(Timestamp::Now());
	bool write_once = (stamp - last_log_time_).GetSec() > LOG_INTERVAL;
	if (write_once)
	{
		last_log_time_ = stamp;
	}

	return write_once;
}

void LogFile::Flush()
{
	size_t written_bytes = 0;
	while (written_bytes < write_idx_)
	{
		size_t write_size = fwrite(&buffer[written_bytes], 1, write_idx_ - written_bytes, out_fp_);

		LOG_FATAL_IF(write_size < 0) << "fwite log error";

		written_bytes += write_size;
	}

	fflush(out_fp_);
	write_idx_ = 0;
}
