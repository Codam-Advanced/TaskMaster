#pragma once

#include <utils/include/utils.hpp>

namespace ipc
{
class FileDescriptor
{
public:
    FileDescriptor();

    /**
     * @brief Construct a new FileDescriptor object with the given file descriptor.
     *
     * @param fd The file descriptor integer.
     */
    explicit FileDescriptor(i32 fd);
    virtual ~FileDescriptor();

    FileDescriptor(const FileDescriptor&)            = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&&) noexcept;
    FileDescriptor& operator=(FileDescriptor&&) noexcept;

    /**
     * @brief Get the underlying file descriptor.
     *
     * @return The file descriptor integer.
     */
    i32 getFd() const;

    void close();

protected:
    i32 _fd;
};
} // namespace taskmasterd
