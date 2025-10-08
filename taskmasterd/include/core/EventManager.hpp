#pragma once

#include <memory>

#include <taskmasterd/include/core/EventHandler.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class EventManager
{
public:
    EventManager();
    virtual ~EventManager();

    /**
     * @brief Register an event handler for a specific event type.
     *
     * @param handler Pointer to the EventHandler instance.
     * @param type The type of event to register (READ or WRITE).
     *
     */
    void registerEvent(EventHandler* handler, EventType type);

    /**
     * @brief Update the event type for an existing event handler.
     *
     * @param handler Pointer to the EventHandler instance.
     * @param type The new type of event to register (READ or WRITE).
     *
     */
    void updateEvent(EventHandler* handler, EventType type);

    /**
     * @brief Unregister an event handler.
     *
     * @param handler Pointer to the EventHandler instance to unregister.
     *
     */
    void unregisterEvent(EventHandler* handler);

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
     * @brief Initialize the singleton instance of EventManager.
     *
     * This method must be called before using getInstance().
     * It creates a single instance of EventManager.
     *
     */
    static void initialize();

    /**
     * @brief Get the singleton instance of EventManager.
     *
     * @return std::unique_ptr<EventManager>& Reference to the singleton instance.
     */
    static std::unique_ptr<EventManager>& getInstance();

private:
    i32 _epoll_fd;

    const static i32 MAX_EVENTS = 10;
};
} // namespace taskmasterd
