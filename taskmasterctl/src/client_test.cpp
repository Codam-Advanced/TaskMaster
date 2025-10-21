#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../../build/libs/ipc/proto/taskmaster.pb.h"

int main()
{
    proto::Command command;
    command.set_type(proto::CommandType::START);
    command.add_args("nginx");

    // Serialize the message to a string
    std::string serialized;
    if (!command.SerializeToString(&serialized)) {
        std::cerr << "Failed to serialize command." << std::endl;
        return 1;
    }

    // Create a UNIX domain socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set up the UNIX socket address
    struct sockaddr_un addr;
    addr.sun_family         = AF_UNIX;
    std::string socket_path = "/tmp/taskmasterd.sock";
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "Failed to connect to server." << std::endl;
        close(sockfd);
        return 1;
    }

    // Send the size of the message first
    uint32_t size = htonl(serialized.size());
    if (send(sockfd, &size, sizeof(size), 0) == -1) {
        std::cerr << "Failed to send message size." << std::endl;
        close(sockfd);
        return 1;
    }

    // Send the serialized message
    if (send(sockfd, serialized.data(), serialized.size(), 0) == -1) {
        std::cerr << "Failed to send message." << std::endl;
        close(sockfd);
        return 1;
    }
    std::cout << "Command sent successfully." << std::endl;

    close(sockfd);

    return 0;
}
