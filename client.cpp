#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>


using namespace std;

int const BUFFER_SIZE = 8192;
string const SOCKET_FILE = "/tmp/OS_net_descriptor_passing";

int main() {
    char buffer[CMSG_SPACE(sizeof(int))], data[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    struct iovec IO{};
    IO.iov_base = &data;
    IO.iov_len = sizeof(data);

    struct msghdr msg{};
    msg.msg_iov = &IO;
    msg.msg_iovlen = 1;
    msg.msg_control = buffer;
    msg.msg_controllen = sizeof(buffer);

    int fileDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fileDescriptor == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un client{};
    client.sun_family = AF_UNIX;
    memcpy(client.sun_path, SOCKET_FILE.c_str(), SOCKET_FILE.size());

    if (connect(fileDescriptor, (sockaddr * )(&client), sizeof(client)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }


    if (recvmsg(fileDescriptor, &msg, 0)) {
        perror("recvmag");
        exit(EXIT_FAILURE);
    }

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);


    string message;
    cout << "Enter :q to quit" << endl;

    dup2(*CMSG_DATA(cmsg), 1);
    bool working  = true;
    while (working) {
        cout << "Enter message ";
        getline(cin, message);
        if (message == ":q") {
            working = false;
            continue;
        }
        cout << message << endl;
    }
    return 0;
}