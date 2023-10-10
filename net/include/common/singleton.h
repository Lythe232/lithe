#pragma once
#include "include/common/noncopyable.h"

#include <memory>
#include <assert.h>

namespace  lithe
{

// template<typename T, typename A>
// class Singleton
// {
// protected:
//     static T* singleton_;
// public:

//     Singleton()
//     {
//         assert(!singleton_);
//         singleton_ = static_cast<T*>(this);
//     }
//     ~Singleton(){ assert(singleton_); singleton_ = 0; }
//     static T* getSingletonPtr() { return (singleton_);}
//     static T& getSingleton() { return (*singleton_); }
// };
template<typename T>
class Singleton : public Noncopyable
{
public:
    static T&  getSingleton()
    {
        static T singleton;
        return singleton;
    }
private:
};

template<typename T>
class SingletonPtr : public Noncopyable
{
public:
    static T* getSingletonPtr()
    {
        static T singleton;
        return &singleton;
    }
};
template<typename T>
class SingletonSPtr : public Noncopyable
{
public:
    static std::shared_ptr<T> getSingletonPtr()
    {
        static std::shared_ptr<T> singleton;
        return singleton;
    }
};

}   //namespace lithe