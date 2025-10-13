#include <taskmasterd/include/core/FileDescriptor.hpp>

namespace taskmasterd
{
FileDescriptor::FileDescriptor() : _fd(-1) {}

FileDescriptor::FileDescriptor(i32 fd) : _fd(fd) {}

FileDescriptor::~FileDescriptor()
{
    if (_fd != -1) {
        ::close(_fd);
    }
}

FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept : _fd(other._fd)
{
    other._fd = -1;
}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept
{
    if (this != &other) {
        this->close();
        _fd       = other._fd;
        other._fd = -1;
    }
    return *this;
}

i32 FileDescriptor::getFd() const
{
    return _fd;
}

void FileDescriptor::close()
{
    if (_fd != -1) {
        ::close(_fd);
        _fd = -1;
    }
}
} // namespace taskmasterd