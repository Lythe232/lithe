#pragma once

#include <unistd.h>
#include <memory>

static size_t s_AllocSize = 0;
static size_t s_DeleteSize = 0;


void* operator new(size_t size)
{
    s_AllocSize += size;
    return malloc(size);
}
void operator delete(void* p, size_t size)
{
    s_DeleteSize += size;
    free(p);
}
static void printMemoryUsage()
{
    printf("Alloc memory size: %ld\nDelete memory size: %ld\n", s_AllocSize, s_DeleteSize);
}