#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOCAL_IP "192.168.1.2"
#define REMOTE_IP "192.168.1.11"
#define REMOTE_PORT 7676
#define LOCAL_PORT 7676
#define MAX_BUFFER_SIZE 1024

int main() {
    // 创建发送端的socket
    int send_sock = socket(AF_INET, SOCK_DGRAM, 0);

    // 设置发送端的地址
    sockaddr_in send_addr;
    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);
    send_addr.sin_port = htons(REMOTE_PORT);

    // 发送数据
    const char* payload = "Hello, this is a test message.";
    sendto(send_sock, payload, strlen(payload), 0, (struct sockaddr*)&send_addr, sizeof(send_addr));

    // 创建接收端的socket
    int recv_sock = socket(AF_INET, SOCK_DGRAM, 0);

    // 设置接收端的地址
    sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(LOCAL_IP);
    recv_addr.sin_port = htons(LOCAL_PORT);

    // 绑定接收端的地址
    bind(recv_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr));

    // 接收数据
    char buffer[MAX_BUFFER_SIZE];
    ssize_t recv_len = recvfrom(recv_sock, buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);

    // 打印接收到的数据
    if (recv_len > 0) {
        std::cout << "Received: " << std::string(buffer, recv_len) << std::endl;
    }

    // 关闭socket
    close(send_sock);
    close(recv_sock);

    return 0;
}
