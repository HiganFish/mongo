//
// Created by lsmg on 4/9/20.
//

#include <cassert>
#include <memory>

#include "LogFile.h"
#include "mongo/base/Logger.h"
#include "Thread.h"

namespace mongo
{
// LEARN 缓存上一次输出的秒 同一秒输出时避免多次格式化
__thread char t_time[64];
__thread time_t t_last_second;

// LEARN 处处保持长度一致 使用NUM_LOG_LEVELS表示大小
const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

// LEARN 临时包装字符串指针和长度
class T
{
public:

    T(const char* str, size_t len):
    str_(str),
    len_(len)
    {
        assert(strlen(str) == len);
    }

    const char* str_;
    const size_t len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
    s.Append(v.str_, v.len_);
    return s;
}
}

using namespace mongo;


Logger::LogPlace Logger::place_ = CONSOLE;
std::unique_ptr<LogFile> Logger::log_file_;

Logger::Logger(LogLevel level, SourceFile file, int line, const char* func):
impl_(level, file, line)
{
    impl_.stream_ << func << ' ';
}

Logger::Logger(Logger::LogLevel level, Logger::SourceFile file, int line):
impl_(level, file, line)
{

}

Logger::~Logger()
{
    // LEARN 使用析构函数输出每行结束
    impl_.Finish();

    const LogBuffer& buffer(GetStream().GetBuffer());

    if (place_ == CONSOLE)
	{
		fwrite(buffer.GetData(), 1, buffer.Length(), stdout);
	}
    else if (place_ == FILE)
	{
    	if (log_file_)
		{
    		log_file_->Append(buffer.GetData(), buffer.Length());
		}
	}

    if (impl_.level_ == FATAL)
    {
        fflush(stdout);
        abort();
    }
}

Logger::LogLevel g_log_level = Logger::INFO;

void Logger::SetLogLevel(Logger::LogLevel level)
{
    g_log_level = level;
}

void Logger::SetLogPlace(Logger::LogPlace place, const std::string& file_prefix, const std::string& file_dir)
{
	if (place == FILE)
	{
		log_file_ = std::make_unique<LogFile>(file_prefix, file_dir);
		place_ = FILE;
	}
}

Logger::Impl::Impl(LogLevel level, const SourceFile& file, int line):
time_(Timestamp::Now()),
level_(level),
filename_(file),
line_(line)
{
    FormatTime();
    CurrentThread::Tid();
    stream_ << T(CurrentThread::TidString(), CurrentThread::TidStringLength());
    stream_ << T(LogLevelName[level_], 6);
}
void Logger::Impl::FormatTime()
{
    // 20200409 19:21:35.121221Z

    int64_t us_since_create = time_.GetUsSinceCreate();
    time_t seconds = static_cast<time_t>(us_since_create / Timestamp::US_PER_SECOND);
    int us = static_cast<int>(us_since_create % Timestamp::US_PER_SECOND);
    if (seconds != t_last_second)
    {
        t_last_second = seconds;

        // CHANGE 简化时区操作
        // from utc to utc+8
        seconds += 8 * 60 * 60;

        struct tm tm_time;
        ::gmtime_r(&seconds, &tm_time);

        // 20200409 19:21:35
        int len = snprintf(t_time, sizeof t_time, "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
    }

    // .121221Z
    Fmt us_fmt(".%06d ", us);
    assert(us_fmt.GetLength() == 8);
    stream_ << T(t_time, 17) << T(us_fmt.GetData(), 8);
}
void Logger::Impl::Finish()
{
    // CHANGE 去掉一个 SourceFile运算符 重载
    stream_ << " - " << T(filename_.data_, filename_.size_) << ":" << line_ << '\n';
}
