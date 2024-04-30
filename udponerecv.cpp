#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOCAL_IP "192.168.1.3"
#define LOCAL_PORT 7676
#define MAX_BUFFER_SIZE 1024

int main() {
    // 创建接收端的socket
    int recv_sock = socket(AF_INET, SOCK_DGRAM, 0);

    // 设置接收端的地址
    sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 任意地址
    recv_addr.sin_port = htons(LOCAL_PORT);

    // 绑定接收端的地址
    bind(recv_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr));

    struct sockaddr_in sender_address;
    socklen_t sender_address_len = sizeof(sender_address);
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // 接收数据
    ssize_t recv_len = recvfrom(recv_sock, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&sender_address, &sender_address_len);

    // 打印接收到的数据
    if (recv_len > 0) {
        std::cout << "Recv Socket: " << recv_sock << std::endl;
        std::cout << "Received: " << std::string(buffer, recv_len) << std::endl;

        // 将源地址转换成字符串形式
        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sender_address.sin_addr), sender_ip, INET_ADDRSTRLEN);
        std::cout << "IP: " << sender_ip << ", Port: " << ntohs(sender_address.sin_port) << std::endl;

        // 发送回源地址
        sendto(recv_sock, buffer, recv_len, 0, (struct sockaddr *)&sender_address, sender_address_len);
    }

    // 关闭socket
    close(recv_sock);

    return 0;
}
