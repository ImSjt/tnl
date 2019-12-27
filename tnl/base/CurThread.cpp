#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <type_traits>

#include "tnl/base/CurThread.h"

// 断言pid_t必须是int类型
static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

namespace tnl
{
namespace CurThread
{
__thread int mCachedTid = 0;
__thread char mTidString[32];
__thread int mTidStringLen;

// 缓存线程ID
void cacheTid()
{
  if (mCachedTid == 0)
  {
      mCachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
      mTidStringLen = snprintf(mTidString, sizeof(mTidString), "%5d ", mCachedTid);
  }
}

} // namespace CurThread 
} // namespace tnl