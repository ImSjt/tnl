#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <pthread.h>

#include "tnl/base/noncopyable.h"

namespace tnl
{

template<typename T>
class Singleton : noncopyable
{
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T& instance()
    {
        pthread_once(&mPOnce, &Singleton::init);
        assert(mValue != NULL);
        return *mValue;
    }

private:
    static void init()
    {
        mValue = new T();
    }

private:
    static pthread_once_t mPOnce;
    static T*             mValue;
};

template<typename T>
pthread_once_t Singleton<T>::mPOnce = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::mValue = NULL;

} // namespace tnl

#endif // _SINGLETON_H_