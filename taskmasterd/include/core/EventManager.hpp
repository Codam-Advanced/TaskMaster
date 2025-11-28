#pragma once

#include <functional>

#include <ipc/include/FileDescriptor.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class EventManager : public ipc::FileDescriptor
{
public:
    using EventCallback = std::function<void()>;

    /**
     * @brief Construct a new EventManager object.
     *
     * Initializes the epoll instance for event monitoring.
     *
     */
    EventManager();

    /**
     * @brief Register an event handler for a file descriptor.
     *
     * @param handler The FileDescriptor to monitor.
     * @param read_callback The callback function to invoke on read events.
     * @param write_callback The callback function to invoke on write events.
     */
    void registerEvent(const FileDescriptor& handler, EventCallback read_callback = nullptr, EventCallback write_callback = nullptr);

    /**
     * @brief Update the event handler for a file descriptor.
     * @param handler The FileDescriptor to update.
     * @param read_callback The new callback function for read events.
     * @param write_callback The new callback function for write events.
     */
    void updateEvent(const FileDescriptor& handler, EventCallback read_callback, EventCallback write_callback);

    /**
     * @brief Unregister an event handler for a file descriptor.
     * @param handler The FileDescriptor to stop monitoring.
     */
    void unregisterEvent(const ipc::FileDescriptor& handler);

    /**
     * @brief Wait for events and dispatch them to the appropriate handlers.
     *
     * This method blocks until one or more events occur, then calls the
     * corresponding handleRead or handleWrite methods on the registered
     * EventHandler instances.
     *
     */
    void handleEvents();

    /**
     * @brief Get the singleton instance of EventManager.
     *
     * @return The singleton instance.
     */
    static EventManager& getInstance();

private:
    using EventCallbackMap = std::unordered_map<i32, EventCallback>;

    void updateEventInternal(const FileDescriptor& handler, i32 operation, EventCallback read_callback, EventCallback write_callback);

    const static i32 MAX_EVENTS = 1024;

    EventCallbackMap _read_callbacks;
    EventCallbackMap _write_callbacks;
};
} // namespace taskmasterd
