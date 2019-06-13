#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using namespace std;


int const BUFFER_SIZE = 8192;
string const SOCKET_FILE = "/tmp/OS_net_descriptor_passing";


int main() {
    cout << "Starting.. " << endl;

    char buffer[CMSG_SPACE(sizeof(int))], data[BUFFER_SIZE];

    int fileDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fileDescriptor == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server{};
    server.sun_family = AF_UNIX;
    memcpy(server.sun_path, SOCKET_FILE.c_str(), SOCKET_FILE.size());
    if((bind(fileDescriptor, (sockaddr *) (&server), sizeof(server))) == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if((listen(fileDescriptor, SOMAXCONN)) == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_FILE.c_str());

    bool working = true;
    while(working) {
        int clientDescriptor = accept(fileDescriptor, nullptr, nullptr);
        std::cout << "client with descriptor " << clientDescriptor << " connected" << endl;


        if(clientDescriptor == -1){
            perror("listen");
            exit(EXIT_FAILURE);
        }

        struct msghdr msg{};
        struct cmsghdr *cmsg;
        memset(buffer, 0, BUFFER_SIZE);

        struct iovec IO{};
        IO.iov_base = &data;
        IO.iov_len = sizeof(data);

        msg.msg_iov = &IO;
        msg.msg_iovlen = 1;
        msg.msg_control = buffer;
        msg.msg_controllen = sizeof (buffer);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));

        if ((sendmsg(clientDescriptor, &msg, 0) == -1)){
            perror("sendmsg");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}