#ifndef _FILE_H_
#define _FILE_H_
#include <string>

namespace tnl
{

class File
{
public:
    File();
    File(std::string file);
    ~File();

    bool open();
    bool close();
    int write(const char* msg, int len);
    bool flush();
    bool reset(std::string file);

private:
    std::string mFile;
    FILE* mFp;

};

} // namespace tnl

#endif // _FILE_H_