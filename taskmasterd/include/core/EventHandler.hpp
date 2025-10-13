#pragma once

#include <taskmasterd/include/core/FileDescriptor.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
enum class EventType : u32
{
    READ  = 0x01,
    WRITE = 0x04,
};

class EventHandler : public FileDescriptor
{
public:
    /**
     * @brief Construct a new EventHandler object.
     *
     * @param fd The file descriptor to be monitored for events.
     */
    EventHandler(i32 fd) : FileDescriptor(fd) {}
    virtual ~EventHandler();

    /**
     * @brief Handle read events.
     *
     * This method is called when a read event occurs on the associated file descriptor.
     * Subclasses should override this method to implement custom read handling logic.
     */
    virtual void handleRead() {}

    /**
     * @brief Handle write events.
     *
     * This method is called when a write event occurs on the associated file descriptor.
     * Subclasses should override this method to implement custom write handling logic.
     */
    virtual void handleWrite() {}
};
} // namespace taskmasterd
