#pragma once

#include "proto/taskmaster.pb.h"
#include <ipc/include/ProtoReader.hpp>
#include <ipc/include/ProtoWriter.hpp>
#include <ipc/include/Socket.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{

class Server;

class Client : ipc::Socket // : public ProtoReader<proto::Command>
{
public:
    /**
     * @brief Construct a new Client object.
     *
     * @param socket The connected socket representing the client.
     */
    Client(ipc::Socket&& socket, Server& server);
    virtual ~Client();

    void handleRead();
    void handleWrite(proto::Command command);

    /**
     * @brief Handle a complete protobuf Command message.
     *
     * This method is called when a complete Command message is received from the client.
     *
     * @param command The complete Command message received.
     */
    void handleMessage(proto::Command command);

    /**
     * @brief Check if the client is still connected.
     *
     * @return true if the client is connected, false otherwise.
     */
    bool isConnected() const { return _fd != -1; }

private:
    ipc::ProtoReader<proto::Command>         _proto_reader;
    ipc::ProtoWriter<proto::CommandResponse> _proto_writer;

    Server& _server;
};
} // namespace taskmasterd
