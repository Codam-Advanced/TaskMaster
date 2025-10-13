#pragma once

#include <utils/include/utils.hpp>

namespace taskmasterd
{
class FileDescriptor
{
public:
    FileDescriptor();
    explicit FileDescriptor(i32 fd);
    virtual ~FileDescriptor();

    FileDescriptor(const FileDescriptor&)            = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&&) noexcept;
    FileDescriptor& operator=(FileDescriptor&&) noexcept;

    i32 getFd() const;

    void close();

protected:
    i32 _fd;
};
} // namespace taskmasterd