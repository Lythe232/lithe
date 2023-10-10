#pragma once

#include <string>

namespace FileUtils
{

class ReadSmallFile
{

};

class AppendFile
{
public:
    AppendFile() {};
    AppendFile(std::string filename);

    ~AppendFile();

    void flush();

    void append(std::string logline);
    void append(const char* logline, int len);
    off_t writtenBytes() const { return writtenBytes_; }
private:
    size_t write(const char* logline, size_t len);

    char buffer_[64 * 1024];
    FILE* fp_;
    off_t writtenBytes_;
};


}