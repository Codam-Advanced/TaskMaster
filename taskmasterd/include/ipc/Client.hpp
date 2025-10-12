#pragma once

#include <taskmasterd/include/core/Socket.hpp>
#include <taskmasterd/include/ipc/ProtoReader.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Client : public ProtoReader<proto::Command>
{
public:
    /** Construct a new Client object.
     *
     * @param socket The connected socket representing the client.
     */
    Client(Socket&& socket);

    /** Handle a complete protobuf Command message.
     * This method is called when a complete Command message is received from the client.
     *
     * @param command The complete Command message received.
     */
    void handleMessage(proto::Command command) override;

    /** Check if the client is still connected.
     *
     * @return true if the client is connected, false otherwise.
     */
    bool isConnected() const { return _fd != -1; }
};
} // namespace taskmasterd
