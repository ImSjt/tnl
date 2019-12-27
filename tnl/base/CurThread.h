#ifndef _CUR_THREAD_H_
#define _CUR_THREAD_H_

namespace tnl
{
namespace CurThread
{
    // __thread表示线程的局部变量
    extern __thread int mCachedTid;
    extern __thread char mTidString[32];
    extern __thread int mTidStringLen;

    // 缓存线程ID
    void cacheTid();

    // 获取当前线程Id
    inline int tid()
    {
        // __builtin_expect告诉编译器大多数情况下的结果
        if (__builtin_expect(mCachedTid == 0, 0))
        {
            cacheTid();
        }

        return mCachedTid;
    }

    inline const char* tidStr()
    {
        return mTidString;
    }

    inline int tidStrLen()
    {
        return mTidStringLen;
    }

} // namespace tnl
} // namespace CurThread

#endif // _CUR_THREAD_H_