#include "fileUtils.h"
#include <iostream>

using namespace lithe;
int main()
{
    AppendFile file("1.txt");
    file.append("123321");
}