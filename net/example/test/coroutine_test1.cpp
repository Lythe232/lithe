#include "coroutine.h"

#include <iostream>
#include <list>
#include <memory>
#include <functional>

using namespace lithe;
bool g_flag = true;
int g_count = 0;
void func2()
{   
    printf("++++++++++\n");
    printf("++++++++++\n");
    printf("++++++++++\n");
    if(g_count > 50)
    {
        auto cor = Coroutine::getRunCo();
        cor->yield();
    }
    printf("++++++++++\n");
    printf("++++++++++\n");
}
void func1()
{
    auto cor = Coroutine::getRunCo();
    std::shared_ptr<Coroutine> cor1(new Coroutine(std::bind(func2)));

    printf("Main = %lx\n", cor.get());
    while(g_flag)
    {

        printf("__________%d\n", g_count++);
        printf("__________%d\n", g_count++);
        printf("__________%d\n", g_count++);
        printf("__________%d\n", g_count++);
        printf("__________%d\n", g_count++);
        if(cor1->getState() == Coroutine::END)
            cor1->reset(std::bind(func2));
        printf("(((((((((((\n");
        cor1->resume();
        printf(")))))))))))\n");
        if(g_count == 100)
        {
            g_flag = false;
        }
    }
    
}

int main()
{
    func1();
}