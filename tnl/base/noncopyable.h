#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

namespace tnl
{

class noncopyable
{   
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}  // namespace tnl

#endif  // _NONCOPYABLE_H_
