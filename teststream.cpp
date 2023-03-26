#include "logStream.h"
#include <iostream>
using namespace lithe;
int main()
{
    LogStream logstream;
    FILE* file = fopen("2.txt", "ae");
    for(int i = 0; i < 1000; i ++)
    {
        logstream << "hello" << 1 << 2 << 3.33;
    }
        fwrite(logstream.buffer().data(), 1, (logstream.buffer().current() - logstream.buffer().data()), file);

    std::cout << logstream.buffer().data() << std::endl;

}