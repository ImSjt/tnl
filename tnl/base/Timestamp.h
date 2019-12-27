#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <stdint.h>
#include <time.h>

namespace tnl
{

class Timestamp
{
public:
    Timestamp() :
        mMicroSecondsSinceEpoch(0)
    {
    }

    Timestamp(int64_t microSecondsSinceEpoch) :
        mMicroSecondsSinceEpoch(microSecondsSinceEpoch)
    {
    }

    int64_t microSecondsSinceEpoch() const { return mMicroSecondsSinceEpoch; }
    time_t secondsSinceEpoch() const
    { return static_cast<time_t>(mMicroSecondsSinceEpoch / MicroSecondsPerSecond); }

    static Timestamp now();

public:
    static const int MicroSecondsPerSecond = 1000 * 1000;

private:

    int64_t mMicroSecondsSinceEpoch;

};

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::MicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

inline bool operator<(const Timestamp& t1, const Timestamp& t2)
{
    return t1.microSecondsSinceEpoch() < t2.microSecondsSinceEpoch();
}

inline bool operator==(const Timestamp& t1, const Timestamp& t2)
{
    return t1.microSecondsSinceEpoch() == t2.microSecondsSinceEpoch();
}

} // namespace tnl

#endif // _TIMESTAMP_H_