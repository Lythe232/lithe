#pragma once

#include <assert.h>


namespace  lithe
{

template<typename T>
class Singleton
{
protected:
    static T* singleton_;
public:

    Singleton()
    {
        assert(!singleton_);
        singleton_ = static_cast<T*>(this);
    }
    ~Singleton(){ assert(singleton_); singleton_ = 0; }
    static T* getSingletonPtr() { return (singleton_);}
    static T& getSingleton() { return (*singleton_); }
};


}   //namespace lithe