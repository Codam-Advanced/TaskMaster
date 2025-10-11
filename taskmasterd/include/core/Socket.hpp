#pragma once

#include <taskmasterd/include/core/Address.hpp>
#include <taskmasterd/include/core/EventHandler.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Socket : public EventHandler
{
public:
    enum class Type
    {
        TCP,
        UDP,
        UNIX
    };

    Socket(Type type);

    void bind(const Address& address);
    void listen(i32 backlog = 5);
};
} // namespace taskmasterd
